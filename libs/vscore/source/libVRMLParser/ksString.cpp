#include "stdafx.h"
#include "ksString.h"

String::String()
{
	_stringBuf = new StringBuf("");
}

String::String(const char *str)
{
	_stringBuf = new StringBuf(str);
}

String::String(char c)
{
	char *str;
	
	str = new char[2];
	str[0] = c;
	str[1] = '\0';
	_stringBuf = new StringBuf(str);
	delete [] str;
}

String::String(const String &s)
{
	_stringBuf = new StringBuf(s._stringBuf->data);
}

String::~String()
{
	delete _stringBuf;
}

String	&String::operator =(const String &s)
{
	if (&s != this)
	{
		int len = s.length();

		if (len < _stringBuf->capacity)
		{
			char *str = _stringBuf->data;

			_stringBuf->len = len;
			strcpy(str, s._stringBuf->data);
			*(str + len) = '\0';
		}
		else 
		{
			delete _stringBuf;
			_stringBuf = new StringBuf(s._stringBuf->data);
		}
	}
	return *this;
}

String	&String::operator +=(char c)
{
	int len = _stringBuf->len;
	char *str;

	if ((len + 1) < _stringBuf->capacity)
	{
		str = _stringBuf->data;
		_stringBuf->len += 1;
		*(str + len) = c;
		*(str + len + 1) = '\0';
	}
	else
	{
		str = new char[len + 2];
		strcpy(str, _stringBuf->data);
		str[len] = c;
		str[len + 1] = '\0';

		delete _stringBuf;
		_stringBuf = new StringBuf(str);
		delete [] str;
	}
	return *this;
}

String	&String::operator +=(const char *s)
{
	int len = _stringBuf->len;
	int length = len + (int)strlen(s);	//CHANGED BY I.K. added explicit cast
	char *str;

	if (length < _stringBuf->capacity)
	{
		str = _stringBuf->data;
		_stringBuf->len = length;
		strcpy(str + len, s);
		*(str + length) = '\0';
	}
	else
	{
		str = new char[length + 1];
		strcpy(str, _stringBuf->data);
		strcpy(str + len, s);
		str[length] = '\0';

		delete _stringBuf;
		_stringBuf = new StringBuf(str);
		delete [] str;
	}
	return *this;
}

int String::operator ==(const String &str) const
{
	int i;
	char *strData;
	char *thisData;

	// checks if str points to the same object as this
	if (&str == this) return 1;
	// compares the str length with the length of this
	// object, and then compares every character of these
	// two objects in corresponding positions
	if (str._stringBuf->len != _stringBuf->len)
		return 0;
	strData = str._stringBuf->data;
	thisData = _stringBuf->data;
	for (i = 0; i < str._stringBuf->len; i++)
	{
		if (*(strData + i) != *(thisData + i)) return 0; 
	}

	// no inequality was previously found
	return 1;
}

int hash(String key)
{
	int hash = 17;
	int length = key.length();
	const char *data = (const char *) key;


	hash = 37 * hash + length;
	int sum = 0;
	for (int i = 0; i < length; i++)
	{
		sum += data[i];
	}
	hash = 37 * hash + sum;

	return hash;
}
