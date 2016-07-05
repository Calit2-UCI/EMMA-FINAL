#include "stdafx.h"
#include <stdio.h>
#include "Matrix.h"
#include <assert.h>
#include <math.h>

#define DET2X2(m11, m12, m21, m22) (m11 * m22 - m12 * m21)

#define DET3X3(m11, m12, m13, m21, m22, m23, m31, m32, m33) (m11 * DET2X2(m22, m23, m32, m33) - m21 * DET2X2(m12, m13, m32, m33) + m31 * DET2X2(m12, m13, m22, m23))

Matrix::Matrix()
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			_mat[i][j] = 0;
}

Matrix Matrix::operator*(const Matrix &m)
{
	Matrix matrix;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
				matrix._mat[i][j] += _mat[i][k] * _mat[k][j];

	return matrix;
}

Vec3f Matrix::operator*(const Vec3f &v)
{
	Vec3f vec;

	float vec1[4] = {0, 0, 0, 0};
	float vec2[4] = {v.x, v.y, v.z, 1};

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			vec1[i] += _mat[i][j] * vec2[j];

	vec.x = vec1[0];
	vec.y = vec1[1];
	vec.z = vec1[2];

	return vec;
}

void Matrix::getValue(float mat[4][4]) const
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			mat[i][j] = _mat[i][j];
}

Matrix Matrix::invert() const
{
	Matrix matrix;
	float det;

	// calculates the determinant of a 4x4 matrix
	det = _mat[0][0] * DET3X3(_mat[1][1], _mat[2][1], _mat[3][1],
								_mat[1][2], _mat[2][2], _mat[3][2], 
								_mat[1][3], _mat[2][3], _mat[3][3])
			- _mat[0][1] * DET3X3(_mat[1][0], _mat[2][0], _mat[3][0],
								_mat[1][2], _mat[2][2], _mat[3][2],
								_mat[1][3], _mat[1][3], _mat[3][3])
			+ _mat[0][2] * DET3X3(_mat[1][0], _mat[2][0], _mat[3][0],
								_mat[1][1], _mat[2][1], _mat[3][1],
								_mat[1][3], _mat[1][3], _mat[3][3])
			- _mat[0][3] * DET3X3(_mat[1][0], _mat[2][0], _mat[3][0],
									_mat[1][1], _mat[2][1], _mat[3][1],
									_mat[1][2], _mat[2][2], _mat[3][2]);

	if ( fabs(det) < EPSILON) {
        printf("Non-singular matrix, no inverse!\n");
        return matrix;
    }

    //row column labeling reversed since rows & columns are being transponsed
    matrix._mat[0][0] =   DET3X3(_mat[1][1], _mat[2][1], _mat[3][1],
									_mat[1][2], _mat[2][2], _mat[3][2],
									_mat[1][3], _mat[2][3], _mat[3][3]);
    matrix._mat[1][0] = - DET3X3(_mat[1][0], _mat[2][0], _mat[3][0],
									_mat[1][2], _mat[2][2], _mat[3][2],
									_mat[1][3], _mat[2][3], _mat[3][3]);
    matrix._mat[2][0] =   DET3X3(_mat[1][0], _mat[2][0], _mat[3][0],
									_mat[1][1], _mat[2][1], _mat[3][1],
									_mat[1][3], _mat[2][3], _mat[3][3]);
    matrix._mat[3][0] = - DET3X3(_mat[1][0], _mat[2][0], _mat[3][0],
									_mat[1][1], _mat[2][1], _mat[3][1],
									_mat[1][2], _mat[2][2], _mat[3][2]);
        
    matrix._mat[0][1] = - DET3X3(_mat[0][1], _mat[2][1], _mat[3][1],
									_mat[0][2], _mat[2][2], _mat[3][2],
									_mat[0][3], _mat[2][3], _mat[3][3]);
    matrix._mat[1][1] =   DET3X3(_mat[0][0], _mat[2][0], _mat[3][0],
									_mat[0][2], _mat[2][2], _mat[3][2],
									_mat[0][3], _mat[2][3], _mat[3][3]);
    matrix._mat[2][1] = - DET3X3(_mat[0][0], _mat[2][0], _mat[3][0],
									_mat[0][1], _mat[2][1], _mat[3][1],
									_mat[0][3], _mat[2][3], _mat[3][3]);
    matrix._mat[3][1] =   DET3X3(_mat[0][0], _mat[2][0], _mat[3][0],
									_mat[0][1], _mat[2][1], _mat[3][1],
									_mat[0][2], _mat[2][2], _mat[3][2]);
        
    matrix._mat[0][2] =   DET3X3(_mat[0][1], _mat[1][1], _mat[3][1],
									_mat[0][2], _mat[1][2], _mat[3][2],
									_mat[0][3], _mat[1][3], _mat[3][3]);
    matrix._mat[1][2] = - DET3X3(_mat[0][0], _mat[1][0], _mat[3][0],
									_mat[0][2], _mat[1][2], _mat[3][2],
									_mat[0][3], _mat[1][3], _mat[3][3]);
    matrix._mat[2][2] =   DET3X3(_mat[0][0], _mat[1][0], _mat[3][0],
									_mat[0][1], _mat[1][1], _mat[3][1],
									_mat[0][3], _mat[1][3], _mat[3][3]);
    matrix._mat[3][2] = - DET3X3(_mat[0][0], _mat[1][0], _mat[3][0],
									_mat[0][1], _mat[1][1], _mat[3][1],
									_mat[0][2], _mat[1][2], _mat[3][2]);
        
    matrix._mat[0][3] = - DET3X3(_mat[0][1], _mat[1][1], _mat[2][1],
									_mat[0][2], _mat[1][2], _mat[2][2],
									_mat[0][3], _mat[1][3], _mat[2][3]);
    matrix._mat[1][3] =   DET3X3(_mat[0][0], _mat[1][0], _mat[2][0],
									_mat[0][2], _mat[1][2], _mat[2][2],
									_mat[0][3], _mat[1][3], _mat[2][3]);
    matrix._mat[2][3] = - DET3X3(_mat[0][0], _mat[1][0], _mat[2][0],
									_mat[0][1], _mat[1][1], _mat[2][1],
									_mat[0][3], _mat[1][3], _mat[2][3]);
    matrix._mat[3][3] =   DET3X3(_mat[0][0], _mat[1][0], _mat[2][0],
									_mat[0][1], _mat[1][1], _mat[2][1],
									_mat[0][2], _mat[1][2], _mat[2][2]);

	// scales the adjoint matrix to get the inverse
    for (int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			matrix._mat[i][j] = matrix._mat[i][j] / det;

	return matrix;
}

Matrix Matrix::identity()
{
	Matrix matrix;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (i == j)	matrix._mat[i][j] = 1;
			else matrix._mat[i][j] = 0;

	return matrix;
}

void Matrix::swapRows(int i, int j)
{
	float element;

	assert((i >= 0) && (i < 4));
	assert((j >= 0) && (j < 4));
	if (i != j)
	{
		for (int k = 0; k < 4; k++)
		{
			element = _mat[i][k];
			_mat[i][k] = _mat[j][k];
			_mat[j][k] = element;
		}
	}
}
