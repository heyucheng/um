/*
 *	filename: load.c
 *      by Yucheng He and Griffin Richards
 *	11/23/2015
 * 	Include the source code for load function and all the helper functions
 *      for load.
 */
#include "load.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <bitpack.h>
#include <sys/stat.h>

/* pack 4 bytes into a word instruction */
uint32_t getInstr(FILE* input, int a);
/* check the input file format is the right um code format */
void checkInput(int size);
/* get the size of the file in bytes with stat */
int getSize(char* filename);


Segment_T load(FILE* input, char* filename)
{	
	int size = getSize(filename);
	checkInput(size);
	Segment_T program = Seg_new(size / 4);
	int a;
	/* pack four bytes into a word instruction wiht getInstr */
	while ((a = getc(input)) != EOF) {
		addInstr(program, getInstr(input, a));
	} 
	
	return program;
}


uint32_t getInstr(FILE* input, int a)
{
	uint64_t word = 0;
	int b = getc(input);
	int c = getc(input);
	int d = getc(input);
	word = Bitpack_newu(word, 8, 24, a);
	word = Bitpack_newu(word, 8, 16, b);
	word = Bitpack_newu(word, 8, 8, c);
	word = Bitpack_newu(word, 8, 0, d);
	return (uint32_t) word;
}

void checkInput(int size)
{
	/* check the size of file should be the multiple of 4 and not 0 */
	if (size % 4 != 0 || size == 0) {
		fprintf(stderr, "Improper file content\n");
		exit(EXIT_FAILURE);
	} 
	return;
}

int getSize(char* filename)
{
 	struct stat fileInfo;
	stat(filename, &fileInfo);
	int size = fileInfo.st_size;
	return size;
}