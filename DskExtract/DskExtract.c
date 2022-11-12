/*
	DskExtract.c
	Copyright (c) 2009-2021. Gerardo Ospina

	This program extracts data from a simulated SOLO removable pack disk

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

/*
    Requires the following directories defined in the directory that contains
    the SOLO disk image to be processed

    +--Files
    |  +--ASCII
    |  +--CONCODE
    |  +--SEQCODE
    |  +--SCRATCH
    +--Segments
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "SOLO.h"

#if defined(_WIN32) || defined(WIN32)
#define SEPARATOR '\\'
#else
#define SEPARATOR '/'
#endif


static char path[PATH_LENGTH];
static char filename[PATH_LENGTH];
static char *FileKind[] = {"EMPTY", "SCRATCH", "ASCII", "SEQCODE", "CONCODE"};

static void ExtractPath(char* filepath)
{
	int i;

	strcpy(path, filepath);
	for (i = strlen(path); i >= 0 && path[i] != SEPARATOR; i--);
    if (i == -1)
    {
        strcpy(path, ". ");
        path[1] = SEPARATOR;
    }
    else
    {
    	path[i + 1] = '\0';
    }
}

static void MakeSegmentName(char *name)
{
	strcpy(filename, path);
	strcat(filename, "Segments ");
    filename[strlen(filename) - 1] = SEPARATOR;
	strcat(filename, name);
}

static void CopySegment(FILE *ifile, char *name, unsigned int begin, unsigned int end)
{
	FILE *ofile;
	TPage datapage;
	unsigned int i, k;

	ofile = fopen(name, "w+b");
	if (ofile == NULL)
	{
		printf("      Error creating segment file: %s\n", name);
	}
	else
	{
		for (i = begin; i < end; i++)
		{
			k = fread(datapage, 1, PAGE_LENGTH, ifile);
			if (k != PAGE_LENGTH)
			{
				printf("      Unable to read segment data page %d\n", i);
			}
			else
			{
				k = fwrite(datapage, 1, PAGE_LENGTH, ofile);
				if (k != PAGE_LENGTH)
				{
					printf("      Unable to write segment data page\n");
				}
			}
		}
		fclose(ofile);
	}
}

static void CopyFile(FILE *ifile, char *name, int page)
{
	FILE *ofile;
	TFile file;
	TPage datapage;
	unsigned int i, k, fpos;

	fpos = page * PAGE_LENGTH;
	fseek (ifile, fpos, SEEK_SET);
	k = fread (file.page, 1, PAGE_LENGTH, ifile);
	if (k != PAGE_LENGTH)
	{
		printf("    Unable to read file page map %d\n", page);
	}
	else
	{
		ofile = fopen (name, "w+b");
		if (ofile == NULL) 
		{
			printf ("      Error creating file: %s\n", name);
		}
		else
		{
			for (i = 1; i <= file.pagemap[0]; i++)
			{
				fpos = file.pagemap[i] * PAGE_LENGTH;
				fseek(ifile, fpos, SEEK_SET);
				k = fread (datapage, 1, PAGE_LENGTH, ifile);
				if (k != PAGE_LENGTH)
				{
					printf("      Unable to read file data page %d\n", file.pagemap[i]);
				}
				else
				{
					k = fwrite(datapage, 1, PAGE_LENGTH, ofile);
					if (k != PAGE_LENGTH)
					{
						printf("      Unable to write file data page\n");
					}
				}
			}
			fclose(ofile);
		}
	}
}

static void MakeFileName(char *type, char *name)
{
	strcpy(filename, path);
	strcat(filename, "Files ");
    filename[strlen(filename) - 1] = SEPARATOR;
	strcat(filename, type);
	strcat(filename, " ");
    filename[strlen(filename) - 1] = SEPARATOR;
	strcat(filename, name);
}

static void Kernel(FILE *ifile)
{
	unsigned fpos;

	fpos = KERNEL_PAGE * PAGE_LENGTH;
	fseek(ifile, fpos, SEEK_SET);
	MakeSegmentName("KERNEL");
	CopySegment(ifile, filename, KERNEL_PAGE, SOLO_PAGE);
}

static void SOLO(FILE *ifile)
{
	unsigned int fpos;

	fpos = SOLO_PAGE * PAGE_LENGTH;
	fseek(ifile, fpos, SEEK_SET);
	MakeSegmentName("SOLO");
	CopySegment(ifile, filename, SOLO_PAGE, OTHER_OS_PAGE);
}

static void otherOS(FILE *ifile)
{
	unsigned int fpos;

	fpos = OTHER_OS_PAGE * PAGE_LENGTH;
	fseek(ifile, fpos, SEEK_SET);
	MakeSegmentName("OTHEROS");
	CopySegment(ifile, filename, OTHER_OS_PAGE, CATALOG_PAGE - 2);
}

static void ExtractFile(FILE *ifile, TFileEntry entry)
{
	int i, j;
	char name[PATH_LENGTH];

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
		MakeFileName(FileKind[entry.kind], name);
		CopyFile(ifile, filename, entry.addr);
	}
}

static void CatalogPage(FILE *ifile, unsigned short page)
{
	TCatalog catalog;
	unsigned int i, k, fpos;

	fpos = page * PAGE_LENGTH;
	fseek (ifile, fpos, SEEK_SET);
	k = fread (catalog.page, 1, PAGE_LENGTH, ifile);
	if (k != PAGE_LENGTH)
	{
		printf("      Unable to read catalog page %d\n", page);
	}
	else
	{
		for (i = 0; i < FILEMAP_LENGTH; i++)
		{
			ExtractFile(ifile, catalog.filemap[i]);
		}
	}
}

static void Catalog(FILE *ifile)
{
	TFile catalog;
	int i,k,fpos;

	fpos = CATALOG_PAGE * PAGE_LENGTH;
	fseek (ifile, fpos, SEEK_SET);
	k = fread (catalog.page, 1, PAGE_LENGTH, ifile);
    if (k != PAGE_LENGTH)
	{
		printf("Unable to read removable pack catalog %d\n", CATALOG_PAGE);
	}
	else
	{
		for (i = 1; i <= catalog.pagemap[0]; i++)
		{
			CatalogPage(ifile, catalog.pagemap[i]);
		}
	}
}

int main(int argc, char *argv[])
{
	FILE *ifile;

	if ((argc != 2) || (argv[0] == NULL))
	{
		printf ("Usage is: DskExtract <dsk image>\n");
	}
	else
	{
		ifile = fopen (argv[1], "rb");
		if (ifile == NULL) 
		{
			printf ("Error opening dsk image: %s\n", argv[1]);
		}
		else
		{
			ExtractPath(argv[1]);
			Kernel(ifile);
			SOLO(ifile);
			otherOS(ifile);
			Catalog(ifile);
			fclose(ifile);
		}
	}
	return EXIT_SUCCESS;
}