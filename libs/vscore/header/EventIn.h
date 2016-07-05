#ifndef _EVENT_IN_H
#define _EVENT_IN_H

#ifndef _ELEMENT_H
#include "Element.h"
#endif

#ifndef KS_STRING_H
#include "ksString.h"
#endif

class ExposedField;

/**Responsible for handling VRML97 eventIn fields.*/
class EventIn : public Element {
public:
						EventIn(int type, String name, ExposedField *exposedField = NULL);
    virtual				~EventIn();

    virtual int			getElementType() const { return EL_EVENT_IN; }
    virtual void		write(FILE *f, int indent) const;

    int					getType() const { return _type; }

    const String		&getName() const { return _name; }

    ExposedField		*getExposedField() const { return _exposedField; }

private:
    int _type;
    String _name;
    ExposedField *_exposedField;
};

#endif  // _EVENT_IN_H
