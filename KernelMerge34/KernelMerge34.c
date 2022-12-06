/*
	KernelMerge34.c
	Copyright (c) 2016, 2022. Gerardo Ospina

	This program merges in a single file the PDP-11/34 kernel source files

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

static void make_paths(unsigned char * path, unsigned char *out_path)
{
	unsigned int len = strlen(path);

	strcpy(out_path, path);
	if (len != 0 && path[len - 1] != SEPARATOR)
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

static int open_files(FILE *ifile[4], FILE **ofile, unsigned char *name, unsigned char *out_path, unsigned char *names[4])
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

static void read_line(FILE *ifile, 	unsigned char *line, unsigned int *lineno)
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
	fprintf(ofile, "%s", line);
}

static void write_comment_line(unsigned char *line, FILE *ofile)
{
	fprintf(ofile, "                                   ;\"%s\"\n", line);
}

static void insert_lines_kernel1(FILE *ofile, unsigned int lineno)
{
	if (lineno == 134 || lineno == 169 || lineno == 220 ||
		lineno == 257 || lineno == 309 || lineno == 366 ||
		lineno == 410 || lineno == 475 || lineno == 561 ||
		lineno == 587 || lineno == 605 || lineno == 635 ||
		lineno == 692 || lineno == 763 || lineno == 782 ||
		lineno == 839 || lineno == 933 || lineno == 992 ||
		lineno == 1242 || lineno == 1252 || lineno == 1294 ||
		lineno == 1331 || lineno == 1507 || lineno == 1813)
	{
		write_line("        .PAGE\n", ofile);
	}
	else if (lineno == 998 || lineno == 1034 || lineno == 1038 ||
		lineno == 1069 || lineno == 1080 || lineno == 1092 ||
		lineno == 1099 || lineno == 1107 || lineno == 1124 ||
		lineno == 1131 || lineno == 1138 || lineno == 1145 ||
		lineno == 1155 || lineno == 1173 || lineno == 1215 ||
		lineno == 1228 || lineno == 1258 || lineno == 1306)
	{
		write_line("                                       ; BEGIN\n", ofile);
	}
}

static void insert_lines_kernel2(FILE *ofile, unsigned int lineno)
{
	if (lineno == 1964)
	{
		write_line("                                   ;\n", ofile);
		write_line("                                   ;\n", ofile);
	}
	if (lineno == 1852 || lineno == 1964 || lineno == 2015 ||
		lineno == 2088 || lineno == 2134 || lineno == 2171 ||
		lineno == 2212 || lineno == 2269 || lineno == 2288 ||
		lineno == 2336 || lineno == 2487 || lineno == 2927 ||
		lineno == 2990 || lineno == 3108)
	{
		write_line("        .PAGE\n", ofile);
	}
	else if (lineno == 2872)
	{
		write_line("                                   ;   END;\n", ofile);
	}
}

static void insert_lines_kernel3(FILE *ofile, unsigned int lineno)
{
	if (lineno == 3220 || lineno == 3293 || lineno == 3323 ||
		lineno == 3578 || lineno == 3817 || lineno == 4029 ||
		lineno == 4143 || lineno == 4384 || lineno == 4489 ||
		lineno == 4587)
	{
		write_line("        .PAGE\n", ofile);
	}
	if (lineno == 3479)
	{
		write_line("                                   ;   BEGIN\n", ofile);
	}
}

static void insert_lines_kernel4(FILE *ofile, unsigned int lineno)
{
	if (lineno == 4627 || lineno == 4683 || lineno == 4768)
	{
		write_line("        .PAGE\n", ofile);
	}
	else if (lineno == 5580 || lineno == 5103 || lineno == 5038 || lineno == 4998 || lineno == 4969 ||
		lineno == 4959)
	{
		write_line("                                ;    BEGIN\n", ofile);
	}
	else if (lineno == 5581 || lineno == 5104 || lineno == 5039 || lineno == 4999 || lineno == 4970 ||
		lineno == 4960)
	{
		write_line("                                ;    END;\n", ofile);
	}
	else if (lineno == 5872)
	{
		write_line("                                ;        BEGIN\n", ofile);
	}
	else if (lineno == 5873)
	{
		write_line("                                ;        END;\n", ofile);
	}
	else if (lineno == 5948)
	{
		write_line("                                ;    ST(S):=ABS<REAL>(ST(S));\n", ofile);
	}
	else if (lineno == 5600 || lineno == 5990)
	{
		write_line("                                ;\n", ofile);
		write_line("                                ;\n", ofile);
	}
}

static void adjust_line_kernel1(unsigned char *line)
{
	unsigned char *p;

	p = strstr(line, "KNLPSW  =      PSCMDK+PSPRT7	   ;   K");
	if (p != NULL)
	{
		memmove(p + 40, p + 36, strlen(p) - 35);
		strncpy(p + 33, "<01>   ", 7);
	}
	p = strstr(line, "& REGISTERS");
	if (p != NULL)
	{
		memmove(p + 3, p + 1, strlen(p));
		strncpy(p, "AND", 3);
	}
	p = strstr(line, "INIT 40");
	if (p != NULL)
	{
		strncpy(p + 5, "24", 2);
	}
	p = strstr(line, "READ(DISK0, 0, 1");
	if (p != NULL)
	{
		p[15] = '0';
	}
}

static void adjust_line_kernel2(unsigned char *line)
{
	unsigned char *p;

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
	unsigned char *p;

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
	unsigned char *p;

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

static void merge_kernel1(FILE *ifile, FILE *ofile, unsigned char *line, unsigned char *name, unsigned int *lineno)
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

static void merge_kernel2(FILE *ifile, FILE *ofile, unsigned char *line, unsigned char *name, unsigned int *lineno)
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

static void merge_kernel3(FILE *ifile, FILE *ofile, unsigned char *line, unsigned char *name, unsigned int *lineno)
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

static void merge_kernel4(FILE *ifile, FILE *ofile, unsigned char *line, unsigned char *name, unsigned int *lineno)
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
	if ((argc != 3) || (argv[0] == NULL))
	{
		printf("Usage is: KernelMerge34 <kernel path> <merge file>\n");
	}
	else
	{
		unsigned char out_path[PATH_LENGTH];
		FILE *ifile[4] = { NULL, NULL, NULL, NULL };
		FILE *ofile =  NULL;
		unsigned char *names[] = { "KERNELTEXT1", "KERNELTEXT2", "KERNELTEXT3", "KERNELTEXT4" };

		make_paths(argv[1], out_path);
		if (open_files(ifile, &ofile, argv[2], out_path, names))
		{
			unsigned char line[LENGTH];
			unsigned int lineno = 0;

			merge_kernel1(ifile[0], ofile, line, names[0], &lineno);
			merge_kernel2(ifile[1], ofile, line, names[1], &lineno);
			merge_kernel3(ifile[2], ofile, line, names[2], &lineno);
			merge_kernel4(ifile[3], ofile, line, names[3], &lineno);
			close_files(ifile, &ofile);
		}
	}
	return EXIT_SUCCESS;
}