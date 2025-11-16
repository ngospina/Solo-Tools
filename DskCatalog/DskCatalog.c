/*
	DskCatalog.c
	Copyright (c) 2009-2017, 2025. Gerardo Ospina

	This program prints the catalog of a dsk Solo disk image

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

#include "solo.h"

static void PageMap(FILE *ifile, FILE *ofile, int page)
{
	TFile file;
	int i, k, fpos;

	fpos = page * PAGE_LENGTH;
	fseek (ifile, fpos, SEEK_SET);
	k = fread (file.page, 1, PAGE_LENGTH, ifile);
    if (k != PAGE_LENGTH)
	{
		fprintf(ofile, "      Unable to read removable pack page map\n");
	}
	else
	{
		fprintf(ofile, "      Number of pages: %4d\n", file.pagemap[0]);
		for (i = 1; i <= file.pagemap[0]; i++)
		{
			fprintf(ofile, "        pagemap[%3d] = %4d\n", i, file.pagemap[i]);
		}
	}
}

static void ShowFile(FILE *ifile, FILE *ofile, TFileEntry entry, int *files)
{
	int i, j;
	char name[PATH_LENGTH];
	char *FileKind[] = {"EMPTY", "SCRATCH", "ASCII", "SEQCODE", "CONCODE"};


	if (entry.id[0] != ' ')
	{
		j = 0;
		for(i = 0; i < ID_LENGTH; i++)
		{
			if (entry.id[i] != ' ')
			{
				name[j] = entry.id[i];
				j++;
			}
		}
		name[j] = '\0';
		fprintf(ofile, "    (%4d) [%7s] %s\n", entry.addr, FileKind[entry.kind], name);
		PageMap(ifile, ofile, entry.addr);
		(*files)++;
	}
}

static void CatalogPage(FILE *ifile, FILE *ofile, unsigned short page, int *files)
{
	TCatalog catalog;
	int i, k, fpos;

	fpos = page * PAGE_LENGTH;
	fseek (ifile, fpos, SEEK_SET);
	k = fread (catalog.page, 1, PAGE_LENGTH, ifile);
	for (i = 0; i < FILEMAP_LENGTH; i++)
	{
		ShowFile(ifile, ofile, catalog.filemap[i], files);
	}
}

static void Catalog(FILE *ifile, FILE *ofile)
{
	TFile catalog;
	int i, k, fpos;
	int files = 0;


	fpos = CATALOG_PAGE * PAGE_LENGTH;
	fseek (ifile, fpos, SEEK_SET);
	k = fread (catalog.page, 1, PAGE_LENGTH, ifile);
    if (k != PAGE_LENGTH)
	{
		printf("Unable to read removable pack catalog %d\n", CATALOG_PAGE);
	}
	else
	{
		fprintf(ofile, "Number of pages: %4d\n", catalog.pagemap[0]);
		for (i = 1; i <= catalog.pagemap[0]; i++)
		{
			fprintf(ofile, "  pagemap[%3d] = %4d\n", i, catalog.pagemap[i]);
			CatalogPage(ifile, ofile, catalog.pagemap[i], &files);
		}
		fprintf(ofile, "Number of files: %4d\n", files);
	}
}

int main(int argc, char *argv[])
{
	FILE *ifile, *ofile;
	char name[1024];

	if ((argc != 2) || (argv[0] == NULL))
	{
		printf ("Usage is: DskCatalog <dsk image>\n");
	}
	else
	{
		ifile = fopen (argv[1], "rb");
		if (ifile == NULL) 
		{
			printf ("Error opening dsk image file: %s\n", argv[1]);
		}
		else
		{
			sprintf(name, "catalog.%s.txt", argv[1]);
			ofile = fopen(name, "wb");
			if (ofile == NULL)
			{
				printf("Error creating catalog file: %s\n", name);
			}
			else
			{
				fprintf(ofile, "Removable Pack Disk: %s\n", argv[1]);
				Catalog(ifile, ofile);
				fclose(ofile);
			}
			fclose(ifile);
		}
	}
	return EXIT_SUCCESS;
}