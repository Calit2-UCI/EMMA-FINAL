#ifndef _MFFLOAT_H
#define _MFFLOAT_H

#ifndef _ARRAY_H
#include "Array.h"
#endif
#ifndef _FIELD_VALUE_H
#include "FieldValue.h"
#endif
#ifndef _SFFLOAT_H
#include "SFFloat.h"
#endif

/**
* MFFloat feld type.
* \ingroup VRMLParser
*/
class MFFloat : public MFieldValue {
public:
			MFFloat(int stride = 1);
			MFFloat(const float values[], int len, int stride = 1);
			MFFloat(const MFFloat &value);

    virtual int         getType() const { return MFFLOAT; }
    virtual int         getStride() const { return 1; }
    virtual const char *getTypeName() const { return "MFFloat"; }
    virtual void	write(FILE *f, int indent) const;
    virtual bool	equals(const FieldValue *value) const;
    virtual void	clamp(float min, float max);
    virtual FieldValue *copy() { return new MFFloat(*this); }

    virtual int		getSFSize() const { return _value.size() / getStride(); }
    virtual FieldValue *getSFValue(int index) const { return new SFFloat(_value.get(index)); }
    virtual void	setSFValue(int index, FieldValue *value);

    const float	       *getValue() const { return _value.getData(); }
    float		getValue(int i) const { return _value.get(i); }
    int			getSize() const { return _value.size(); }
    void		setValue(int index, float value) { _value[index] = value; }

protected:
    bool		equals(const MFFloat *value) const;

protected:
    Array<float>	_value;
};

#endif // _MFFLOAT_H
