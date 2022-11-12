/*
    SOLO.h
    Copyrighth (C) 2022. Gerardo Ospina
*/

#ifndef _SOLO_H_

#define _SOLO_H_

#define CATALOG_PAGE	154
#define PAGE_LENGTH		512
#define PAGEMAP_LENGTH	(PAGE_LENGTH / sizeof(unsigned short))

#define ID_LENGTH		12
#define FILEMAP_LENGTH	(PAGE_LENGTH / sizeof(TFileEntry))

#define PATH_LENGTH		256

typedef unsigned char	TPage[PAGE_LENGTH];
typedef unsigned short	TPageMap[PAGEMAP_LENGTH];

typedef union {
	TPage    page;
	TPageMap pagemap;
} TFile;

typedef struct {
	unsigned char id[ID_LENGTH];
	unsigned short kind, addr, protect;
	unsigned short unused[5];
	unsigned short key;
	unsigned short searchlength;
} TFileEntry;

typedef TFileEntry TFileMap[FILEMAP_LENGTH];

typedef union {
	TPage page;
	TFileMap filemap;
} TCatalog;

#endif /* _SOLO_H_ */

