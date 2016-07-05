#ifndef _EVENTOUT_H
#define _EVENTOUT_H

#ifndef _ELEMENT_H
#include "Element.h"
#endif
#ifndef KS_STRING_H
#include "ksString.h"
#endif

class ExposedField;

/**Responsible for handling VRML97 eventIn fields.*/
class EventOut : public Element {
public:
					EventOut(int type, const String &name,
								ExposedField *exposedField = NULL);
    virtual			~EventOut();
    virtual int	    getElementType() const { return EL_EVENT_OUT; }
    virtual void    write(FILE *f, int indent) const;

    int				getType() const { return _type; }
    const String	&getName() const { return _name; }
    ExposedField	*getExposedField() const { return _exposedField; }

private:
    int				_type;
    String			_name;
    ExposedField	*_exposedField;
};

#endif // _EVENTOUT_H
