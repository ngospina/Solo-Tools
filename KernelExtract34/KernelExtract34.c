/*
	KernelExtract34.c
	Copyright (c) 2016, 2022. Gerardo Ospina

	This program extracts PDP-11/34 Concurrent Pascal Kernel Text

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

#if defined(_WIN32) || defined(WIN32)
#define SEPARATOR '\\'
#else
#define SEPARATOR '/'
#endif

#define LENGTH		1024
#define PATH_LENGTH	1024

static void make_out_path(unsigned char *in_path, unsigned char *name, unsigned char *out_path)
{
	int i;
	unsigned int len = strlen(in_path);

	strcpy(out_path, in_path);
	for (i = len - 1; i >= 0 && out_path[i] != SEPARATOR; i--);
	if (i >= 0)
	{
		out_path[i + 1] = '\0';
	}
	else
	{
		strcpy(out_path, ". ");
		out_path[1] = SEPARATOR;
	}
	strcat(out_path, name);
}

static void read_line(FILE *ifile, unsigned char *line, unsigned int *lineno)
{
	memset(line, '\0', LENGTH);
	fgets(line, LENGTH, ifile);
	if (line[0] == 12)
	{
		memmove(line, line + 1, strlen(line));
	}
	(*lineno)++;
}

static void write_line(unsigned char *line, unsigned int lineno, FILE *ofile)
{
	unsigned char *p;
	int count = 0;

	for (p = line; *p; p++)
	{
		if (*p > ' ' && *p <= '~')
		{
			count++;
		}
	}
	if (count > 0)
	{
		fprintf(ofile, "%s", line);
	}
}

static void adjust_line(unsigned char *line)
{
	unsigned char *p;

	p = strstr(line, "<01>");
	while (p != NULL)
	{
		memmove(p + 2, p + 4, strlen(p) - 3);
		strncpy(p, "  ", 2);
		p = strstr(line, "<01>");
	}
}

static void extract_kernel1(FILE *ifile, FILE *ofile, unsigned char *line, unsigned int *lineno)
{
	int extract = 1;

	read_line(ifile, line, lineno);
	while (extract && !feof(ifile))
	{
		extract = strstr(line, "KERNELTEXT2") == NULL;
		if (extract)
		{
			adjust_line(line);
			if ((*lineno >= 137 && *lineno <= 150) ||
				(*lineno >= 155 && *lineno <= 170) ||
				*lineno == 1011 || *lineno == 1088 || *lineno == 1259 || *lineno == 1289 ||
				*lineno == 1333 || *lineno == 1522 || *lineno == 1551)
			{
				if (*lineno == 137 || *lineno == 1011 || *lineno == 1088 ||
					*lineno == 1259 || *lineno == 1289 || *lineno == 1333 || *lineno == 1522 ||
					*lineno == 1551)
				{
					write_line("\"\n", *lineno, ofile);
				}
				write_line(line + 1, *lineno, ofile);
				if (*lineno == 170 || *lineno == 1011 ||  *lineno == 1088 ||
					*lineno == 1259 || *lineno == 1289 || *lineno == 1333 || *lineno == 1522 ||
					*lineno == 1551)
				{
					write_line("\"\n", *lineno, ofile);
				}
			}
			else if (*lineno == 1525)
			{
				write_line(line + 29, *lineno, ofile);
			}
			else if (*lineno == 1633)
			{
				memmove(line + 57, line + 72, strlen(line) - 71);
				write_line(line + 36, *lineno, ofile);
			}
			else if ((*lineno >= 1015 && *lineno <= 1018) ||
				(*lineno >= 1020 && *lineno <= 1021) ||
				(*lineno >= 1023 && *lineno <= 1024) ||
				(*lineno >= 1026 && *lineno <= 1033) ||
				(*lineno >= 1035 && *lineno <= 1043) ||
				(*lineno >= 1045 && *lineno <= 1054) ||
				(*lineno >= 1056 && *lineno <= 1059) ||
				(*lineno >= 1061 && *lineno <= 1073) ||
				(*lineno >= 1075 && *lineno <= 1105) ||
				(*lineno >= 1107 && *lineno <= 1116) ||
				(*lineno >= 1118 && *lineno <= 1151) ||
				(*lineno >= 1153 && *lineno <= 1159) ||
				(*lineno >= 1161 && *lineno <= 1167) ||
				(*lineno >= 1169 && *lineno <= 1389) ||
				(*lineno >= 1403 && *lineno <= 1507) ||
				(*lineno >= 1509 && *lineno <= 1518) ||
				(*lineno >= 1520 && *lineno <= 1544) ||
				(*lineno >= 1546 && *lineno <= 1582) ||
				(*lineno >= 1584 && *lineno <= 1614) ||
				(*lineno >= 1616 && *lineno <= 1630) ||
				(*lineno >= 1632 && *lineno <= 1633) ||
				(*lineno >= 1638 && *lineno <= 1815) ||
				(*lineno >= 1818 && *lineno <= 1848) ||
				(*lineno >= 1850))
			{
				if (line[39] == ';')
				{
					write_line(line + 40, *lineno, ofile);
				}
				else if (line[35] == ';')
				{
					write_line(line + 36, *lineno, ofile);
				}
			}
			read_line(ifile, line, lineno);
		}
	}
}

static void extract_kernel2(FILE *ifile, FILE *ofile, unsigned char *line, unsigned int *lineno)
{
	int extract = 1;

	read_line(ifile, line, lineno);
	while (extract && !feof(ifile))
	{
		extract = strstr(line, "KERNELTEXT3") == NULL;
		if (extract) {
			adjust_line(line);
			if ((*lineno <= 2111 || *lineno >= 2115) &&
				(*lineno <= 2534 || *lineno >= 2539) &&
				*lineno != 2592)
			{
				if (line[35] == ';')
				{
					write_line(line + 36, *lineno, ofile);
				}
				else if (line[32] == ';')
				{
					write_line(line + 33, *lineno, ofile);
				}
			}
			read_line(ifile, line, lineno);
		}
	}
}

static void extract_kernel3(FILE *ifile, FILE *ofile, unsigned char *line, unsigned int *lineno)
{
	int extract = 1;

	read_line(ifile, line, lineno);
	while (extract && !feof(ifile))
	{
		extract = strstr(line, "KERNELTEXT4") == NULL;
		if (extract) {
			adjust_line(line);		
			if (*lineno == 4537 || *lineno == 4552)
			{
				write_line(line + 22, *lineno, ofile);
			}
			else if (line[35] == ';')
			{
				if (*lineno == 3772)
				{
					memmove(line + 65, line + 64, strlen(line) - 63);
					line[64] = ';';
				}
				write_line(line + 36, *lineno, ofile);
			}
			read_line(ifile, line, lineno);
		}
	}
}

static void extract_interpreter(FILE *ifile, FILE *ofile, unsigned char *line, unsigned int *lineno)
{
	read_line(ifile, line, lineno);
	while (!feof(ifile))
	{
		adjust_line(line);
		if (*lineno >= 4758 && *lineno <= 4762)
		{
			if (*lineno == 4758)
			{
				write_line("\"\n", *lineno, ofile);
			}
			write_line(line + 1, *lineno, ofile);
			if (*lineno == 4762)
			{
				write_line("\"\n", *lineno, ofile);
			}
		}
		else if (*lineno == 5134)
		{
			memmove(line + 53, line + 56, strlen(line) - 55);
			write_line(line + 29, *lineno, ofile);
		}
		else if ((*lineno >= 4696 && *lineno <= 4766) ||
			(*lineno >= 4769 && *lineno <= 4783) ||
			(*lineno >= 4796 && *lineno <= 6213))
		{
			if (line[35] == ';')
			{
				write_line(line + 36, *lineno, ofile);
			}
			else if (line[32] == ';')
			{
				write_line(line + 33, *lineno, ofile);
			}
			else if (line[25] == ';')
			{
				write_line(line + 26, *lineno, ofile);
			}
		}
		read_line(ifile, line, lineno);
	}
}

int main(int argc, char *argv[])
{
	if ((argc != 4) || (argv[0] == NULL))
	{
		printf("Usage is: KernelExtract34 <Merged kernel file> <Kernel file> <Interpreter file>\n");
	}
	else
	{
		FILE *ifile = fopen(argv[1], "rb");

		if (ifile == NULL)
		{
			printf("Error opening file: %s\n", argv[1]);
		}
		else
		{
			FILE *ofile;
			unsigned char out_path[PATH_LENGTH];

			make_out_path(argv[1], argv[2], out_path);
			ofile = fopen(out_path, "wb");
			if (ofile == NULL)
			{
				printf("Error creating file: %s\n", out_path);
			}
			else
			{
				unsigned char line[LENGTH];
				unsigned int lineno = 0;

				extract_kernel1(ifile, ofile, line, &lineno);
				extract_kernel2(ifile, ofile, line, &lineno);
				extract_kernel3(ifile, ofile, line, &lineno);
				fclose(ofile);
				make_out_path(argv[1], argv[3], out_path);
				ofile = fopen(out_path, "wb");
				if (ofile == NULL)
				{
					printf("Error creating file: %s\n", out_path);
				}
				else
				{
					extract_interpreter(ifile, ofile, line, &lineno);
					fclose(ofile);
				}
			}
			fclose(ifile);
		}
	}
	return EXIT_SUCCESS;
}