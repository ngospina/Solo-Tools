/* 
	DskMatch.c
	Copyright (C) 2011-2017. Gerardo Ospina

	This program matches the pages of a .dsk SOLO disk image and any other
	SOLO disk image

	Permission is hereby granted, free of charge, to any person obtaining a
	copy of this software and associated documentation files (the "Software"),
	to deal in the Software without restriction, including without limitation
	the rights to use, copy, modify, merge, publish, distribute, sublicense,
	and/or sell copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
	GERARDO OSPINA BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
	IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
	CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

	Except as contained in this notice, the name of Gerardo Ospina shall not
	be used in advertising or otherwise to promote the sale, use or other dealings
	in this Software without prior written authorization from Gerardo Ospina.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "LCS.h"

#define PAGE_LENGTH	    512
#define PAGEMAP_LENGTH	(PAGE_LENGTH / sizeof(unsigned short))

#define ID_LENGTH		 12
#define FILEMAP_LENGTH	(PAGE_LENGTH / sizeof(TFileEntry))

#define CATALOG_PAGE	154
#define PATH_LENGTH		1024
#define DISK_PAGES		4800

#define EMPTY	0
#define SCRATCH	1
#define ASCII	2
#define SEQCODE	3
#define CONCODE	4

typedef unsigned char	TPage[PAGE_LENGTH];
typedef unsigned short	TPageMap[PAGEMAP_LENGTH];

typedef union {
	TPage page;
	TPageMap pagemap;
} TFile;

typedef struct {
	unsigned char id[ID_LENGTH];
	unsigned short kind, addr, protect;
	unsigned short unused[5];
	unsigned short key;
	unsigned short searchlength;
} TFileEntry;

typedef TFileEntry TFileMap[FILEMAP_LENGTH];

typedef union {
	TPage page;
	TFileMap filemap;
} TCatalog;

#define DELTA		0

static TPage dsk_pages[DISK_PAGES];
static TPage image_pages[DISK_PAGES];

static size_t lcs[DISK_PAGES];
static size_t lcs_delta[DISK_PAGES];

static char dsk_name[PATH_LENGTH];
static char image_name[PATH_LENGTH];

static unsigned int dsk_map[DISK_PAGES][2];
static unsigned int dsk_list[DISK_PAGES];

static unsigned int image_dsk_map[DISK_PAGES];
static unsigned int dsk_image_map[DISK_PAGES][2];

static char path[PATH_LENGTH];

static void MakePath(char* filepath)
{
	int i;

	strcpy(path, filepath);
	for (i = strlen(path); i >= 0 && path[i] != '\\'; i--);
	if (i >= 0)
	{
		for (i = i - 1; i >= 0 && path[i] != '\\'; i--);
		if (i >= 0)
		{
			for (i = i - 1; i >= 0 && path[i] != '\\'; i--);
		}
	}
	path[i + 1] = '\0';
}

static void write_dsk_map(FILE *out)
{
	unsigned int page;

	fprintf(out, "/*\n\tDskImageMap.h\n*/\n\n");
	fprintf(out, "#ifndef _DSKIMAGEMAP_H_\n\n");
	fprintf(out, "#define _DSKIMAGEMAP_H_\n\n");
	fprintf(out, "static int dsk_image_map[%d] = {", DISK_PAGES);
	for (page = 0; page < DISK_PAGES; page++)
	{
		if (page % 12 == 0)
		{
			fprintf(out, "\n/*%4d*/\t", page);
		}
		fprintf(out, "%4d", dsk_image_map[page][0]);
		if (page != DISK_PAGES - 1)
		{
			fprintf(out, ",");
		}
	}
	fprintf(out, "\n};\n\n");
	fprintf(out, "static int dsk_image_LCS_length[%d] = {", DISK_PAGES);
	for (page = 0; page < DISK_PAGES; page++)
	{
		if (page % 12 == 0)
		{
			fprintf(out, "\n/*%4d*/\t", page);
		}
		fprintf(out, "%4d", dsk_image_map[page][1]);
		if (page != DISK_PAGES - 1)
		{
			fprintf(out, ",");
		}
	}
	fprintf(out, "\n};\n\n");
	fprintf(out, "#endif\n");
}

static void write_geometry(FILE *file, unsigned int page)
{
	unsigned int surface, cylinder, sector;

	surface = (page / 12) % 2;
	cylinder = page / 24;
	sector = page % 12;
	fprintf(file, "(H:%1d C:%3d S:%2d)", surface, cylinder, sector);
}

static void write_dsk_image_map(FILE *file)
{
	unsigned int page;

	for (page = 0; page < DISK_PAGES; page++)
	{
		if (dsk_image_map[page][0] != DISK_PAGES)
		{
			fprintf(file, "%4d ", page);
			write_geometry(file, page);
			fprintf(file, " %4d ", dsk_image_map[page][0]);
			write_geometry(file, dsk_image_map[page][0]);
			fprintf(file, "\n");
		}
	}
}

static unsigned int match_page(FILE *out, unsigned int dsk_page_number, unsigned int page_map)
{
	unsigned int image_page_number = dsk_image_map[dsk_page_number][0];

	if (image_page_number == DISK_PAGES)
	{
		unsigned int i, j;
		TPage *dsk_page = (TPage *) (dsk_pages + dsk_page_number);
		size_t len, max_lcs = 0;

		printf("Match page: %4d\n", dsk_page_number);
		for (i = 0; i < DISK_PAGES; i++)
		{
			if (image_dsk_map[i] == DISK_PAGES)
			{
				len = LCSLength(image_pages[i], *dsk_page, PAGE_LENGTH, PAGE_LENGTH);
				if (len > max_lcs)
				{
					max_lcs = len;
				}
				lcs[i] = len;
			}
			else
			{
				lcs[i] = 0;
			}
		}
		for (i = 0; i < DISK_PAGES; i++)
		{
			lcs_delta[i] = max_lcs - lcs[i];
		}
		for (i = 0; i < DISK_PAGES; i++)
		{
			if (lcs_delta[i] <= DELTA)
			{
				if (dsk_map[dsk_page_number][0] == DISK_PAGES)
				{
					image_page_number = i;
					dsk_map[dsk_page_number][0] = i;
				}
				else
				{
					dsk_list[dsk_map[dsk_page_number][1]] = i;
				}
				dsk_map[dsk_page_number][1] = i;
				dsk_list[i] = DISK_PAGES;
				fprintf(out, "%4d,%4d,%4d\n>", dsk_page_number, i, lcs[i]);
				for (j = 0; j < PAGE_LENGTH; j++)
				{
					if ((*dsk_page)[j] >= ' ' && (*dsk_page)[j] <= '~')
					{
						fprintf(out, "%c", (*dsk_page)[j]);
					}
					else
					{
						fprintf(out, ".");
					}
				}
				fprintf(out, "\n>");
				for (j = 0; j < PAGE_LENGTH; j++)
				{
					if (image_pages[i][j] >= ' ' && image_pages[i][j] <= '~')
					{
						fprintf(out, "%c", image_pages[i][j]);
					}
					else
					{
						fprintf(out, ".");
					}
				}
				fprintf(out, "\n\n");
			}
		}
		if (dsk_map[dsk_page_number][1] == image_page_number)
		{
			image_dsk_map[image_page_number] = dsk_page_number;
			dsk_image_map[dsk_page_number][0] = image_page_number;
			dsk_image_map[dsk_page_number][1] = lcs[image_page_number];
		}
	}
	return image_page_number;
}

static void match_file(FILE *files[6], char * name, int dsk_page, int map_page)
{
	TFile *dsk_file, *image_file;
	TPage *dsk_datapage, *image_datapage;
	unsigned int i, image_page, image_pagemap;

	dsk_file = (TFile *) (dsk_pages + dsk_page);
	image_page = match_page(files[3], dsk_page, map_page);
	do
	{
		fprintf(files[2], "    addr=%4d (%4d, %4d) %s\n", dsk_page, map_page, image_page, name);
		image_file = (TFile *)(image_pages + image_page);
		fprintf(files[2], "      Number of pages: %d (%d)", dsk_file->pagemap[0], image_file->pagemap[0]);
		if (dsk_file->pagemap[0] == image_file->pagemap[0])
		{
			fprintf(files[2], "\n");
			for (i = 1; i <= dsk_file->pagemap[0]; i++)
			{
				fprintf(files[2], "        pagemap[%2d] = %4d (%4d", i, dsk_file->pagemap[i], image_file->pagemap[i]);
				if (dsk_file->pagemap[i] == image_file->pagemap[i])
				{
					dsk_datapage = (TPage *)(dsk_pages + dsk_file->pagemap[i]);
					image_pagemap = match_page(files[3], dsk_file->pagemap[i], image_file->pagemap[i]);
					fprintf(files[2], ":");
					do
					{
						fprintf(files[2], "%5d", image_pagemap);
						image_datapage = (TPage *)(image_pages + image_pagemap);
						image_pagemap = dsk_list[image_pagemap];
					}
					while (image_pagemap != DISK_PAGES);
					fprintf(files[2], ")");
				}
				else
				{
					fprintf(files[2], ") !");
				}
				fprintf(files[2], "\n");
			}
		}
		else
		{
			fprintf(files[2], " !");
		}
		fprintf(files[2], "\n");
		image_page = dsk_list[image_page];
	} 
	while (image_page != DISK_PAGES);
}

static void get_dsk_name(TFileEntry dsk_entry)
{
	unsigned int i, j;

	j = 0;
	for (i = 0; i < ID_LENGTH; i++)
	{
		if (dsk_entry.id[i] != ' ')
		{
			dsk_name[j] = dsk_entry.id[i];
			j++;
		}
	}
	dsk_name[j] = '\0';
}

static void get_image_name(TFileEntry image_entry)
{
	unsigned int i, j;

	j = 0;
	for (i = 0; i < ID_LENGTH; i++)
	{
		if (image_entry.id[i] != ' ')
		{
			image_name[j] = image_entry.id[i];
			j++;
		}
	}
	image_name[j] = '\0';
}

static void match_entry(FILE *files[6], TFileEntry dsk_entry, TFileEntry image_entry)
{
	if (dsk_entry.id[0] != ' ')
	{
		get_dsk_name(dsk_entry);
		get_image_name(image_entry);
		if (strcmp(dsk_name, image_name) == 0)
		{
			match_file(files, dsk_name, dsk_entry.addr, image_entry.addr);
		}
	}
}

static void match_catalog_page(FILE *files[6], unsigned short dsk_page)
{
	TCatalog *dsk_catalog, *image_catalog;
	unsigned int i, j, image_page = DISK_PAGES;

	image_page = match_page(files[3], dsk_page, dsk_page);
	dsk_catalog = (TCatalog *) (dsk_pages + dsk_page);
	fprintf(files[2], "\n");
	do 
	{
		image_catalog = (TCatalog *)(image_pages + image_page);
		fprintf(files[2], "  File Catalog %d (%d)\n", dsk_page, image_page);
		for (i = 0; i < FILEMAP_LENGTH; i++)
		{
			for (j = 0; j < FILEMAP_LENGTH; j++)
			{
				match_entry(files, dsk_catalog->filemap[i], image_catalog->filemap[j]);
			}
		}
		image_page = dsk_list[image_page];
	}
	while (image_page != DISK_PAGES);
}

static void match_dsk(FILE *files[6])
{
	TFile *dsk_catalog, *image_catalog;
	unsigned int i, image_page;

	create_L(PAGE_LENGTH, PAGE_LENGTH);
	image_page = match_page(files[3], CATALOG_PAGE, CATALOG_PAGE);
	dsk_catalog = (TFile *) (dsk_pages + CATALOG_PAGE);
	do
	{
		image_catalog = (TFile *)(image_pages + image_page);
		fprintf(files[2], "Disk Catalog %d (%d)\n", CATALOG_PAGE, image_page);
		fprintf(files[2], "Number of pages: %d (%d)", dsk_catalog->pagemap[0], image_catalog->pagemap[0]);
		if (dsk_catalog->pagemap[0] == image_catalog->pagemap[0])
		{
			fprintf(files[2], "\n");
			for (i = 1; i <= dsk_catalog->pagemap[0]; i++)
			{
				fprintf(files[2], "  Catalog pagemap[%2d] = %d (%d)", i, dsk_catalog->pagemap[i], image_catalog->pagemap[i]);
				if (dsk_catalog->pagemap[i] == image_catalog->pagemap[i])
				{
					match_catalog_page(files, dsk_catalog->pagemap[i]);
				}
				else
				{
					fprintf(files[2], " !");
				}
			}
		}
		else
		{
			fprintf(files[2], " !");
		}
		fprintf(files[2], "\n");
		image_page = dsk_list[image_page];
	}
	while (image_page != DISK_PAGES);
	destroy_L();
}

static void read_dsk_file(FILE *in, unsigned int *n_pages)
{
	int k;
	unsigned int i;
	
	*n_pages = 0;
	for (i = 0; i < DISK_PAGES; i++)
	{
		dsk_map[i][0] = DISK_PAGES;
		dsk_map[i][1] = 0;
		dsk_list[i] = DISK_PAGES;
		dsk_image_map[i][0] = DISK_PAGES;
		dsk_image_map[i][1] = 0;
		k = fread(dsk_pages[i], 1, PAGE_LENGTH, in);
		if (k == PAGE_LENGTH)
		{
			(*n_pages)++;
		}
		else
		{
			printf("Unable to read dsk image page %d (%d != %d)\n", i, k, PAGE_LENGTH);
		}
	}
}

static void read_image_file(FILE *in, unsigned int *n_pages)
{
	int k;
	unsigned int i;

	*n_pages = 0;
	for (i = 0; i < DISK_PAGES; i++)
	{
		image_dsk_map[i] = DISK_PAGES;
		k = fread(image_pages[i], 1, PAGE_LENGTH, in);
		if (k == PAGE_LENGTH)
		{
			(*n_pages)++;
		}
		else
		{
			printf("Unable to read image page %d (%d != %d)\n", i, k, PAGE_LENGTH);
		}
	}
}

static unsigned int openFiles(FILE *files[6], char *dsk_filename, char *image_filename)
{
	char filename[1024];
	unsigned int i;
	char *filenames[6] = { dsk_filename, image_filename, "catalogMatch.txt", "dataMatch.txt", "DskImageMap.h", "DskImageMap.txt" };
	unsigned int rc = 1;

	for (i = 0; rc == 1 && i < 6; i++) {
		strcpy(filename, path);
		strcat(filename, filenames[i]);
		files[i] = fopen(filename, i < 2 ? "rb" : "wb");
		if (files[i] == NULL)
		{
			printf("Error %s file: %s\n", i < 2 ? "creating" : "opening", filename);
			rc = 0;
		}
	}
	return rc;
}

static void closeFiles(FILE *files[6])
{
	unsigned int i;

	for (i = 0; i < 6; i++)
	{
		if (files[i] != NULL)
		{
			fclose(files[i]);
			files[i] = NULL;
		}
	}
}

int main(int argc, char *argv[])
{
	unsigned int n_dsk_pages, n_image_pages;
	FILE *files[6] = { NULL, NULL, NULL, NULL, NULL, NULL };

	if ((argc != 3) || (argv[0] == NULL))
	{
		printf("Usage is: DskMatch <dsk image> <image>\n");
	}
	else
	{
		if (openFiles(files, argv[1], argv[2]))
		{
			read_dsk_file(files[0], &n_dsk_pages);
			if (n_dsk_pages == DISK_PAGES)
			{
				read_image_file(files[1], &n_image_pages);
				if (n_image_pages == DISK_PAGES)
				{
					MakePath(argv[1]);
					printf("Removable Pack Disks: %s %s\n", argv[1], argv[2]);
					fprintf(files[2], "Removable Pack Disks: %s %s\n", argv[1], argv[2]);
					fprintf(files[3], "Removable Pack Disks: %s %s\n\n", argv[1], argv[2]);
					match_dsk(files);
					write_dsk_map(files[4]);
					write_dsk_image_map(files[5]);
				}
			}
		}
		closeFiles(files);
	}
	return EXIT_SUCCESS;
}