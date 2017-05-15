/*
    LCS.h
	Copyright (c) Gerardo Ospina 2011, 2015

	Longest Common Subsequence

	Adapted from: http://www.ics.uci.edu/~eppstein/161/960229.html
	David Eppstein. Ph.D.
	ICS 161: Design and Analysis of Algorithms
	Lecture notes for February 29, 1996
*/

#if !defined(_LCS_H_)

#define _LCS_H_

void create_L(unsigned int A_length, unsigned int B_length);
size_t LCSLength(const unsigned char *A, const unsigned char *B, unsigned int A_length, unsigned int B_length);
void LCS(const unsigned char *A, const unsigned char *B, unsigned char *lcs, unsigned int A_length, unsigned int B_length);
void destroy_L(void);

#endif
