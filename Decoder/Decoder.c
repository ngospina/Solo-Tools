/* 
   Decoder.c   
   Copyright (c) 2009, Gerardo Ospina

   Concurrent Pascal Virtual Machine Decoder

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

#define CONSTADDR    2
#define LOCALADDR    4
#define GLOBALADDR   6
#define PUSHCONST    8
#define PUSHLOCAL   10
#define PUSHGLOBAL  12
#define PUSHIND     14
#define PUSHBYTE    16
#define PUSHREAL    18
#define PUSHSET     20
#define FIELD       22
#define INDEX       24
#define POINTER     26
#define VARIANT     28
#define RANGE       30
#define COPYBYTE    32
#define COPYWORD    34
#define COPYREAL    36
#define COPYSET     38
#define COPYTAG     40
#define COPYSTRUCT  42
#define NEW         44
#define NEWINIT     46
#define NOT         48
#define ANDWORD     50
#define ANDSET      52
#define ORWORD      54
#define ORSET       56
#define NEGWORD     58
#define NEGREAL     60
#define ADDWORD     62
#define ADDREAL     64
#define SUBWORD     66
#define SUBREAL     68
#define SUBSET      70
#define MULWORD     72
#define MULREAL     74
#define DIVWORD     76
#define DIVREAL     78
#define MODWORD     80
#define BUILDSET    82
#define INSET       84
#define LSWORD      86
#define EQWORD      88
#define GRWORD      90
#define NLWORD      92
#define NEWORD      94
#define NGWORD      96
#define LSREAL      98
#define EQREAL     100 
#define GRREAL     102
#define NLREAL     104
#define NEREAL     106
#define NGREAL     108
#define EQSET      110
#define NLSET      112
#define NESET      114
#define NGSET      116
#define LSSTRUCT   118
#define EQSTRUCT   120
#define GRSTRUCT   122
#define NLSTRUCT   124
#define NESTRUCT   126
#define NGSTRUCT   128
#define FUNCVALUE  130
#define JUMP       132
#define FALSEJUMP  134
#define CASEJUMP   136
#define INITVAR    138
#define CALL       140
#define CALLSYS    142
#define ENTER      144
#define EXIT       146
#define ENTERPROG  148
#define EXITPROG   150
#define BEGINCLASS 152
#define ENDCLASS   154
#define ENTERCLASS 156
#define EXITCLASS  158
#define BEGINMON   160
#define ENDMON     162
#define ENTERMON   164
#define EXITMON    166
#define BEGINPROC  168
#define ENDPROC    170
#define ENTERPROC  172
#define EXITPROC   174
#define POP        176
#define NEWLINE    178
#define INCRWORD   180
#define DECRWORD   182
#define INITCLASS  184
#define INITMON    186
#define INITPROC   188
#define PUSHLABEL  190
#define CALLPROG   192
#define TRUNCREAL  194
#define ABSWORD    196
#define ABSREAL    198
#define SUCCWORD   200
#define PREDWORD   202
#define CONVWORD   204
#define EMPTY      206
#define ATTRIBUTE  208
#define REALTIME   210
#define DELAY      212
#define CONTINUE   214
#define IO         216
#define START      218
#define STOP       220
#define SETHEAP    222
#define WAIT       224

typedef struct
{
	unsigned short int ProgLen;
	unsigned short int CodeLen;
	unsigned short int StackLen;
	unsigned short int VarLen;
} THeader;

static THeader header;
static char *TABL99[] =
{
/*   0 */ "","",
/*   2 */ "CONSTADDR ","",
/*   4 */ "LOCALADDR ","",
/*   6 */ "GLOBALADDR","",
/*   8 */ "PUSHCONST ","",
/*  10 */ "PUSHLOCAL ","",
/*  12 */ "PUSHGLOBAL","",
/*  14 */ "PUSHIND   ","",
/*  14 */ "PUSHBYTE  ","",
/*  18 */ "PUSHREAL  ","",
/*  20 */ "PUSHSET   ","",
/*  22 */ "FIELD     ","",
/*  24 */ "INDEX     ","",
/*  26 */ "POINTER   ","",
/*  28 */ "VARIANT   ","",
/*  30 */ "RANGE     ","",
/*  32 */ "COPYBYTE  ","",
/*  34 */ "COPYWORD  ","",
/*  36 */ "COPYREAL  ","",
/*  38 */ "COPYSET   ","",
/*  40 */ "COPYTAG   ","",
/*  42 */ "COPYSTRUCT","",
/*  44 */ "NEW       ","",
/*  46 */ "NEWINIT   ","",
/*  48 */ "NOT       ","",
/*  50 */ "ANDWORD   ","",
/*  52 */ "ANDSET    ","",
/*  54 */ "ORWORD    ","",
/*  56 */ "ORSET     ","",
/*  58 */ "NEGWORD   ","",
/*  60 */ "NEGREAL   ","",
/*  62 */ "ADDWORD   ","",
/*  64 */ "ADDREAL   ","",
/*  66 */ "SUBWORD   ","",
/*  68 */ "SUBREAL   ","",
/*  70 */ "SUBSET    ","",
/*  72 */ "MULWORD   ","",
/*  74 */ "MULREAL   ","",
/*  76 */ "DIVWORD   ","",
/*  78 */ "DIVREAL   ","",
/*  80 */ "MODWORD   ","",
/*  82 */ "BUILDSET  ","",
/*  84 */ "INSET     ","",
/*  86 */ "LSWORD    ","",
/*  88 */ "EQWORD    ","",
/*  90 */ "GRWORD    ","",
/*  92 */ "NLWORD    ","",
/*  94 */ "NEWORD    ","",
/*  96 */ "NGWORD    ","",
/*  98 */ "LSREAL    ","",
/* 100 */ "EQREAL    ","",
/* 102 */ "GRREAL    ","",
/* 104 */ "NLREAL    ","",
/* 106 */ "NEREAL    ","",
/* 108 */ "NGREAL    ","",
/* 110 */ "EQSET     ","",
/* 112 */ "NLSET     ","",
/* 114 */ "NESET     ","",
/* 116 */ "NGSET     ","",
/* 118 */ "LSSTRUCT  ","",
/* 120 */ "EQSTRUCT  ","",
/* 122 */ "GRSTRUCT  ","",
/* 124 */ "NLSTRUCT  ","",
/* 126 */ "NESTRUCT  ","",
/* 128 */ "NGSTRUCT  ","",
/* 130 */ "FUNCVALUE ","",
/* 132 */ "JUMP      ","",
/* 134 */ "FALSEJUMP ","",
/* 136 */ "CASEJUMP  ","",
/* 138 */ "INITVAR   ","",
/* 140 */ "CALL      ","",
/* 142 */ "CALLSYS   ","",
/* 144 */ "ENTER     ","",
/* 146 */ "EXIT      ","",
/* 148 */ "ENTERPROG ","",
/* 150 */ "EXITPROG  ","",
/* 152 */ "BEGINCLASS","",
/* 154 */ "ENDCLASS  ","",
/* 156 */ "ENTERCLASS","",
/* 158 */ "EXITCLASS ","",
/* 160 */ "BEGINMON  ","",
/* 162 */ "ENDMON    ","",
/* 164 */ "ENTERMON  ","",
/* 166 */ "EXITMON   ","",
/* 168 */ "BEGINPROC ","",
/* 170 */ "ENDPROC   ","",
/* 172 */ "ENTERPROC ","",
/* 174 */ "EXITPROC  ","",
/* 176 */ "POP       ","",
/* 178 */ "NEWLINE   ","",
/* 180 */ "INCRWORD  ","",
/* 182 */ "DECRWORD  ","",
/* 184 */ "INITCLASS ","",
/* 186 */ "INITMON   ","",
/* 188 */ "INITPROC  ","",
/* 190 */ "PUSHLABEL ","",
/* 192 */ "CALLPROG  ","",
/* 194 */ "TRUNCREAL ","",
/* 196 */ "ABSWORD   ","",
/* 198 */ "ABSREAL   ","",
/* 200 */ "SUCCWORD  ","",
/* 202 */ "PREDWORD  ","",
/* 204 */ "CONVWORD  ","",
/* 206 */ "EMPTY     ","",
/* 208 */ "ATTRIBUTE ","",
/* 210 */ "REALTIME  ","",
/* 212 */ "DELAY     ","",
/* 214 */ "CONTINUE  ","",
/* 216 */ "IO        ","",
/* 218 */ "START     ","",
/* 220 */ "STOP      ","",
/* 222 */ "SETHEAP   ","",
/* 224 */ "WAIT      "
};

static FILE *ifile;
static unsigned short int pos, op, op1, op2, op3, op4;

void Trailer(void)
{
	int n;

	printf("\nTrailer:\n");
	pos += sizeof(THeader);
	n = fread(&op, sizeof(unsigned short int), 1, ifile);
	while (n == 1)
	{
		printf("  %05d: [%5d]\n", pos, op);
		pos += sizeof(unsigned short int);
		n = fread(&op, sizeof(unsigned short int), 1, ifile);
	}
}

void Constants(void)
{
	int n;
	unsigned short int low,high;

	printf("\nConstants:\n");
	while (pos < header.ProgLen)
	{
		n = fread(&op, sizeof(unsigned short int), 1, ifile);
		high = (op & 0xFF00) >> 8;
		low = op & 0xFF;
		if (low >= ' ' && low <= '~')
		{
			printf("  %05d: [%05d:%3d] %c\n", sizeof(THeader) + pos, pos, low, low);
		}
		else
		{
			printf("  %05d: [%05d:%3d]\n", sizeof(THeader) + pos, pos, low);
		}
		if (high >= ' ' && high <= '~')
		{
			printf("  %05d: [%05d:%3d] %c\n", sizeof(THeader) + pos + 1, pos + 1, high, high);
		}
		else
		{
			printf("  %05d: [%05d:%3d]\n", sizeof(THeader) + pos + 1, pos + 1, high);
		}
		pos += sizeof(unsigned short int);
	}
}

void Program(void)
{
	int i, n;

	printf("\nCode:\n"),
	pos = 0;
	while (pos < header.CodeLen)
	{
		n = fread(&op, sizeof(unsigned short int), 1, ifile);
		printf("  %05d: [%05d:%3d]", sizeof(THeader) + pos, pos, op);
		if (op > 0 && op < 225)
		{
			printf(" %s", TABL99[op]);
		}
		pos += sizeof(unsigned short int);
		switch (op)
		{

			/* Required by SOLO */

			case JUMP:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				pos += sizeof(unsigned short int);
				printf(" %d\n", op1);
				break;
			case ENTERCLASS:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				n = fread(&op2, sizeof(unsigned short int), 1, ifile);
				n = fread(&op3, sizeof(unsigned short int), 1, ifile);
				n = fread(&op4, sizeof(unsigned short int), 1, ifile);
				pos += 4 * sizeof(unsigned short int);
				printf(" %d %d %d %d\n", op1, op2, op3, op4);
				break;
			case LOCALADDR:
			case PUSHGLOBAL:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				pos += sizeof(unsigned short int);
				printf(" %d\n", op1);
				break;
			case COPYWORD:
				printf("\n");
				break;
			case GLOBALADDR:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				pos += sizeof(unsigned short int);
				printf(" %d\n", op1);
				break;			
			case MODWORD:
				printf("\n");
				break;
			case PUSHCONST:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				pos += sizeof(unsigned short int);
				printf(" %d\n", op1);
				break;			
			case ADDWORD:
			case EXITCLASS:
			case SUBWORD:
			case EQWORD:
				printf("\n");
				break;
			case BEGINCLASS:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				n = fread(&op2, sizeof(unsigned short int), 1, ifile);
				n = fread(&op3, sizeof(unsigned short int), 1, ifile);
				n = fread(&op4, sizeof(unsigned short int), 1, ifile);
				pos += 4 * sizeof(unsigned short int);
				printf(" %d %d %d %d\n", op1, op2, op3, op4);
				break;
			case ENDCLASS:
			case ANDSET:
				printf("\n");
				break;
			case ENTERMON:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				n = fread(&op2, sizeof(unsigned short int), 1, ifile);
				n = fread(&op3, sizeof(unsigned short int), 1, ifile);
				n = fread(&op4, sizeof(unsigned short int), 1, ifile);
				pos += 4 * sizeof(unsigned short int);
				printf(" %d %d %d %d\n", op1, op2, op3, op4);
				break;
			case FALSEJUMP:
			case FIELD:
			case FUNCVALUE:
			case CALL:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				pos += sizeof(unsigned short int);
				printf(" %d\n", op1);
				break;
			case INDEX:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				n = fread(&op2, sizeof(unsigned short int), 1, ifile);
				n = fread(&op3, sizeof(unsigned short int), 1, ifile);
				pos += 3 * sizeof(unsigned short int);
				printf(" %d %d %d\n", op1, op2, op3);
				break;
			case DELAY:
			case EXITMON:
			case CONTINUE:
				printf("\n");
				break;
			case BEGINMON:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				n = fread(&op2, sizeof(unsigned short int), 1, ifile);
				n = fread(&op3, sizeof(unsigned short int), 1, ifile);
				n = fread(&op4, sizeof(unsigned short int), 1, ifile);
				pos += 4 * sizeof(unsigned short int);
				printf(" %d %d %d %d\n", op1, op2, op3, op4);
				break;
			case INITCLASS:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				n = fread(&op2, sizeof(unsigned short int), 1, ifile);
				pos += 2 * sizeof(unsigned short int);
				printf(" %d %d\n", op1, op2);
				break;
			case ENDMON:
				printf("\n");
				break;
			case PUSHLOCAL:
			case NESTRUCT:
			case COPYSTRUCT:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				pos += sizeof(unsigned short int);
				printf(" %d\n", op1);
				break;
			case COPYBYTE:
				printf("\n");
				break;
			case ENTER:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				n = fread(&op2, sizeof(unsigned short int), 1, ifile);
				n = fread(&op3, sizeof(unsigned short int), 1, ifile);
				n = fread(&op4, sizeof(unsigned short int), 1, ifile);
				pos += 4 * sizeof(unsigned short int);
				printf(" %d %d %d %d\n", op1, op2, op3, op4);
				break;
			case IO:
			case EXIT:
			case PUSHBYTE:
			case ORWORD:
			case NEWORD:
				printf("\n");
				break;
			case RANGE:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				n = fread(&op2, sizeof(unsigned short int), 1, ifile);
				pos += 2 * sizeof(unsigned short int);
				printf(" %d %d\n", op1, op2);
				break;
			case GRWORD:
			case PUSHIND:
				printf("\n");
				break;
			case POP:
			case CONSTADDR:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				pos += sizeof(unsigned short int);
				printf(" %d\n", op1);
				break;
			case NGWORD:
			case ANDWORD:
			case DIVWORD:
			case MULWORD:
			case NOT:
				printf("\n");
				break;
			case EQSTRUCT:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				pos += sizeof(unsigned short int);
				printf(" %d\n", op1);
				break;
			case INCRWORD:
			case LSWORD:
			case ATTRIBUTE:
			case SETHEAP:
				printf("\n");
				break;
			case PUSHLABEL:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				pos += sizeof(unsigned short int);
				printf(" %d\n", op1);
				break;
			case CALLPROG:
				printf("\n");
				break;
			case ENTERPROC:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				n = fread(&op2, sizeof(unsigned short int), 1, ifile);
				n = fread(&op3, sizeof(unsigned short int), 1, ifile);
				n = fread(&op4, sizeof(unsigned short int), 1, ifile);
				pos += 4 * sizeof(unsigned short int);
				printf(" %d %d %d %d\n", op1, op2, op3, op4);
				break;
			case EXITPROC:
				printf("\n");
				break;
			case BEGINPROC:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				pos += sizeof(unsigned short int);
				printf(" %d\n", op1);
				break;
			case ENDPROC:
			case WAIT:
				printf("\n");
				break;
			case CASEJUMP:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				n = fread(&op2, sizeof(unsigned short int), 1, ifile);
				pos += 2 * sizeof(unsigned short int);
				printf(" %d %d", op1, op2);
				for (i = 0; i <= op2; i++)
				{
					fread(&op3, sizeof(unsigned short int), 1, ifile);
					pos += sizeof(unsigned short int);
					printf(" %d", op3);
				}
				printf("\n");
				break;
			case INITMON:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				n = fread(&op2, sizeof(unsigned short int), 1, ifile);
				pos += 2 * sizeof(unsigned short int);
				printf(" %d %d\n", op1, op2);
				break;
			case INITPROC:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				n = fread(&op2, sizeof(unsigned short int), 1, ifile);
				n = fread(&op3, sizeof(unsigned short int), 1, ifile);
				n = fread(&op4, sizeof(unsigned short int), 1, ifile);
				pos += 4 * sizeof(unsigned short int);
				printf(" %d %d %d %d\n", op1, op2, op3, op4);
				break;

			/* Required by JOBSTREAM */

			case SUCCWORD:
			case STOP:
			case START:
			case PUSHSET:
			case INSET:
			case NEGWORD:
			case ABSWORD:
			case NLWORD:
			case DECRWORD:
			case BUILDSET:
			case COPYSET:
			case ORSET:
				printf("\n");
				break;

			/* Required by REALTIME */

			case PUSHREAL:
			case COPYREAL:
			case ADDREAL:
			case NLREAL:
			case SUBREAL:
			case DIVREAL:
			case TRUNCREAL:
			case CONVWORD:
			case MULREAL:
			case ABSREAL:
			case LSREAL:
				printf("\n");
				break;

			/* Required by PIPELINE */

			case NEWLINE:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				pos += sizeof(unsigned short int);
				printf(" %d\n", op1);
				break;

			/* Required by Sequential Pascal Programs */

			case ENTERPROG:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				n = fread(&op2, sizeof(unsigned short int), 1, ifile);
				n = fread(&op3, sizeof(unsigned short int), 1, ifile);
				n = fread(&op4, sizeof(unsigned short int), 1, ifile);
				pos += 4 * sizeof(unsigned short int);
				printf(" %d %d %d %d\n", op1, op2, op3, op4);
				break;
			case EXITPROG:
				printf("\n");
				break;

			/* Required by BACKUP */

			case CALLSYS:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				pos += sizeof(unsigned short int);
				printf(" %d\n", op1);
				break;
			case SUBSET:
			case EQSET:
				printf("\n");
				break;
			case INITVAR:
			case COPYTAG:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				pos += sizeof(unsigned short int);
				printf(" %d\n", op1);
				break;
			case VARIANT:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				n = fread(&op2, sizeof(unsigned short int), 1, ifile);
				pos += 2 * sizeof(unsigned short int);
				printf(" %d %d\n", op1, op2);
				break;

			/* Required by CARDS */

			case PREDWORD:
				printf("\n");
				break;

			/* Required by CPASS1 */

			case NEW:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				n = fread(&op2, sizeof(unsigned short int), 1, ifile);
				pos += 2 * sizeof(unsigned short int);
				printf(" %d %d\n", op1, op2);
				break;
			case NGREAL:
			case GRREAL:
			case EQREAL:
				printf("\n");
				break;

			/* Required by CPASS2 */

			case POINTER:
				printf("\n");
				break;

			/* Required by CPASS3 */

			case NEWINIT:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				n = fread(&op2, sizeof(unsigned short int), 1, ifile);
				pos += 2 * sizeof(unsigned short int);
				printf(" %d %d\n", op1, op2);
				break;

			/* Required by FILE */

			case NEGREAL:
			case NESET:
				printf("\n");
				break;

			/* Required by FILE */

			case NGSTRUCT:
			case LSSTRUCT:
				n = fread(&op1, sizeof(unsigned short int), 1, ifile);
				pos += sizeof(unsigned short int);
				printf(" %d\n", op1);
				break;

			default:
				printf(" UNRECOGNIZED \n");
				exit(0);
		}
	}
}

void Header(void)
{
	fread(&header, sizeof(THeader), 1, ifile);
	printf("Header:\n");
	printf("  %05d: %d %d %d %d\n\n", 0, header.ProgLen, header.CodeLen, header.StackLen, header.VarLen);
	printf("  Program  Length: %5d byte(s)\n", header.ProgLen);
	printf("  Code     Length: %5d byte(s)\n", header.CodeLen);
	printf("  Stack    Length: %5d byte(s)\n", header.StackLen);
	printf("  Variable Length: %5d byte(s)\n", header.VarLen);
}

void VirtualCode(void)
{
	Header();
	Program();
	Constants();
	Trailer();
}

int main(int argc, char *argv[])
{
	if ((argc != 2) || (argv[0] == NULL))
	{
		printf ("Usage is: Decoder <Concurrent Pascal Virtual Code File>\n");
	}
	else
	{
		ifile = fopen (argv[1], "rb");
		if (ifile == NULL) 
		{
			printf ("Error opening file: %s\n", argv[1]);
		}
		else
		{
			printf("Concurrent Pascal Virtual Machine Decoder\nFile: %s\n\n", argv[1]);
			VirtualCode();
			fclose(ifile);
		}
	}	return EXIT_SUCCESS;
}