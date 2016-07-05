#ifndef _FIELD_H
#define _FIELD_H


#ifndef FLT_MAX
#include <float.h>
#endif


#include "Element.h"
#include "Node.h"

class FieldValue;
class ExposedField;

enum {
    FF_HIDDEN = 1 << 0, FF_FIXEDSTRINGS = 1 << 1, FF_URL = 1 << 2,
};

/**Generic class that provides the interface to initialise the Node Fields, that correspond to VRML97 node fields.
\ingroup VRMLParser*/
class Field : public Element {
public:
	/**Default Field constructor. 
	@param type the field type, e.g. "SFVEC3F"
	@param name the name of the field, e.g. "bboxCenter"
	@param value, the value of the field, e.g. "new SFVec3f(0.0f, 0.0f, 0.0f)" 
	@param exposedField the default value is NULL
	@param min the default value is -FLT_MAX
	@param max the default value is FLT_MAX
	@param nodeType the default value is NODE_ANY (it can also be NODE_CHILD, NODE_GEOMETRY, NODE_TEXTURE, NODE_INTERPOLATOR)
	@param flags the default value is 0
	@param **strings the default value is NULL*/	
						Field(int type, const String & name, FieldValue *value, 
								ExposedField *exposedField = NULL, float min = -FLT_MAX,
								float max = FLT_MAX, int nodeType = NODE_ANY, int flags = 0, 
								const char **strings = NULL);

    virtual				~Field();

	/**Returns the EL_FIELD_DEF as specified in parent class Element.*/
    virtual int			getElementType() const { return EL_FIELD_DEF; }

    virtual void		write(FILE *f, int indent) const;

	/**Returns the name of the field, e.g. "bboxCenter"*/
    const String &getName() const { return _name; }

	/**Returns the type of the field, e.g. "SFVEC3F"*/
    int					getType() const { return _type; }

	/**Returns the default value of the field as it was specified in the Field constructor.*/
    FieldValue			*getDefault() const { return _value; }

	/**Returns the ExposedField of the field as it was specified in the Field constructor.*/
    ExposedField		*getExposedField() const { return _exposedField; }

	/**Returns the Min value of the field as it was specified in the Field constructor.*/
    float				getMin() const { return _min; }

	/**Returns the Max value of the field as it was specified in the Field constructor.*/
    float				getMax() const { return _max; }

	/**Returns the NodeType of the field as it was specified in the Field constructor.*/
    int					getNodeType() const { return _nodeType; }

	/**Returns the Flags of the field as it was specified in the Field constructor.*/
    int					getFlags() const { return _flags; }

	/**Returns the Strings of the field as it was specified in the Field constructor.*/
    const char			**getStrings() const { return _strings; }

private:
    String _name;
    int _type;
    FieldValue *_value;
    ExposedField *_exposedField;
    float _min;
    float _max;
    int _flags;
    int _nodeType;
    const char **_strings;
};

#endif  // _FIELD_H
