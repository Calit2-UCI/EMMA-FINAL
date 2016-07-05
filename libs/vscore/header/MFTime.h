

#ifndef _MFTIME_H
#define _MFTIME_H

#ifndef _FIELDVALUE_H
#include "FieldValue.h"
#endif
#ifndef _ARRAY_H
#include "Array.h"
#endif
#ifndef _SFTIME_H
#include "SFTime.h"
#endif

/**
* MFTime field type.
* \ingroup VRMLParser
*/
class MFTime : public MFieldValue {
public:
			MFTime();
			MFTime(const MFTime &value);
			MFTime(const double values[], int len);
			MFTime(const int values[], int len);

    virtual int		getType() const		    { return MFTIME; }
    virtual const char *getTypeName() const	    { return "MFTime"; }
    virtual int		getSFSize() const	    { return _value.size(); }
    virtual FieldValue *getSFValue(int index) const   { return new SFTime(_value[index]); }
    virtual void	setSFValue(int index, FieldValue *value);
    void		write(FILE *f, int indent) const;
    void		clamp(float min, float max);
    virtual FieldValue *copy() { return new MFTime(*this); }

    virtual bool	equals(const FieldValue *value) const;
    bool		equals(const MFTime *value) const;
private:
    Array<double>	_value;
};

#endif // _MFTIME_H
