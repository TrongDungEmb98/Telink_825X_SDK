/********************************************************************************************************
 * @file     string.c 
 *
 * @brief    for TLSR chips
 *
 * @author	 public@telink-semi.com;
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
#include "types.h"
#include "string.h"
#include "../common/assert.h"
#include "../tl_common.h"

char* strcpy(char * dst0, const char * src0) {
	char *s = dst0;
	while ((*dst0++ = *src0++))
		;
	return s;
}

char * strchr(const char *s, int c) {
	do {
		if (*s == c) {
			return (char*) s;
		}
	} while (*s++);
	return (0);
}

int memcmp(const void * m1, const void *m2, size_t len) {
	u8 *st1 = (u8 *) m1;
	u8 *st2 = (u8 *) m2;

	while(len--){
		if(*st1 != *st2){
			return (*st1 - *st2);
		}
		st1++;
		st2++;
	}
	return 0;
}

void * memchr(register const void * src_void, int c, size_t length) {
	const unsigned char *src = (const unsigned char *) src_void;

	while (length-- > 0) {
		if (*src == c)
			return (void *) src;
		src++;
	}
	return NULL;
}

void * memmove(void * dest, const void * src, size_t n) {
	char * d = (char *)dest;
	char * s = (char *)src;

	while (n--)
		*d++ = *s++;

	return dest;
}

void bbcopy(register char * src, register char * dest, int len) {
	if (dest < src)
		while (len--)
			*dest++ = *src++;
	else {
		char *lasts = src + (len - 1);
		char *lastd = dest + (len - 1);
		while (len--)
			*(char *) lastd-- = *(char *) lasts--;
	}
}

void bcopy(register char * src, register char * dest, int len) {
	bbcopy(src, dest, len);
}

void * memset(void * dest, int val, size_t len) {
	register unsigned char *ptr = (unsigned char*) dest;
	while (len-- > 0)
		*ptr++ = (unsigned char)val;
	return dest;
}

void * memcpy(void * out, const void * in, size_t length) {
	bcopy((char *) in, (char *) out, (int) length);
	return out;
}

// for performance, assume lenght % 4 == 0,  and no memory overlapped
void memcpy4(void * d, const void * s, size_t length){
	int* dst = (int*)d;
	int* src = (int*)s;
	assert((((int)dst) >> 2) << 2 == ((int)dst));			// address must alighn to 4
	assert((((int)src) >> 2) << 2 == ((int)src));			// address must alighn to 4
	assert((length >> 2) << 2 == length);					// lenght % 4 == 0
	assert(( ((char*)dst) + length <= (const char*)src) || (((const char*)src) + length <= (char*)dst));	//  no overlapped
	size_t len = length >> 2;
	while(len --){
		*dst++ = *src++;
	}
}

size_t strlen(const char *str) {

	size_t len = 0;

	if (str != NULL) {
		while (*str++) {

			len++;

		}
	}

	return len;
}

int strcmp(const char* firstString, const char* secondString) {
	while (*firstString == *secondString) {
		if (*firstString == '\0') {
			return 0;
		}
		++firstString;
		++secondString;
	}
	if (((unsigned char) *firstString - (unsigned char) *secondString) < 0) {
		return -1;
	}
	return 1;
}

int strxcmp(const char* firstString, const char* secondString) {
	while (*firstString == *secondString) {
		if (*firstString == '\0') {
			return 0;
		}
		++firstString;
		++secondString;
	}
	
	if (*firstString == '\0') return 0;

	if (((unsigned char) *firstString - (unsigned char) *secondString) < 0) {
		return -1;
	}
	return 1;
}

char *strstr(const char* src, const char *sub)
{
    if(src ==0 || 0 == sub)
    {
        return (char*)src;
    }
    
    const char *bp=src;
    const char *sp=sub;
    
    while(*src)
    {
        bp=src;
        sp=sub;
        do{
            if(!*sp)
                return (char*)src;
            }while(*bp++==*sp++);
        src+=1;
    }  
    return 0;
}

char * strncpy(char *s, const char *t, size_t n) {
	char *p = s;
	size_t i = 0;

	if (!s)
		return s;

	while (t && i < n) {
		*s++ = *t++;
		i++;
	}

	if (!t) {
		do
			*s++ = '\0';
		while (i++ < n);
	}
	return p;
}

int ismemzero4(void *data, size_t len){
	int *p = (int*)data;
	len = len >> 2;
	for(int i = 0; i < len; ++i){
		if(*p){
			return 0;
		}
		++p;
	}
	return 1;
}

int ismemf4(void *data, size_t len){
	int *p = (int*)data;
	len = len >> 2;
	for(int i = 0; i < len; ++i){
		if(*p != 0xffffffff){
			return 0;
		}
		++p;
	}
	return 1;
}

void * memset4(void * dest, int val, size_t len) {
	int *p = (int*)dest;
	len = len >> 2;
	for(int i = 0; i < len; ++i){
		*p++ = val;
	}
	return dest;
}

void zeromem4(void *data, size_t len){
	memset4(data, 0, len);
}


int strncmp (const char *s1, const char *s2, size_t n)
{
  unsigned char c1 = '\0';
  unsigned char c2 = '\0';
  if (n >= 4)
    {
      size_t n4 = n >> 2;
      do
        {
          c1 = (unsigned char) *s1++;
          c2 = (unsigned char) *s2++;
          if (c1 == '\0' || c1 != c2)
            return c1 - c2;
          c1 = (unsigned char) *s1++;
          c2 = (unsigned char) *s2++;
          if (c1 == '\0' || c1 != c2)
            return c1 - c2;
          c1 = (unsigned char) *s1++;
          c2 = (unsigned char) *s2++;
          if (c1 == '\0' || c1 != c2)
            return c1 - c2;
          c1 = (unsigned char) *s1++;
          c2 = (unsigned char) *s2++;
          if (c1 == '\0' || c1 != c2)
            return c1 - c2;
        } while (--n4 > 0);
      n &= 3;
    }
  while (n > 0)
    {
      c1 = (unsigned char) *s1++;
      c2 = (unsigned char) *s2++;
      if (c1 == '\0' || c1 != c2)
        return c1 - c2;
      n--;
    }
  return c1 - c2;
}