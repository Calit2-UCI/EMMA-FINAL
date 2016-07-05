#ifndef _MATRIX_H
#define _MATRIX_H

#ifndef _VEC3F_H
#include "Vec3f.h"
#endif

/**
* 4x4 matrix.
* \ingroup VRMLParser
*/
class Matrix {
public:
		    Matrix();

    Matrix	    operator*(const Matrix &m);
    Vec3f	    operator*(const Vec3f &v);

    operator	    float *() { return (float *) _mat; }
    operator	    const float *() const { return (const float *) _mat; }
    void	    getValue(float mat[4][4]) const;

    Matrix	    invert() const;

    static Matrix   identity();
    void	    swapRows(int i, int j);

private:
    float	    _mat[4][4];
};

#endif // _MATRIX_H
