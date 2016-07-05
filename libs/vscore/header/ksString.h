#ifndef KS_STRING_H
#define KS_STRING_H


#ifndef _STRING_H
#include <string.h>
#include <stdio.h>
#endif

/*
#ifndef _MALLOC_H
#include <malloc.h>
#endif
*/

/**
* String, dynamically allocated array of characters.
* \ingroup VRMLParser
*/
class String {

    class StringBuf {
    public:
	friend class String;
		StringBuf(const char *d)
		{ refs = 1; len = (int)strlen(d); data = new char[len + 1]; strcpy(data, d); capacity = len + 1; }	//CHANGED BY I.K. added explicit cast
	    ~StringBuf()
		{ delete [] data; }
    protected:
	char   *data;
	int	refs;
	int	len;
	int	capacity;
    };

    public:
		String();
		String(const char *str);
		String(char c);
		String(const String &s);
	       ~String();

    String     &operator =(const String &s);
    String     &operator +=(char c);
    String     &operator +=(const char *s);
	        operator const char *() const { return _stringBuf->data; }
    int		operator ==(const String &str) const;
    int		length() const { return _stringBuf->len; }

protected:
    StringBuf  *_stringBuf;
};

extern int hash(String key);

#endif // KS_STRING_H
