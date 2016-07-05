#ifndef _ELEMENT_H
#define _ELEMENT_H

enum {
    EL_UNKNOWN,

    EL_EVENT_IN,
    EL_EVENT_OUT,
    EL_EXPOSED_FIELD,
    EL_FIELD_DEF
};
/**(Pure Virtual) The base class of Field, EventIn, EventOut and ExposedField classes. Contains an enumeration
of the element types: EL_UNKNOWN, EL_EVENT_IN, EL_EVENT_OUT, L_EXPOSED_FIELD, EL_FIELD_DEF that
correspond to the VRML97 element types.*/

class Element {
public:
    virtual	       ~Element() {}
    virtual int		getElementType() const = 0;
};

#endif // _ELEMENT_H
