#ifndef _SFROTATION_H
#define _SFROTATION_H

#ifndef _FIELDVALUE_H
#include "FieldValue.h"
#endif
#ifndef _VEC3F_H
#include "Vec3f.h"
#endif
#ifndef _MATRIX_H
#include "Matrix.h"
#endif
#ifndef _QUATERNION_H
#include "Quaternion.h"
#endif

/**
* SFRotation field type.
* \ingroup VRMLParser
*/
class SFRotation : public FieldValue {
public:
			SFRotation(float x, float y, float z, float w);
			SFRotation(const float *value);
			SFRotation(Vec3f euler, int mode);
			SFRotation(const Quaternion &q);
			SFRotation(const Matrix &mat);

    virtual int         getType() const { return SFROTATION; }
    virtual const char *getTypeName() const { return "SFRotation"; }
    virtual void        write(FILE *f, int indent) const;
    virtual bool	equals(const FieldValue *value) const;
    virtual FieldValue *copy() { return new SFRotation(*this); }

    const float	       *getValue() const { return _value; }
    Vec3f		getEulerAngles(int order);
    void		setEulerAngles(Vec3f angles, int order);
    const Quaternion   &getQuat() const;

    Vec3f		operator *(const Vec3f &v) const;

private:
    Quaternion		_quaternion;
    float		_value[4];
    Vec3f		_euler;
    int			_eulerOrder;
    bool		_eulerDirty;
};

#endif // _SFROTATION_H
