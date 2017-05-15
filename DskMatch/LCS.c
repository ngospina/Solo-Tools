/*
    LCS.c
	Copyright (c) Gerardo Ospina 2011, 2015-2016

	Longest Common Subsequence

	Adapted from: http://www.ics.uci.edu/~eppstein/161/960229.html
	David Eppstein. Ph.D.
	ICS 161: Design and Analysis of Algorithms
	Lecture notes for February 29, 1996
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "LCS.h"

static size_t **L = NULL;
static unsigned int A_sequence_length = 0;
static unsigned int B_sequence_length = 0;

/* 
	Compute the longest common subsequence between A and B
	On return, L will contain the LCS table.
	L[0][0] will contain the length of the longest common subsequence.
*/

static void LCSBacktrack(const unsigned char *A, const unsigned char *B, 
	unsigned int A_length, unsigned int B_length)
{
	int i, j;

	for (i = A_length; i >= 0; i--) {
	    for (j = B_length; j >= 0; j--) {
			if (i == A_length || j == B_length) {
				L[i][j] = 0;
			} else if (A[i] == B[j]) {
				L[i][j] = L[i + 1][j + 1] + 1;
			} else if (L[i + 1][j] > L[i][j + 1]) {
				L[i][j] = L[i + 1][j];
			} else {
				L[i][j] = L[i][j + 1];
			}
		}
	}
}

static void ReadLCSFromBacktrack(const unsigned char *A, const unsigned char *B, unsigned char *lcs, 
	unsigned int A_length, unsigned int B_length)
{
    size_t i = 0, j = 0, pos = 0;

    while (i < A_length && j < B_length) {
		if (A[i] == B[j]) {
			lcs[pos] = A[i];
			i++; 
			j++;
			pos++;
		} else if (L[i + 1][j] >= L[i][j + 1]) {
			i++;
		} else {
			j++;
		}
    }
}

void create_L(unsigned int A_length, unsigned int B_length)
{
	assert(L == NULL && A_sequence_length == 0 && B_sequence_length == 0);
	unsigned int i;

	A_sequence_length = A_length;
	B_sequence_length = B_length;
	L = (size_t **) malloc((A_length + 1) * sizeof(size_t *));
	for (i = 0; i <= A_length; i++) {
		L[i] = (size_t *) malloc((B_length + 1) * sizeof(size_t));
	}
}

size_t LCSLength(const unsigned char *A, const unsigned char *B, 
	unsigned int A_length, unsigned int B_length)
{
	assert(L != NULL && A_sequence_length > 0 && B_sequence_length > 0);
	LCSBacktrack(A, B, A_length, B_length);
	return L[0][0];
}

void LCS(const unsigned char *A, const unsigned char *B, unsigned char *lcs, 
	unsigned int A_length, unsigned int B_length)
{
	assert(L != NULL && A_sequence_length > 0 && B_sequence_length > 0);
	LCSBacktrack(A, B, A_length, B_length);
	memset(lcs, 0, A_length + 1); 
	ReadLCSFromBacktrack(A, B, lcs, A_length, B_length);
}

void destroy_L(void)
{
	assert(L != NULL && A_sequence_length > 0 && B_sequence_length > 0);
	unsigned int i;

	for (i = 0; i < A_sequence_length; i++) {
		free(L[i]);
		L[i] = NULL;
	}
	free(L);
	L = NULL;
	A_sequence_length = 0;
	B_sequence_length = 0;
}
