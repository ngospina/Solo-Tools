/*
	KernelExtract45.c
	Copyright (c) 2016, 2022. Gerardo Ospina

	This program extracts PDP-11/45 Concurrent Pascal Kernel Text

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

static void write_line(unsigned char *line, FILE *ofile)
{
	char *p;
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

static void extract_kernel1(FILE *ifile, FILE *ofile, unsigned char *line, unsigned int *lineno)
{
	int extract = 1;

	read_line(ifile, line, lineno);
	while (extract && !feof(ifile))
	{
		extract = strstr(line, "KERNELTEXT2") == NULL;
		if (extract)
		{
			if ((*lineno >= 35 && *lineno <= 48) ||
				(*lineno >= 53 && *lineno <= 67) ||
				*lineno == 893 || *lineno == 970 || *lineno == 1141 || *lineno == 1173 ||
				*lineno == 1218 ||*lineno == 1296 || *lineno == 1323)
			{
				if (*lineno == 35 || *lineno == 893 || *lineno == 970 || *lineno == 1141 ||
					*lineno == 1173 || *lineno == 1218 || *lineno == 1296 || *lineno == 1323)
				{
					write_line("\"\n", ofile);
				}
				write_line(line + 1, ofile);
				if (*lineno == 67 || *lineno == 893 || *lineno == 970 || *lineno == 1141 ||
					*lineno == 1173 || *lineno == 1218 || *lineno == 1296 || *lineno == 1323)
				{
					write_line("\"\n", ofile);
				}
			}
			else if (*lineno == 1396)
			{
				memmove(line + 57, line + 72, strlen(line) - 71);
				write_line(line + 36, ofile);
			}
			else if ((*lineno >= 897 && *lineno <= 900) ||
				(*lineno >= 902 && *lineno <= 903) ||
				(*lineno >= 905 && *lineno <= 906) ||
				(*lineno >= 908 && *lineno <= 915) ||
				(*lineno >= 917 && *lineno <= 925) ||
				(*lineno >= 927 && *lineno <= 936) ||
				(*lineno >= 938 && *lineno <= 941) ||
				(*lineno >= 943 && *lineno <= 955) ||
				(*lineno >= 957 && *lineno <= 987) ||
				(*lineno >= 989 && *lineno <= 998) ||
				(*lineno >= 1000 && *lineno <= 1033) ||
				(*lineno >= 1035 && *lineno <= 1041) ||
				(*lineno >= 1043 && *lineno <= 1049) ||
				(*lineno >= 1051 && *lineno <= 1265) ||
				(*lineno >= 1267 && *lineno <= 1270) ||
				(*lineno >= 1274 && *lineno <= 1277) ||
				(*lineno >= 1282 && *lineno <= 1286) ||
				(*lineno >= 1288 && *lineno <= 1292) ||
				(*lineno >= 1294 && *lineno <= 1316) ||
				(*lineno >= 1318 && *lineno <= 1354) ||
				(*lineno >= 1356 && *lineno <= 1381) ||
				(*lineno >= 1383 && *lineno <= 1393) ||
				(*lineno >= 1401 && *lineno <= 1576) ||
				(*lineno >= 1579 && *lineno <= 1609) ||
				(*lineno >= 1611))
			{
				if (line[39] == ';')
				{
					write_line(line + 40, ofile);
				}
				else if (line[35] == ';')
				{
					write_line(line + 36, ofile);
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
			if ((*lineno <= 1872 || *lineno >= 1876) &&
				(*lineno <= 2295 || *lineno >= 2300)) 
			{
				if (line[35] == ';')
				{
					write_line(line + 36, ofile);
				}
				else if (line[32] == ';')
				{
					write_line(line + 33, ofile);
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
			if (line[35] == ';')
			{
				write_line(line + 36, ofile);
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
		if (*lineno >= 4478 && *lineno <= 4483)
		{
			if (*lineno == 4478)
			{
				write_line("\"\n", ofile);
			}
			write_line(line + 1, ofile);
			if (*lineno == 4483)
			{
				write_line("\"\n", ofile);
			}
		}
		else if ((*lineno >= 4415 && *lineno <= 4487) ||
			(*lineno >= 4490 && *lineno <= 4504) ||
			(*lineno >= 4517 && *lineno <= 5879))
		{
			if (line[35] == ';')
			{
				write_line(line + 36, ofile);
			}
			else if (line[32] == ';')
			{
				write_line(line + 33, ofile);
			}
			else if (line[25] == ';')
			{
				write_line(line + 26, ofile);
			}
		}
		read_line(ifile, line, lineno);
	}
}

int main(int argc, char *argv[])
{
	if ((argc != 4) || (argv[0] == NULL))
	{
		printf("Usage is: KernelExtract45 <Merged kernel file> <Kernel file> <Interpreter file>\n");
	}
	else
	{
		FILE *ifile;

		ifile = fopen(argv[1], "rb");
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