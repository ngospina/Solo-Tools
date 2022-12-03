/*
	KernelMerge45.c
	Copyright (c) 2016, 2022. Gerardo Ospina

	This program merges in a single file the PDP-11/45 kernel source files

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

static void make_paths(unsigned char *path, unsigned char *out_path)
{
	unsigned int len = strlen(path);

	strcpy(out_path, path);
	if (len != 0 && path[len - 1] != '\\')
	{
		strcat(out_path, " ");
		out_path[len] = SEPARATOR;
	}
}

static void close_files(FILE *ifile[4], FILE **ofile)
{
	unsigned int i;

	for (i = 0; i < 4; i++)
	{
		if (ifile[i] != NULL)
		{
			fclose(ifile[i]);
			ifile[i] = NULL;
		}
	}
	for (i = 0; i < 2; i++)
	{
		if (*ofile != NULL)
		{
			fclose(*ofile);
			*ofile = NULL;
		}
	}
}

static int open_files(FILE *ifile[4], FILE **ofile, unsigned char *name, unsigned char *out_path, unsigned char *names[])
{
	unsigned int i;
	unsigned int opened = 0;
	char filename[PATH_LENGTH];

	for (i = 0; i < 4; i++)
	{
		strcpy(filename, out_path);
		strcat(filename, names[i]);
		ifile[i] = fopen(filename, "rb");
		if (ifile[i] == NULL)
		{
			printf("Error opening file: %s\n", filename);
		}
		else
		{
			opened++;
		}
	}
	if (opened == 4)
	{
		strcpy(filename, out_path);
		strcat(filename, name);
		*ofile = fopen(filename, "wb");
		if (*ofile == NULL)
		{
			printf("Error creating file: %s\n", filename);
		}
		else
		{
			opened++;
		}
	}
	if (opened != 5)
	{
		close_files(ifile, ofile);
	}
	return opened == 5;
}

static void read_line(FILE *ifile, unsigned char *line, unsigned int *lineno)
{
	memset(line, '\0', LENGTH);
	fgets(line, LENGTH, ifile);
	if (line[0] == 12)
	{
		strcpy(line, line + 1);
	}
	(*lineno)++;
}

static void write_line(unsigned char *line, FILE *ofile)
{
	fprintf(ofile, "%s", line);
}

static void write_comment_line(unsigned char *line, FILE *ofile)
{
	fprintf(ofile, "                                   ;\"%s\"\n", line);
}

static void insert_lines_kernel1(FILE *ofile, unsigned int lineno)
{
	if (lineno == 898 || lineno == 934 || lineno == 938 ||
		lineno == 969 || lineno == 980 || lineno == 992 ||
		lineno == 999 || lineno == 1007 || lineno == 1024 ||
		lineno == 1031 || lineno == 1038 || lineno == 1045 ||
		lineno == 1055 || lineno == 1073 || lineno == 1115 ||
		lineno == 1128 || lineno == 1162 || lineno == 1212)
	{
		write_line("                                       ; BEGIN\n", ofile);
	}
}

static void insert_lines_kernel2(FILE *ofile, unsigned int lineno)
{
}

static void insert_lines_kernel3(FILE *ofile, unsigned int lineno)
{
	if (lineno == 3252)
	{
		write_line("                                   ;   BEGIN\n", ofile);
	}
}

static void insert_lines_kernel4(FILE *ofile, unsigned int lineno)
{
	if (lineno == 5313|| lineno == 4868 || lineno == 4813 ||
		lineno == 4773 || lineno == 4744 || lineno == 4734)
	{
		write_line("                                ;    BEGIN\n", ofile);
	}
	else if (lineno == 5314 || lineno == 4869 || lineno == 4814 ||
		lineno == 4774 || lineno == 4745 || lineno == 4735)
	{
		write_line("                                ;    END;\n", ofile);
	}
	else if (lineno == 5607)
	{
		write_line("                                ;        BEGIN\n", ofile);
	}
	else if (lineno == 5608)
	{
		write_line("                                ;        END;\n", ofile);
	}
}

static void adjust_line_kernel1(unsigned char *line)
{
	char *p;

	p = strstr(line, "& REGISTERS");
	if (p != NULL)
	{
		memmove(p + 3, p + 1, strlen(p));
		strncpy(p, "AND", 3);
	}
}

static void adjust_line_kernel2(unsigned char *line)
{
	char *p;

	p = strstr(line, "; MACRO ");
	if (p != NULL)
	{
		memmove(p + 2, p + 8, strlen(p) - 7);
	}
	p = strstr(line, "VAR READY");
	if (p != NULL)
	{
		memmove(p + 10, p + 9, strlen(p) - 8);
		p[9] = ':';
	}
	p = strstr(line, "USER @");
	if (p != NULL)
	{
		memmove(p + 4, p + 5, strlen(p) - 4);
	}
	p = strstr(line, "@ QUEUETYPE");
	if (p != NULL)
	{
		memmove(p + 1, p + 2, strlen(p) - 1);
	}
	p = strstr(line, "@ PROCESS");
	if (p != NULL)
	{
		memmove(p + 1, p + 2, strlen(p) - 1);
	}
	if (line[35] == ':')
	{
		line[35] = ';';
	}
}

static void adjust_line_kernel3(unsigned char *line)
{
	char *p;

	p = strstr(line, "CURSOR@.(.I.)");
	if (p != NULL)
	{
		memmove(p + 7, p + 8, strlen(p) - 7);
	}
	p = strstr(line, "STAT30  =      CDST                ;     STATUSREG");
	if (p != NULL)
	{
		memmove(p + 51, p + 50, strlen(p) - 49);
		p[50] = ':';
	}
}

static void adjust_line_kernel4(unsigned char *line)
{
	char *p;

	p = strstr(line, "ST(S):  S");
	if (p != NULL)
	{
		p[5] = ';';
	}
	p = strstr(line, "NEWINI: MOV     B,X             ;    B:=X");
	if (p != NULL)
	{
		strncpy(p + 37, "X:=B", 4);
	}
	p = strstr(line, "ST(S) S:+2");
	if (p != NULL)
	{
		memmove(p + 6, p + 5, strlen(p) - 5);
		p[5] = ';';
	}
	p = strstr(line, "ST:=ABS");
	if (p != NULL)
	{
		memmove(p + 5, p + 2, strlen(p) - 1);
		strncpy(p + 2, "(S)", 3);
	}
	p = strstr(line, "ELSE S:-2; ST(S):=ST(G+10);");
	if (p != NULL)
	{
		memmove(p + 10, p + 4, strlen(p) - 3);
		strncpy(p + 4, " BEGIN", 6);
	}
	p = strstr(line, "THEN S:-2; ST(S):=ST(CONSTADDR)");
	if (p != NULL)
	{
		memmove(p + 10, p + 4, strlen(p) - 3);
		strncpy(p + 4, " BEGIN", 6);
	}
	p = strstr(line, ";     \"JOB\"");
	if (p != NULL)
	{
		memmove(p + 6, p + 1, strlen(p));
		strncpy(p + 1, " END;", 5);
	}
	p = strstr(line, ") \"SYSTEM\"");
	if (p != NULL)
	{
		memmove(p + 7, p + 1, strlen(p));
		strncpy(p + 1, "; END;", 6);
	}
	p = strstr(line, "<REAL> ST(S)");
	if (p != NULL)
	{
		memmove(p + 6, p + 7, strlen(p) - 6);
	}
	p = strstr(line, " PROGRAM\"");
	if (p != NULL)
	{
		p[0] = '\"';
	}
	p = strstr(line, " LINE 1 OF USER");
	if (p != NULL)
	{
		memmove(p + 16, p + 15, strlen(p) - 14);
		p[0] = p[15] = '\"';
	}
	p = strstr(line, " LINE OF CALL\"");
	if (p != NULL)
	{
		p[0] = '\"';
	}
	p = strstr(line, " WILL REFER TO");
	if (p != NULL)
	{
		memmove(p + 15, p + 14, strlen(p) - 13);
		p[0] = p[14] = '\"';
	}
	p = strstr(line, "\"ERROR MESSAGE");
	if (p != NULL)
	{
		memmove(p + 15, p + 14, strlen(p) - 13);
		p[14] = '\"';
	}
	p = strstr(line, ": SHIFT");
	if (p != NULL)
	{
		memmove(p + 1, p + 2, strlen(p) - 1);
	}
	p = strstr(line, ";     &");
	if (p != NULL)
	{
		memmove(p + 9, p + 7, strlen(p) - 6);
		strncpy(p, ";     AND", 9);
	}
	p = strstr(line, ";PROCEDURE ");
	if (p != NULL)
	{
		memmove(p + 3, p + 1, strlen(p));
		strncpy(p + 1, "  ", 2);
	}
}

static void merge_kernel1(FILE *ifile, FILE *ofile, unsigned char *line, unsigned int *lineno, unsigned char *name)
{
	int extract = 1;

	write_comment_line(name, ofile);
	read_line(ifile, line, lineno);
	while (extract && !feof(ifile))
	{
		extract = line[0] != 25;
		if (extract)
		{
			insert_lines_kernel1(ofile, *lineno);
			adjust_line_kernel1(line);
			write_line(line, ofile);
			read_line(ifile, line, lineno);
		}
	}
}

static void merge_kernel2(FILE *ifile, FILE *ofile, unsigned char *line, unsigned int *lineno, unsigned char *name)
{
	int extract = 1;

	write_comment_line(name, ofile);
	read_line(ifile, line, lineno);
	while (extract && !feof(ifile))
	{
		extract = line[0] != 25;
		if (extract) {
			insert_lines_kernel2(ofile, *lineno);
			adjust_line_kernel2(line);
			write_line(line, ofile);
			read_line(ifile, line, lineno);
		}
	}
}

static void merge_kernel3(FILE *ifile, FILE *ofile, unsigned char *line, unsigned int *lineno, unsigned char *name)
{
	int extract = 1;

	write_comment_line(name, ofile);
	read_line(ifile, line, lineno);
	while (extract && !feof(ifile))
	{
		extract = line[0] != 25;
		if (extract) {
			insert_lines_kernel3(ofile, *lineno);
			adjust_line_kernel3(line);
			write_line(line, ofile);
			read_line(ifile, line, lineno);
		}
	}
}

static void merge_kernel4(FILE *ifile, FILE *ofile, unsigned char *line, unsigned int *lineno, unsigned char *name)
{
	int extract = 1;

	write_comment_line(name, ofile);
	read_line(ifile, line, lineno);
	while (extract && !feof(ifile))
	{
		extract = line[0] != 25;
		if (extract)
		{
			insert_lines_kernel4(ofile, *lineno);
			adjust_line_kernel4(line);
			write_line(line, ofile);
			read_line(ifile, line, lineno);
		}
	}
}

int main(int argc, char *argv[])
{
	FILE *ifile[4] = { NULL, NULL, NULL, NULL };
	FILE *ofile = NULL;

	if ((argc != 3) || (argv[0] == NULL))
	{
		printf("Usage is: KernelMerge45 <kernel path> <merge file>\n");
	}
	else
	{
		unsigned char out_path[PATH_LENGTH];
		unsigned char *names[] = { "KERNELTEXT1", "KERNELTEXT2", "KERNELTEXT3", "KERNELTEXT4" };

		make_paths(argv[1], out_path);
		if (open_files(ifile, &ofile, argv[2], out_path, names))
		{
			unsigned char line[LENGTH];
			unsigned int lineno = 0;

			merge_kernel1(ifile[0], ofile, line, &lineno, names[0]);
			merge_kernel2(ifile[1], ofile, line, &lineno, names[1]);
			merge_kernel3(ifile[2], ofile, line, &lineno, names[2]);
			merge_kernel4(ifile[3], ofile, line, &lineno, names[3]);
			close_files(ifile, &ofile);
		}
	}
	return EXIT_SUCCESS;
}