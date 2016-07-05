#ifndef __RBF_MORPHER__
#define __RBF_MORPHER__

#include <vector>
#include <math.h>

#include <vcg/math/matrix.h>
#include <vcg/math/lin_algebra.h>
#include <vcg/space/point3.h>

using namespace vcg;


namespace VisageSDK
{

// Morph a source mesh (ms) into a target mesh (mt) using Hardy's method
// prerequisite: a set of landmarks must be associated with the source and the taget
// For further details, see "Fratarcangeli, Schaerf and Forchheimer, Facial Motion Cloning with Radial 
// Basis Functions in MPEG-4 FBA, Graphical Models, 69 (2007) 106–118, ISSN: 1524-0703, Academic Press

// TODO: MESH_TYPE is not required, the morphing requires only the vert vector...
// by using only the vert vector the class become more generalized

template<class MESH_TYPE, class CONT_TYPE>
class RBF_Morpher
{
public:

	typedef typename MESH_TYPE::VertexIterator VertexIterator;
	typedef typename MESH_TYPE::ScalarType	ScalarType;
	typedef typename MESH_TYPE::CoordType	CoordType;
	typedef typename ndim::Matrix<ScalarType>		Matrixx;

	RBF_Morpher(){}
	//RBF_Morpher(MESH_TYPE & ms, CONT_TYPE & ls, MESH_TYPE & mt, CONT_TYPE & lt);

	//void Init(MESH_TYPE & ms, CONT_TYPE & ls, MESH_TYPE & mt, CONT_TYPE & lt);


	// Morph the source into the target.
	// The morphing function is found by solving a linear system of equations.
	// The morphing is carried out in four different steps:
	//		- 1. assemble the matrix A 
	//		- 2. assemble the known vector b with elements of lt
	//		- 3. solve Ax = b with Singular Value Decomposition and find x, that is, the Hardy coefficients
	//		- 4. use x = [h1, ..., h_n+4] to morph the vertexes of the source and the correspondent landmarks (that is, ls)

	// ms is the source mesh
	// ls is a container with source landmarks stored
	// mt is the target mesh
	// lt is a container with target landmarks stored
	// m_r is the stiffness radius; if -1, stiffness radius is set for smooth results
	void Morph(MESH_TYPE & ms, CONT_TYPE & ls, MESH_TYPE & mt, CONT_TYPE & lt, float m_r = -1)
	{
		assert (ls.size() == lt.size());

		// step 1.
		int nbLands = (int) ls.size();
		int nbCoeff = nbLands + 4;	// number of Hardy coefficients to be found

		// initialize stiffness radius for smooth results
		// for each i, r[i] is the minimum distance from landmarks i != j
		vector<ScalarType> r_squared;	// square of stiffness radius for each rbf
		for (int i = 0; i < nbLands; i++)
		{
			ScalarType _r = 10^6;
			for (int j = 0; j < nbLands; j++)
			{
				if (i == j) continue;

				ScalarType dist = (ls[i] - ls[j]).SquaredNorm();
				if (dist < _r) _r = dist; 
			}

			r_squared.push_back(_r);
		}

		if (m_r != -1)		// impose external stiffness radius
			for (int i = 0; i < nbLands; i++)
				r_squared[i] = (ScalarType)m_r;

		Matrixx A(nbCoeff, nbCoeff);	// initialize matrix of coefficients
		A.SetZero();

		// fill the matrix A
		for (int i = 0; i < nbLands; i++)			// rows
			for (int j = 0; j < nbLands; j++)		// columns
			{
				if (i == j)
				{
					A[i][j] = 0;
					continue;
				}

				A[i][j] = sqrt( (ls[i] - ls[j]).SquaredNorm() + r_squared[j] );
			}

		for (int i = nbLands; i < nbCoeff - 1; i++)			// rows
			for (int j = 0; j < nbLands; j++)							// columns
				A[i][j] = A[j][i] = ls[j][i - nbLands];

		for (int j = 0; j < nbLands; j++)							// columns
			A[nbCoeff - 1][j] = A[j][nbCoeff - 1] = 1;

		// h is a vector of 3D coefficients; it stores the solution of the linear system
		CONT_TYPE h;
		CoordType dummy; dummy.Zero();
		for (int i = 0; i < nbCoeff; i++)	// initially h is filled with (0, 0, 0)s
			h.push_back(dummy);

		// V and w are used for the singular value decomposition
		Matrixx V(nbCoeff, nbCoeff);
		ScalarType * w = new ScalarType[nbCoeff];
		V.SetZero();

		// perform singular value decomposition
		SingularValueDecomposition<Matrixx>(A, w, V);		// There is side.effect on A here 
															// from now on, A should be considered as U in standard SVD notation
		// construct diag(w);
		Matrixx W(nbCoeff, nbCoeff);
		W.SetZero();
		for (int i = 0; i < nbCoeff; i++)
			if (w[i] > 0.00001)
				W[i][i] = 1.0 / w[i];
			else
			{
				printf("w[%d] is zero and zeroed => A is singular!\n", i);
				W[i][i] = 0.0;
			}

		A.Transpose();

		Matrixx A_inv(nbCoeff, nbCoeff);

		A_inv = V * (W * A);

		ScalarType * b = new ScalarType[nbCoeff];
		ScalarType * x;

		for (int i = 0; i < nbLands; i++)
			b[i] = lt[i].X();
		for (int i = nbLands; i < nbCoeff; i++)
			b[i] = 0;
		x = A_inv * b;

		for (int i = 0; i < nbCoeff; i++)
			h[i][0] = x[i];
		delete [] x;

		for (int i = 0; i < nbLands; i++)
			b[i] = lt[i].Y();
		for (int i = nbLands; i < nbCoeff; i++)
			b[i] = 0;
		x = A_inv * b;

		for (int i = 0; i < nbCoeff; i++)
			h[i][1] = x[i];
		delete [] x;

		for (int i = 0; i < nbLands; i++)
			b[i] = lt[i].Z();
		for (int i = nbLands; i < nbCoeff; i++)
			b[i] = 0;
		x = A_inv * b;

		for (int i = 0; i < nbCoeff; i++)
			h[i][2] = x[i];
		delete [] x;

		// Morph!!

		// morph the mesh
		for (VertexIterator vi = ms.vert.begin(); vi != ms.vert.end(); vi++)
		{
			CoordType p = vi->P();

			CoordType p_morphed;
			p_morphed.Zero();

			for (int i = 0; i < nbLands; i++)
			{
				ScalarType R_i = sqrt((p - ls[i]).SquaredNorm() + r_squared[i]);
				p_morphed += h[i] * R_i;
			}

			p_morphed += h[nbLands] * p.X() + h[nbLands + 1] * p.Y() + h[nbLands + 2] * p.Z() + h[nbLands + 3];

			vi->P() = p_morphed;
		}

		// morph the landmarks ls
		// copy the landmarks in a temporary vector
		CONT_TYPE temp;
		temp.assign(ls.begin(), ls.end());
		for (CONT_TYPE::iterator vi = temp.begin(); vi != temp.end(); vi++)
		{
			CoordType p = *vi;

			CoordType p_morphed;
			p_morphed.Zero();

			for (int i = 0; i < nbLands; i++)
			{
				ScalarType R_i = sqrt((p - ls[i]).SquaredNorm() + r_squared[i]);
				p_morphed += h[i] * R_i;
			}

			p_morphed += h[nbLands] * p.X() + h[nbLands + 1] * p.Y() + h[nbLands + 2] * p.Z() + h[nbLands + 3];

			*vi = p_morphed;
		}
		// dump the temp vector back to ls
		ls.assign(temp.begin(), temp.end());

		delete [] w;
		delete [] b;
	}

	~RBF_Morpher(){}

private:

	vector<ScalarType> r;	// stiffness radius for each rbf
	int nbCoeff;					// number of Hardy coefficients to be found
};

}

#endif