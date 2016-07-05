#ifndef _FIELDVALUE_H
#define _FIELDVALUE_H

#ifndef _ARRAY_H
#include "Array.h"
#endif
#ifndef KS_STRING_H
#include "ksString.h"
#endif
#ifndef _STDIO_H
#include <stdio.h>
#endif

#include <cstring>

class Scene;
class Node;

typedef Array<int>	IntArray;
typedef Array<float>	FloatArray;
typedef Array<String>	StringArray;

enum {
    SFBOOL,
    SFCOLOR,
    SFIMAGE,
    SFINT32,
    SFFLOAT,
    SFNODE,
    SFROTATION,
    SFSTRING,
    SFTIME,
    SFVEC2F,
    SFVEC3F,
    MFCOLOR,
    MFINT32,
    MFFLOAT,
    MFNODE,
    MFROTATION,
    MFSTRING,
    MFTIME,
    MFVEC2F,
    MFVEC3F
};
/**(Pure virtual)Parent class of SFString, SFFloat etc. containing information about the Single field values.
Also contains the enumeration of all field types, SFSTRING,MFNODE etc...
\ingroup VRMLParser*/
class FieldValue 
{
public:
						FieldValue();
						FieldValue(const FieldValue &value);
    virtual 			~FieldValue() {}
    virtual void		write(FILE *f, int indent) const = 0;
    virtual int			getType() const = 0;
    virtual bool		equals(const FieldValue *value) const = 0;
	virtual const char	*getTypeName() const = 0;
    virtual FieldValue	*addNode(Node *node) const;
    virtual FieldValue	*removeNode(Node *node) const;

    virtual void		clamp(float /* min */, float /* max */) {}

    void				ref() { _refs++; }
    void				unref() { if (--_refs == 0) delete this;}

    virtual FieldValue	*copy() = 0;

private:
    int			_refs;
};

/**Parent class of MFString, MFFloat etc. containing information about the Multiple field values.
\ingroup VRMLParser*/
class MFieldValue : public FieldValue
{
public:
    virtual int				getSFSize() const = 0;
    virtual FieldValue		*getSFValue(int index) const = 0;
    virtual void			setSFValue(int index, FieldValue *value) = 0;
};

/**Calculates the indentation in the output VRML97 file. Specified extern in FieldValue.cpp so that all classes can access it. */
extern void indentf(FILE *f, int indent);

#endif // _FIELDVALUE_H
