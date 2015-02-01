#pragma once

#include <iostream>
#include "common.h"
#include "Vector3.h"
#include "Vector4.h"

class Vector3;

class Matrix3	{
public:
	Matrix3(void);
	Matrix3(float elements[9]);
	~Matrix3(void);

	float	values[9];

	//Set all matrix values to zero
	void	ToZero();
	//Sets matrix to identity matrix (1.0 down the diagonal)
	void	ToIdentity();

	void SetBalanceInertiaMatrix (float xx, float yy, float zz);
	//An inverse for the main diagonal
	Matrix3 Inverse();

	//Multiplies 'this' matrix by matrix 'a'. Performs the multiplication in 'OpenGL' order (ie, backwards)
	inline Matrix3 operator*(const Matrix3 &a) const{	
		Matrix3 out;
		//Students! You should be able to think up a really easy way of speeding this up...
		for(unsigned int r = 0; r < 3; ++r) {
			for(unsigned int c = 0; c < 3; ++c) {
				out.values[c + (r*4)] = 0.0f;
				for(unsigned int i = 0; i < 3; ++i) {
					out.values[c + (r*3)] += this->values[c+(i*3)] * a.values[(r*3)+i];
				}
			}
		}
		return out;
	}

	static Vector3 Transform(const Matrix3& a, const Vector3& b);

	inline Vector3 operator*(const Vector3 &v) const {
		Vector3 vec;

		//float temp;

		vec.x = v.x*values[0] + v.y*values[1] + v.z*values[2];
		vec.y = v.x*values[3] + v.y*values[4] + v.z*values[5];
		vec.z = v.x*values[6] + v.y*values[7] + v.z*values[8];

		return vec;
	};
};

