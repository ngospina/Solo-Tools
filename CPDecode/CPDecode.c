/* Concurrent Pascal Virtual Machine Decoder

   Copyright (c) 2009, 2022, Gerardo Ospina

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

#include "CPDecode.h" 

void Trailer(FILE *ifile, TWord *pos)
{
	TWord word;
	int n;

	printf("\nTrailer:\n");
	n = fread(&word, sizeof(TWord), 1, ifile);
	while (n == 1)
	{
		*pos += sizeof(TWord);
		printf("  %05d: [%5d]\n", *pos, word);
	    n = fread(&word, sizeof(TWord), 1, ifile);
	}
}

void Constants(FILE *ifile, TWord progLen, TWord *pos)
{
	int max = progLen + sizeof(THeader) - sizeof(TWord);
	TWord word, low, high;
	int n;

	printf("\nConstants:\n");
	while (*pos < max)
	{
    	fread(&word, sizeof(TWord), 1, ifile);
		*pos += sizeof(TWord);
		high = (word & 0xFF00) >> 8;
		low = word & 0xFF;
		if (low >= ' ' && low <= '~')
		{
			printf("  %05d: [%05d:%3d] %c\n", *pos, *pos - sizeof(THeader), low, low);
		}
		else
		{
			printf("  %05d: [%05d:%3d]\n", *pos, *pos - sizeof(THeader), low);
		}
		if (high >= ' ' && high <= '~')
		{
			printf("  %05d: [%05d:%3d] %c\n", *pos + 1, *pos - sizeof(THeader) + 1, high, high);
		}
		else
		{
			printf("  %05d: [%05d:%3d]\n", *pos + 1, *pos - sizeof(THeader) + 1, high);
		}
	}
}

void Program(FILE *ifile, TWord codeLen, TWord *pos)
{
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

	int max = codeLen + sizeof(THeader) - sizeof(TWord);
	TWord op, op1, op2, op3, op4;
	int i, n;

	printf("\nCode:\n");
	while (*pos < max)
	{
    	fread(&op, sizeof(TWord), 1, ifile);
		*pos += sizeof(TWord);
		printf("  %05d: [%05d:%3d]", *pos, *pos - sizeof(THeader), op);
		if (op > 1 && op < 225)
		{
			printf(" %s", TABL99[op]);
		}
		switch (op)
		{
			/* Required by SOLO */

			case JUMP:
				fread(&op1, sizeof(TWord), 1, ifile);
				*pos += sizeof(TWord);
				printf(" %hu\n", op1);
				break;
			case ENTERCLASS:
				fread(&op1, sizeof(TWord), 1, ifile);
				fread(&op2, sizeof(TWord), 1, ifile);
				fread(&op3, sizeof(TWord), 1, ifile);
				fread(&op4, sizeof(TWord), 1, ifile);
				*pos += 4 * sizeof(TWord);
				printf(" %hu %hu %hu %hu\n", op1, op2, op3, op4);
				break;
			case LOCALADDR:
			case PUSHGLOBAL:
				fread(&op1, sizeof(TWord), 1, ifile);
				*pos += sizeof(TWord);
				printf(" %hu\n", op1);
				break;
			case COPYWORD:
				printf("\n");
				break;
			case GLOBALADDR:
				fread(&op1, sizeof(TWord), 1, ifile);
				*pos += sizeof(TWord);
				printf(" %hu\n", op1);
				break;			
			case MODWORD:
				printf("\n");
				break;
			case PUSHCONST:
				fread(&op1, sizeof(TWord), 1, ifile);
				*pos += sizeof(TWord);
				printf(" %hu\n", op1);
				break;			
			case ADDWORD:
			case EXITCLASS:
			case SUBWORD:
			case EQWORD:
				printf("\n");
				break;
			case BEGINCLASS:
				fread(&op1, sizeof(TWord), 1, ifile);
				fread(&op2, sizeof(TWord), 1, ifile);
				fread(&op3, sizeof(TWord), 1, ifile);
				fread(&op4, sizeof(TWord), 1, ifile);
				*pos += 4 * sizeof(TWord);
				printf(" %hu %hu %hu %hu\n", op1, op2, op3, op4);
				break;
			case ENDCLASS:
			case ANDSET:
				printf("\n");
				break;
			case ENTERMON:
				fread(&op1, sizeof(TWord), 1, ifile);
				fread(&op2, sizeof(TWord), 1, ifile);
				fread(&op3, sizeof(TWord), 1, ifile);
				fread(&op4, sizeof(TWord), 1, ifile);
				*pos += 4 * sizeof(TWord);
				printf(" %hu %hu %hu %hu\n", op1, op2, op3, op4);
				break;
			case FALSEJUMP:
			case FIELD:
			case FUNCVALUE:
			case CALL:
				fread(&op1, sizeof(TWord), 1, ifile);
				*pos += sizeof(TWord);
				printf(" %hu\n", op1);
				break;
			case INDEX:
				fread(&op1, sizeof(TWord), 1, ifile);
				fread(&op2, sizeof(TWord), 1, ifile);
				fread(&op3, sizeof(TWord), 1, ifile);
				*pos += 3 * sizeof(TWord);
				printf(" %hu %hu %hu\n", op1, op2, op3);
				break;
			case DELAY:
			case EXITMON:
			case CONTINUE:
				printf("\n");
				break;
			case BEGINMON:
				fread(&op1, sizeof(TWord), 1, ifile);
				fread(&op2, sizeof(TWord), 1, ifile);
				fread(&op3, sizeof(TWord), 1, ifile);
				fread(&op4, sizeof(TWord), 1, ifile);
				*pos += 4 * sizeof(TWord);
				printf(" %hu %hu %hu %hu\n", op1, op2, op3, op4);
				break;
			case INITCLASS:
				fread(&op1, sizeof(TWord), 1, ifile);
				fread(&op2, sizeof(TWord), 1, ifile);
				*pos += 2 * sizeof(TWord);
				printf(" %hu %hu\n", op1, op2);
				break;
			case ENDMON:
				printf("\n");
				break;
			case PUSHLOCAL:
			case NESTRUCT:
			case COPYSTRUCT:
				fread(&op1, sizeof(TWord), 1, ifile);
				*pos += sizeof(TWord);
				printf(" %hu\n", op1);
				break;
			case COPYBYTE:
				printf("\n");
				break;
			case ENTER:
				fread(&op1, sizeof(TWord), 1, ifile);
				fread(&op2, sizeof(TWord), 1, ifile);
				fread(&op3, sizeof(TWord), 1, ifile);
				fread(&op4, sizeof(TWord), 1, ifile);
				*pos += 4 * sizeof(TWord);
				printf(" %hu %hu %hu %hu\n", op1, op2, op3, op4);
				break;
			case IO:
			case EXIT:
			case PUSHBYTE:
			case ORWORD:
			case NEWORD:
				printf("\n");
				break;
			case RANGE:
				fread(&op1, sizeof(TWord), 1, ifile);
				fread(&op2, sizeof(TWord), 1, ifile);
				*pos += 2 * sizeof(TWord);
				printf(" %hu %hu\n", op1, op2);
				break;
			case GRWORD:
			case PUSHIND:
				printf("\n");
				break;
			case POP:
			case CONSTADDR:
				fread(&op1, sizeof(TWord), 1, ifile);
				*pos += sizeof(TWord);
				printf(" %hu\n", op1);
				break;
			case NGWORD:
			case ANDWORD:
			case DIVWORD:
			case MULWORD:
			case NOT:
				printf("\n");
				break;
			case EQSTRUCT:
				fread(&op1, sizeof(TWord), 1, ifile);
				*pos += sizeof(TWord);
				printf(" %hu\n", op1);
				break;
			case INCRWORD:
			case LSWORD:
			case ATTRIBUTE:
			case SETHEAP:
				printf("\n");
				break;
			case PUSHLABEL:
				fread(&op1, sizeof(TWord), 1, ifile);
				*pos += sizeof(TWord);
				printf(" %hu\n", op1);
				break;
			case CALLPROG:
				printf("\n");
				break;
			case ENTERPROC:
				fread(&op1, sizeof(TWord), 1, ifile);
				fread(&op2, sizeof(TWord), 1, ifile);
				fread(&op3, sizeof(TWord), 1, ifile);
				fread(&op4, sizeof(TWord), 1, ifile);
				*pos += 4 * sizeof(TWord);
				printf(" %hu %hu %hu %hu\n", op1, op2, op3, op4);
				break;
			case EXITPROC:
				printf("\n");
				break;
			case BEGINPROC:
				fread(&op1, sizeof(TWord), 1, ifile);
				*pos += sizeof(TWord);
				printf(" %hu\n", op1);
				break;
			case ENDPROC:
			case WAIT:
				printf("\n");
				break;
			case CASEJUMP:
				fread(&op1, sizeof(TWord), 1, ifile);
				fread(&op2, sizeof(TWord), 1, ifile);
				*pos += 2 * sizeof(TWord);
				printf(" %hu %hu\n", op1, op2);
				for (i = 0; i <= op2; i++)
				{
					fread(&op3, sizeof(TWord), 1, ifile);
					*pos += sizeof(TWord);
					printf(" %d", op3);
				}
				printf("\n");
				break;
			case INITMON:
				fread(&op1, sizeof(TWord), 1, ifile);
				fread(&op2, sizeof(TWord), 1, ifile);
				*pos += 2 * sizeof(TWord);
				printf(" %hu %hu\n", op1, op2);
				break;
			case INITPROC:
				fread(&op1, sizeof(TWord), 1, ifile);
				fread(&op2, sizeof(TWord), 1, ifile);
				fread(&op3, sizeof(TWord), 1, ifile);
				fread(&op4, sizeof(TWord), 1, ifile);
				*pos += 4 * sizeof(TWord);
				printf(" %hu %hu %hu %hu\n", op1, op2, op3, op4);
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
				fread(&op1, sizeof(TWord), 1, ifile);
				*pos += sizeof(TWord);
				printf(" %hu\n", op1);
				break;

			/* Required by Sequential Pascal Programs */

			case ENTERPROG:
				fread(&op1, sizeof(TWord), 1, ifile);
				fread(&op2, sizeof(TWord), 1, ifile);
				fread(&op3, sizeof(TWord), 1, ifile);
				fread(&op4, sizeof(TWord), 1, ifile);
				*pos += 4 * sizeof(TWord);
				printf(" %hu %hu %hu %hu\n", op1, op2, op3, op4);
				break;
			case EXITPROG:
				printf("\n");
				break;

			/* Required by BACKUP */

			case CALLSYS:
				fread(&op1, sizeof(TWord), 1, ifile);
				*pos += sizeof(TWord);
				printf(" %hu\n", op1);
				break;
			case SUBSET:
			case EQSET:
				printf("\n");
				break;
			case INITVAR:
			case COPYTAG:
				fread(&op1, sizeof(TWord), 1, ifile);
				*pos += sizeof(TWord);
				printf(" %hu\n", op1);
				break;
			case VARIANT:
				fread(&op1, sizeof(TWord), 1, ifile);
				fread(&op2, sizeof(TWord), 1, ifile);
				*pos += 2 * sizeof(TWord);
				printf(" %hu %hu\n", op1, op2);
				break;

			/* Required by CARDS */

			case PREDWORD:
				printf("\n");
				break;

			/* Required by CPASS1 */

			case NEW:
				fread(&op1, sizeof(TWord), 1, ifile);
				fread(&op2, sizeof(TWord), 1, ifile);
				*pos += 2 * sizeof(TWord);
				printf(" %hu %hu\n", op1, op2);
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
				fread(&op1, sizeof(TWord), 1, ifile);
				fread(&op2, sizeof(TWord), 1, ifile);
				*pos += 2 * sizeof(TWord);
				printf(" %hu %hu\n", op1, op2);
				break;

			/* Required by FILE */

			case NEGREAL:
			case NESET:
				printf("\n");
				break;

			/* Required by LIST */

			case NGSTRUCT:
			case LSSTRUCT:
				fread(&op1, sizeof(TWord), 1, ifile);
				*pos += sizeof(TWord);
				printf(" %hu\n", op1);
				break;

			default:
				printf(" UNRECOGNIZED %d\n", op);
				exit(0);
		}
	}
}

void Header(FILE *ifile, THeader *header, TWord *pos)
{
	fread(header, sizeof(THeader), 1, ifile);
	*pos += sizeof(THeader) - sizeof(TWord);
	printf("Header:\n");
	printf("  %05hu: %hu %hu %hu %hu\n\n", 0, header->ProgLen, header->CodeLen, header->StackLen, header->VarLen);
	printf("  Program  Length: %5hu byte(s)\n", header->ProgLen);
	printf("  Code     Length: %5hu byte(s)\n", header->CodeLen);
	printf("  Stack    Length: %5hu byte(s)\n", header->StackLen);
	printf("  Variable Length: %5hu byte(s)\n", header->VarLen);
}

void VirtualCode(FILE *ifile)
{
	TWord pos = 0;
	THeader header;

	Header(ifile, &header, &pos);
	Program(ifile, header.CodeLen, &pos);
	Constants(ifile, header.ProgLen, &pos);
	Trailer(ifile, &pos);
}

int main(int argc, char *argv[])
{

	if ((argc != 2) || (argv[0] == NULL))
	{
		printf ("Usage is: CPDecode <Concurrent Pascal Virtual Code File>\n");
	}
	else
	{
		FILE *ifile = fopen (argv[1], "rb");
		if (ifile == NULL) 
		{
			printf ("Error opening file: %s\n", argv[1]);
		}
		else
		{
			printf("Concurrent Pascal Virtual Machine Decoder\nFile: %s\n\n", argv[1]);
			VirtualCode(ifile);
			fclose(ifile);
		}
	}	return EXIT_SUCCESS;
}
