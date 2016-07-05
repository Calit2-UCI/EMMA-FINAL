#ifndef _EXPOSED_FIELD_H
#define _EXPOSED_FIELD_H


#ifndef FLT_MAX
#include <float.h>
#endif


#include "Element.h"
#include "Node.h"

class FieldValue;

/**Responsible for specifically handling the VRML97 exposedFields. The class operation resembles Field class.
\ingroup VRMLParser*/
class ExposedField : public Element {
public:
						ExposedField(int type, const String &name,
								FieldValue *value, float min = -FLT_MAX,
								float max = FLT_MAX, int nodeType = NODE_ANY,
								int flags = 0, const char **strings = NULL);

    virtual				~ExposedField();


    virtual int			getElementType() const { return EL_EXPOSED_FIELD; }


    int					getType() const { return _type; }


    const String		&getName() const { return _name; }


    FieldValue			*getValue() const { return _value; }


    float				getMin() const { return _min; }


    float				getMax() const { return _max; }


    int					getNodeType() const { return _nodeType; }


    int					getFlags() const { return _flags; }


    const char			**getStrings() const { return _strings; }


    int					getField() const { return _field; }


    int					getEventIn() const { return _eventIn; }


    int					getEventOut() const { return _eventOut; }


    void				setField(int field) { _field = field; }


    void				setEventIn(int eventIn) { _eventIn = eventIn; }


    void				setEventOut(int eventOut) { _eventOut = eventOut; }


    virtual void		write(FILE *f, int indent) const;

private:
    int				_type;
    String			_name;
    FieldValue		*_value;
    int				_field;
    int				_eventIn;
    int				_eventOut;
    float			_min;
    float			_max;
    int				_nodeType;
    int				_flags;
    const char		**_strings;
};

#endif // _EXPOSED_FIELD_H
