/*
	RK05toDsk.c
	Copyright (C) 2016-2017, 2022, 2025. Gerardo Ospina

	This program creates a dsk Solo file image from a rk05 Solo file image

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
#include "DskRK05Map.h"

static void read_rk05(FILE *rk05_file, unsigned int *n_rk05_pages, TPage *rk05_pages)
{
	int k;
	unsigned int i;

	*n_rk05_pages = 0;
	for (i = 0; i < DISK_PAGES; i++)
	{
		k = fread(rk05_pages[i], 1, PAGE_LENGTH, rk05_file);
		if (k == PAGE_LENGTH) 
		{
			(*n_rk05_pages)++;
		}
		else
		{
			printf("Unable to read rk05 page %d (%d != %d)\n", i, k, PAGE_LENGTH);
		}
	}
}

void create_dsk(char *filename, TPage *rk05_pages)
{
	FILE *dsk_file;
	char name[1024];

	strcpy(name, filename);
	strcat(name, ".dsk");
	dsk_file = fopen(name, "wb");
	if (dsk_file == NULL)
	{
		printf("Error creating dsk image: %s\n", filename);
	}
	else
	{
		int i, k;

		printf("Removable Pack Disk Image: %s\n", name);
		for (i = 0; i < DISK_PAGES; i++)
		{
			k = fwrite(rk05_pages[dsk_rk05_map[i]], 1, PAGE_LENGTH, dsk_file);
			if (k != PAGE_LENGTH)
			{
				printf("Unable to write dsk page %d (%d != %d)\n", i, k, PAGE_LENGTH);
			}
		}
		fclose(dsk_file);
	}
}

int main(int argc, char *argv[])
{
	if ((argc != 2) || (argv[0] == NULL))
	{
		printf("Usage is: RK05toDsk <RK05 image>\n");
	}
	else
	{
		FILE *rk05_file;

		rk05_file = fopen(argv[1], "rb");
		if (rk05_file == NULL)
		{
			printf("Error opening RK05 image: %s\n", argv[1]);
		}
		else
		{
			unsigned int n_rk05_pages;
			TPage rk05_pages[DISK_PAGES];

			read_rk05(rk05_file, &n_rk05_pages, rk05_pages);
			fclose(rk05_file);
			if (n_rk05_pages == DISK_PAGES)
			{
				printf("RK05 Removable Pack Disk Image: %s\n", argv[1]);
				create_dsk(argv[1], rk05_pages);
			}
			else
			{
				printf("Error: (%s) Invalid RK05 image\n", argv[1]);
			}
		}
	}
	return EXIT_SUCCESS;
}
