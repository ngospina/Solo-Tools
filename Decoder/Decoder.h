/*
   Decoder.h
   Copyright (c) 2009, 2016, 2022. Gerardo Ospina
*/

#ifndef _DECODER_H_

#define _DECODER_H_

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

typedef unsigned short int TWord;

typedef struct
{
	TWord ProgLen;
	TWord CodeLen;
	TWord StackLen;
	TWord VarLen;
} THeader;

#endif /* _DECODER_H_ */
