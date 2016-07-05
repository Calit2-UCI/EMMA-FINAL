#if _MSC_VER >= 1400
 #define _CRT_SECURE_NO_DEPRECATE				// shut up the vs2005 compiler
 #define _CRT_NONSTDC_NO_DEPRECATE
#endif

#ifdef __WIN32__
	#ifndef WIN32
    	#define WIN32
    #endif //WIN32
#endif //__WIN32__

// define MEMSTDALLOC to force using malloc,...
#define MEMSTDALLOC 

#if ((!defined WIN32) || (defined MEMSTDALLOC))

#include <stdlib.h>
#include <cstring>
//#include <memory.h>

void *MEMALLOC( size_t size )
{
	void * v = malloc(size);
    memset(v,0,size);
    return v;
}

void *MEMREALLOC(void *block, size_t size)
{
	if (!block)
		return MEMALLOC(size);
	return realloc(block,size);
}

void MEMFREE(void *block)
{
	free(block);
}

#else // ifdef WIN32
#include <windows.h>

void *MEMALLOC( size_t size )
{
	DWORD d = GetLastError();
    return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY + HEAP_GENERATE_EXCEPTIONS, size);
}

void *MEMREALLOC(void *block, size_t size)
{
	if (!block)
		return MEMALLOC(size);
	DWORD d = GetLastError();
	return HeapReAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY + HEAP_GENERATE_EXCEPTIONS, block, size);
}

void MEMFREE(void *block)
{
	HeapFree( GetProcessHeap(), NULL, block);
}

#endif// endifdef WIN32


