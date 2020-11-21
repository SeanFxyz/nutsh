#ifndef _nutsh_string_h
#define _nutsh_string_h

#include <stdlib.h>
#include <string.h>

#define NSTR_DEFAULT_CAP 128
#define NSTR_DEFAULT_EX 64

typedef struct nutsh_string {
	int cap; // Current capacity of character array
	char *cs; // Character string data
	int ex; // Extra space to leave for new characters when resizing
} nstr;

nstr nstrInit(nstr *str)
{
	char *newcs = malloc(sizeof(char) * NSTR_DEFAULT_CAP);
	str->cap = NSTR_DEFAULT_CAP;
	str->cs = newcs;
	str->ex = NSTR_DEFAULT_EX;
}

nstr nstrNew()
{
	nstr new_nstr;
	nstrInit(&new_nstr);
	return new_nstr;
}

// Free the nstr's cs
void nstrDel(nstr *str)
{
	free(str->cs);
}

// set the capacity of the string, copy as much of the old string data
// as possible into the new cs field
void nstrSetCap(nstr *str, int newcap)
{
	char *newcs = malloc(sizeof(char) * newcap);
	strncpy(newcs, str->cs, newcap);
	free(str->cs);
	str->cs = newcs;
	str->cap = newcap;
}

// grow the capacity of a string to accomodate newlen
void nstrGrow(nstr *str, int newlen)
{
	if (newlen > str->cap) {
		newlen += str->ex;
		nstrSetCap(str, newlen);
	}
}

// shrink the capacity of a string to current content
void nstrShrink(nstr *str)
{
	nstrSetCap(str, strlen(str->cs));
}

// return a minimum-length c-string with the nstr's contents
char *nstrMinCs(nstr str)
{
	nstrShrink(&str);
	return str.cs;
}

// append a char to str
void nstrAppendCh(nstr *str, char ch)
{
	int oldlen = strlen(str->cs);
	nstrGrow(str, oldlen + 1);
	str->cs[oldlen] = ch;
	str->cs[oldlen + 1] = '\0';
}

// append a c-string to str
void nstrAppendCs(nstr *str, char* cs)
{
	int oldlen = strlen(str->cs);
	int newlen = oldlen + strlen(cs);
	nstrGrow(str, newlen);
	strcpy(str->cs + sizeof(char) * oldlen, cs);
}

// set the content of the nstr to the content of ch
void nstrSetCs(nstr *str, char* ch)
{
	nstrGrow(str, strlen(ch));
	strcpy(str->cs, ch);
}

// get a line of stdin and store in str
void nstrGetline(nstr *str)
{
	char c;
	nstrSetCs(str, "");
	while ((c = getchar()) != EOF && c != '\n')
		nstrAppendCh(str, c);
}

#endif
