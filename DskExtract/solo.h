/*
    solo.h
    Copyright (C) 2022, 2025. Gerardo Ospina
*/

#ifndef _SOLO_H_

#define _SOLO_H_

#define DISK_PAGES      4800
#define KERNEL_PAGE	    0
#define SOLO_PAGE		24
#define OTHER_OS_PAGE	88
#define CATALOG_PAGE    154
#define PAGE_LENGTH     512
#define PAGEMAP_LENGTH  (PAGE_LENGTH / sizeof(unsigned short))

#define ID_LENGTH       12
#define FILEMAP_LENGTH  (PAGE_LENGTH / sizeof(TFileEntry))

#define PATH_LENGTH     1024

#define EMPTY	0
#define SCRATCH	1
#define ASCII	2
#define SEQCODE	3
#define CONCODE	4

typedef unsigned char  TPage[PAGE_LENGTH];
typedef unsigned short TPageMap[PAGEMAP_LENGTH];

typedef union {
    TPage    page;
    TPageMap pagemap;
} TFile;

typedef struct {
    unsigned char  id[ID_LENGTH];
    unsigned short kind, addr, protect;
    unsigned short unused[5];
    unsigned short key;
    unsigned short searchlength;
} TFileEntry;

typedef TFileEntry TFileMap[FILEMAP_LENGTH];

typedef union {
    TPage    page;
    TFileMap filemap;
} TCatalog;

#endif /* _SOLO_H_ */
