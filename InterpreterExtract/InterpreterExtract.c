/*
	InterpreterExtract.c
	Copyright (c) 2022. Gerardo Ospina

	This program extracts the interpreter from the kernel sources and converts
	low level assembler like instructions to high level

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

static void adjust_declaration(unsigned char *line, unsigned int lineno, FILE *ofile)
{
	if (lineno == 350)
	{
		strncpy(line + 1, "VAR", 3);
	}
	if (lineno == 2071)
	{
		write_line("     Z:\n", ofile);
		write_line("        INTEGER;\n", ofile);
		write_line("     ZERO: BOOLEAN;\n", ofile);
		write_line("     NEGATIVE: BOOLEAN;\n", ofile);
		write_line("     GREATER: BOOLEAN;\n", ofile);
		write_line("     LESS: BOOLEAN;\n", ofile);
		write_line("     EQUAL: BOOLEAN;\n", ofile);
		write_line("     BITZERO: BOOLEAN;\n", ofile);
	}
}

static void adjust_operator(unsigned char *line, unsigned int lineno)
{
	unsigned char *p;

	p = strstr(line, "Q:+");
	if (p != NULL)
	{
		memmove(p + 4, p + 2, strlen(p) - 1);
		strncpy(p + 2, "=Q", 2);
	}
	p = strstr(line, "ST(S):+");
	if (p != NULL)
	{
		memmove(p + 12, p + 6, strlen(p) - 1);
		strncpy(p + 6, "=ST(S)", 6);
	}
	p = strstr(line, "W:+");
	if (p != NULL)
	{
		memmove(p + 4, p + 2, strlen(p) - 1);
		strncpy(p + 2, "=W", 2);
	}
	p = strstr(line, "S:+");
	if (p != NULL)
	{
		memmove(p + 4, p + 2, strlen(p) - 1);
		strncpy(p + 2, "=S", 2);
	}
	p = strstr(line, "X:+");
	if (p != NULL)
	{
		memmove(p + 4, p + 2, strlen(p) - 1);
		strncpy(p + 2, "=X", 2);
	}
	p = strstr(line, "ST(HEAPTOP):+");
	if (p != NULL)
	{
		memmove(p + 24, p + 12, strlen(p) - 10);
		strncpy(p + 12, "=ST(HEAPTOP)", 12);
	}
	p = strstr(line, "ST(S+2):+");
	if (p != NULL)
	{
		memmove(p + 16, p + 8, strlen(p) - 7);
		strncpy(p + 8, "=ST(S+2)", 8);
	}
	p = strstr(line, "Y:+");
	if (p != NULL)
	{
		memmove(p + 4, p + 2, strlen(p) - 1);
		strncpy(p + 2, "=Y", 2);
	}
	p = strstr(line, "ST(KERNELARG4):+");
	if (p != NULL)
	{
		memmove(p + 30, p + 15, strlen(p) - 14);
		strncpy(p + 15, "=ST(KERNELARG4)", 15);
	}
	p = strstr(line, "S:-");
	if (p != NULL)
	{
		memmove(p + 4, p + 2, strlen(p) - 1);
		strncpy(p + 2, "=S", 2);
	}
	p = strstr(line, "W:-");
	if (p != NULL)
	{
		memmove(p + 4, p + 2, strlen(p) - 1);
		strncpy(p + 2, "=W", 2);
	}
	p = strstr(line, "X:-");
	if (p != NULL)
	{
		memmove(p + 4, p + 2, strlen(p) - 1);
		strncpy(p + 2, "=X", 2);
		if (lineno == 2150)
		{
			memmove(p + 15, p, strlen(p));
			strncpy(p, "LESS:=X<ST(Q); ", 15);
		}
	}
	p = strstr(line, "ST(S+2):-");
	if (p != NULL)
	{
		memmove(p + 16, p + 8, strlen(p) - 7);
		strncpy(p + 8, "=ST(S+2)", 8);
	}
	p = strstr(line, "Y:-");
	if (p != NULL)
	{
		memmove(p + 4, p + 2, strlen(p) - 1);
		strncpy(p + 2, "=Y", 2);
	}
	p = strstr(line, "W:*");
	if (p != NULL)
	{
		memmove(p + 4, p + 2, strlen(p) - 1);
		strncpy(p + 2, "=W", 2);
	}
	p = strstr(line, "X:*");
	if (p != NULL)
	{
		memmove(p + 4, p + 2, strlen(p) - 1);
		strncpy(p + 2, "=X", 2);
	}
	if (lineno == 2345 || lineno == 2357)
	{
		printf("%s", line);
		line[4] = 'W';
	}
	p = strstr(line, "WX:/");
	if (p != NULL)
	{
		if (lineno == 2346)
		{
			memmove(p + 9, p + 4, strlen(p));
			strncpy(p + 1, ":=W DIV ", 8);
		}
		else
		{
			memmove(p + 9, p + 4, strlen(p));
			strncpy(p + 1, ":=W MOD ", 8);
		}
	}
	p = strstr(line, "W:/");
	if (p != NULL)
	{
		memmove(p + 4, p + 2, strlen(p) - 1);
		strncpy(p + 2, "=W", 2);
	}
	p = strstr(line, "Y:MOD");
	if (p != NULL)
	{
		memmove(p + 5, p + 2, strlen(p) - 1);
		strncpy(p + 2, "=Y ", 3);
	}
	p = strstr(line, "W:MOD");
	if (p != NULL)
	{
		memmove(p + 5, p + 2, strlen(p) - 1);
		strncpy(p + 2, "=W ", 3);
	}
	p = strstr(line, "X:DIV");
	if (p != NULL)
	{
		memmove(p + 5, p + 2, strlen(p) - 1);
		strncpy(p + 2, "=X ", 3);
	}
	p = strstr(line, "ST(X):ANDNOT");
	if (p != NULL)
	{
		memmove(p + 10, p + 9, strlen(p) - 8);
		p[9] = ' ';
		memmove(p + 13, p + 6, strlen(p) - 5);
		strncpy(p + 6, "=ST(X) ", 7);
	}
	p = strstr(line, "ST(S):ANDNOT");
	if (p != NULL)
	{
		memmove(p + 10, p + 9, strlen(p) - 8);
		p[9] = ' ';
		memmove(p + 13, p + 6, strlen(p) - 5);
		strncpy(p + 6, "=ST(S) ", 7);
	}
	p = strstr(line, "ST(S+16):ANDNOT");
	if (p != NULL)
	{
		memmove(p + 13, p + 12, strlen(p) - 11);
		p[12] = ' ';
		memmove(p + 19, p + 9, strlen(p) - 8);
		strncpy(p + 9, "=ST(S+16) ", 10);
	}
	p = strstr(line, "ST(X+16):ANDNOT");
	if (p != NULL)
	{
		memmove(p + 13, p + 12, strlen(p) - 11);
		p[12] = ' ';
		memmove(p + 19, p + 9, strlen(p) - 8);
		strncpy(p + 9, "=ST(X+16) ", 10);
	}
	p = strstr(line, "ST(X):OR");
	if (p != NULL)
	{
		memmove(p + 13, p + 6, strlen(p) - 5);
		strncpy(p + 6, "=ST(X) ", 7);
	}
	p = strstr(line, "ST(S+2):OR");
	if (p != NULL)
	{
		memmove(p + 17, p + 8, strlen(p) - 7);
		strncpy(p + 8, "=ST(S+2) ", 9);
	}
	p = strstr(line, "ST(S+16):OR");
	if (p != NULL)
	{
		memmove(p + 19, p + 9, strlen(p) - 8);
		strncpy(p + 9, "=ST(S+16) ", 10);
	}
	p = strstr(line, "Y:SHIFT");
	if (p != NULL)
	{
		memmove(p + 5, p + 2, strlen(p) - 1);
		strncpy(p + 2, "=Y ", 3);
	}
	p = strstr(line, "W:SHIFT");
	if (p != NULL)
	{
		memmove(p + 5, p + 2, strlen(p) - 1);
		strncpy(p + 2, "=W ", 3);
	}
}

static void adjust_instruction(unsigned char *line, unsigned int lineno)
{
	unsigned char *p;

	p = strstr(line, "CLEAR ST(X)");
	if (p != NULL)
	{
		memmove(p + 14, p + 11, strlen(p));
		strncpy(p + 11, ":=0", 3);
		memmove(p, p + 6, strlen(p) - 5);
	}
	p = strstr(line, "CLEAR W");	                 
	if (p != NULL)
	{
		memmove(p + 10, p + 7, strlen(p));
		strncpy(p + 7, ":=0", 3);
		memmove(p, p + 6, strlen(p) - 5);
	}
	p = strstr(line, "CLEAR ST(S)");
	if (p != NULL)
	{
		memmove(p + 14, p + 11, strlen(p));
		strncpy(p + 11, ":=0", 3);
		memmove(p, p + 6, strlen(p) - 5);
	}
	p = strstr(line, "CLEAR ST(JOB)");
	if (p != NULL)
	{
		memmove(p + 16, p + 13, strlen(p));
		strncpy(p + 13, ":=0", 3);
		memmove(p, p + 6, strlen(p) - 5);
	}
	p = strstr(line, "INCREMENT ST(S)");	                 
	if (p != NULL)
	{
		memmove(p + 24, p + 15, strlen(p));
		strncpy(p + 15, ":=ST(S)+1", 9);
		memmove(p, p + 10, strlen(p) - 9);
	}
	p = strstr(line, "INCREMENT W");	                 
	if (p != NULL)
	{
		memmove(p + 16, p + 11, strlen(p));
		strncpy(p + 11, ":=W+1", 5);
		memmove(p, p + 10, strlen(p) - 9);
	}
	p = strstr(line, "INCREMENT ST(JOB)");	                 
	if (p != NULL)
	{
		memmove(p + 28, p + 17, strlen(p));
		strncpy(p + 17, ":=ST(JOB)+1", 11);
		memmove(p, p + 10, strlen(p) - 9);
	}
	p = strstr(line, "INCREMENT ST(ST(S))");	                 
	if (p != NULL)
	{
		memmove(p + 32, p + 19, strlen(p));
		strncpy(p + 19, ":=ST(ST(S))+1", 13);
		memmove(p, p + 10, strlen(p) - 9);
	}
	p = strstr(line, "DECREMENT ST(ST(S))");	                 
	if (p != NULL)
	{
		memmove(p + 32, p + 19, strlen(p));
		strncpy(p + 19, ":=ST(ST(S))-1", 13);
		memmove(p, p + 10, strlen(p) - 9);
	}
	p = strstr(line, "DECREMENT ST(S)");	                 
	if (p != NULL)
	{
		memmove(p + 24, p + 15, strlen(p));
		strncpy(p + 15, ":=ST(S)-1", 9);
		memmove(p, p + 10, strlen(p) - 9);
	}
	p = strstr(line, "HALVE W");	                 
	if (p != NULL)
	{
		memmove(p + 17, p + 7, strlen(p));
		strncpy(p + 7, ":=W DIV 2;", 10);
		memmove(p, p + 6, strlen(p) - 5);
	}
	p = strstr(line, "DOUBLE W");             	                 
	if (p != NULL)
	{
		memmove(p + 13, p + 8, strlen(p));
		strncpy(p + 8, ":=W*2", 5);
		memmove(p, p + 7, strlen(p) - 6);
	}
	p = strstr(line, "ITERATE W TIMES");
	if (p != NULL)
	{
		memmove(p + 1, p, strlen(p) );
		strncpy(p, "FOR Z:=1 TO W DO", 16);
	}
}

static void adjust_condition(unsigned char *line, unsigned int lineno)
{
	unsigned char *p;

	p = strstr(line, "TEST ST(JOB)");
	if (p != NULL)
	{
		memmove(p + 14, p + 12, strlen(p) - 11);
		strncpy(p + 12, "=0", 2);
		memmove(p + 6, p + 5, strlen(p) - 4);
		strncpy(p, "ZERO:=", 6);
	}
	p = strstr(line, "TEST ST(S)");
	if (p != NULL)
	{
		if (lineno == 2161 || lineno == 2665 || lineno == 3007)
		{
			memmove(p + 12, p + 10, strlen(p) - 9);
			strncpy(p + 10, "=0", 2);
			memmove(p + 6, p + 5, strlen(p) - 4);
			strncpy(p, "ZERO:=", 6);
		}
		else if (lineno == 2981)
		{
			memmove(p + 12, p + 10, strlen(p) - 9);
			strncpy(p + 10, "<0", 2);
			memmove(p + 10, p + 5, strlen(p) - 4);
			strncpy(p, "NEGATIVE:=", 10);
		}
		else
		{
			strncpy(p, "           ", 11);
		}
	}
	p = strstr(line, "TEST ST(X)");
	if (p != NULL)
	{
		strncpy(p, "           ", 11);
	}
	p = strstr(line, "TEST ST(CONTINUE)");
	if (p != NULL)
	{
		memmove(p + 19, p + 17, strlen(p) - 11);
		strncpy(p + 17, "=0", 2);
		memmove(p + 6, p + 5, strlen(p) - 4);
		strncpy(p, "ZERO:=", 6);
	}
	p = strstr(line, "TEST ST(Q)");
	if (p != NULL)
	{
		strncpy(p, "           ", 11);
	}
	p = strstr(line, "X COMPARE ST(Q)");
	if (p != NULL)
	{
		memmove(p + 2, p + 10, strlen(p) - 9);
		if (lineno == 2153)
		{
			p[1] = '>';
			memmove(p + 9, p, strlen(p));
			strncpy(p, "GREATER:=", 9);
		}
		else
		{
			p[1] = '<';
			memmove(p + 6, p, strlen(p) + 1);
			strncpy(p, "LESS:=", 6);
		}
	}
	p = strstr(line, "ST(S) COMPARE ST(Q)");
	if (p != NULL)
	{
		memmove(p + 6, p + 14, strlen(p) - 9);
		if (lineno == 2177)
		{
			p[5] = '<';
			memmove(p + 6, p, strlen(p) + 1);
			strncpy(p, "LESS:=", 6);
		}
		else
		{
			p[5] = '>';
			memmove(p + 9, p, strlen(p));
			strncpy(p, "GREATER:=", 9);
		}
	}
	p = strstr(line, "W COMPARE ");
	if (p != NULL)
	{
		memmove(p + 2, p + 10, strlen(p) - 9);
		p[1] = '>';
		memmove(p + 9, p, strlen(p));
		strncpy(p, "GREATER:=", 9);
	}
	p = strstr(line, "ST(S) COMPARE ST(S+2)");
	if (p != NULL)
	{
		memmove(p + 6, p + 14, strlen(p) - 13);
		if (lineno == 2397 || lineno == 2421)
		{
			p[5] = '>';
			memmove(p + 9, p, strlen(p));
			strncpy(p, "GREATER:=", 9);
		}
		else if (lineno == 2405 || lineno == 2429)
		{
			p[5] = '=';
			memmove(p + 7, p, strlen(p) + 1);
			strncpy(p, "EQUAL:=", 7);
		}
		else 
		{
			p[5] = '<';
			memmove(p + 6, p, strlen(p) + 1);
			strncpy(p, "LESS:=", 6);
		}
	}
	p = strstr(line, "ST(S) COMPARE X");
	if (p != NULL)
	{
		memmove(p + 6, p + 14, strlen(p) - 13);
		if (lineno == 2446 || lineno == 2473)
		{
			p[5] = '<';
			memmove(p + 6, p, strlen(p) + 1);
			strncpy(p, "LESS:=", 6);
		}
		else if (lineno == 2455 || lineno == 2482)
		{
			p[5] = '=';
			memmove(p + 7, p, strlen(p) + 1);
			strncpy(p, "EQUAL:=", 7);
		}
		else 
		{
			p[5] = '>';
			memmove(p + 9, p, strlen(p));
			strncpy(p, "GREATER:=", 9);
		}
	}
	p = strstr(line, "ST(X+16) COMPARE ST(X)");
	if (p != NULL)
	{
		memmove(p + 9, p + 17, strlen(p) - 16);
		p[8] = '=';
		memmove(p + 7, p, strlen(p) + 1);
		strncpy(p, "EQUAL:=", 7);
	}
	p = strstr(line, "ST(Y) COMPARE<BYTE> ST(X)");
	if (p != NULL)
	{
		memmove(p + 6, p + 13, strlen(p) - 12);
		p[5] = '=';
		memmove(p + 7, p, strlen(p) + 1);
		strncpy(p, "EQUAL:=", 7);
		if (lineno == 2556 || lineno == 2596)
		{
			memmove(p + 26, p, strlen(p) + 1);
			strncpy(p, "LESS:=ST(Y)<\"BYTE\" ST(X); ", 26);
		}
		else
		{
			memmove(p + 28, p, strlen(p) + 1);
			strncpy(p, "GREATER:=ST(Y)>\"BYTE\" ST(X); ", 28);
		}
	}
	p = strstr(line, "ST(Y) COMPARE ST(X)");
	if (p != NULL)
	{
		memmove(p + 6, p + 14, strlen(p) - 13);
		p[5] = '=';
		memmove(p + 7, p, strlen(p) + 1);
		strncpy(p, "EQUAL:=", 7);
	}
	p = strstr(line, "ST(Q) TESTBIT W");
	if (p != NULL)
	{
		memmove(p + 9, p, strlen(p) + 1);
		strncpy(p, "BITZERO:=", 9);
	}
	if (lineno == 2925)
	{
		memmove(line + 23, line + 12, strlen(line) + 12);
		strncpy(line + 12, "; ZERO:=W=0", 11);
	}
	p = strstr(line, "NONZERO");
	if (p != NULL)
	{
		memmove(p + 4, p + 3, strlen(p) - 2);
		p[2] = 'T';
		p[3] = ' ';
	}
	p = strstr(line, "NOTZERO");
	if (p != NULL)
	{
		memmove(p + 4, p + 3, strlen(p) - 2);
		p[3] = ' ';
	}
	p = strstr(line, "NOTGREATER");
	if (p != NULL)
	{
		memmove(p + 4, p + 3, strlen(p) - 2);
		p[3] = ' ';
	}
	p = strstr(line, "NOTEQUAL");
	if (p != NULL)
	{
		memmove(p + 4, p + 3, strlen(p) - 2);
		p[3] = ' ';
	}
	p = strstr(line, "NOTLESS");
	if (p != NULL)
	{
		memmove(p + 4, p + 3, strlen(p) - 2);
		p[3] = ' ';
	}
}

static void adjust_cast(unsigned char *line, unsigned int lineno)
{
	unsigned char *p;

	p = strstr(line, "<BYTE>");
	if (p != NULL)
	{
		p[0] = '"';
		p[5] = '"';
	}
	p = strstr(line, "<UNSIGNED>");
	if (p != NULL)
	{
		p[0] = '"';
		p[9] = '"';
	}
	p = strstr(line, "<REAL>");
	if (p != NULL)
	{
		p[0] = '"';
		p[5] = '"';
	}
}

static void adjust_line(unsigned char *line, unsigned int lineno, FILE *ofile)
{
	adjust_declaration(line, lineno, ofile);
	adjust_operator(line, lineno);
	adjust_instruction(line, lineno);
	adjust_condition(line, lineno);
	adjust_cast(line, lineno);
}

static void extract_interpreter(FILE *ifile, FILE *ofile, unsigned char *line, unsigned int *lineno)
{
	read_line(ifile, line, lineno);
	while (!feof(ifile))
	{
		if ((*lineno >= 350 && *lineno <= 365) ||
			*lineno >= 2071)
		{
			adjust_line(line, *lineno, ofile);
			write_line(line, ofile);
		}
		read_line(ifile, line, lineno);
	}
}

int main(int argc, char *argv[])
{
	if ((argc != 3) || (argv[0] == NULL))
	{
		printf("Usage is: Convert <Interpreter file> <File name>\n");
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

				make_out_path(argv[1], argv[2], out_path);
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