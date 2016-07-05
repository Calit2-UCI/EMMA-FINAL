


#ifndef __HEADER_MEMALLOC_H
#define __HEADER_MEMALLOC_H

#if _MSC_VER >= 1400
 #define _CRT_SECURE_NO_DEPRECATE				// shut up the vs2005 compiler
 #define _CRT_NONSTDC_NO_DEPRECATE
#endif


#include <stdio.h>

void *MEMALLOC( size_t size );
void *MEMREALLOC(void * block, size_t size);
void MEMFREE(void *block);


#endif //__HEADER_MEMALLOC_H
