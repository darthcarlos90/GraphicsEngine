#include "Matrix3.h"

Matrix3::Matrix3(void)	{
	ToIdentity();
}

Matrix3::Matrix3( float elements[9] )	{
	memcpy(this->values,elements,9*sizeof(float));
}

Matrix3::~Matrix3(void)	{
	ToIdentity();
}

void Matrix3::ToIdentity() {
	ToZero();
	values[0]  = 1.0f;
	values[4]  = 1.0f;
	values[8] = 1.0f;
}

void Matrix3::ToZero()	{
	for(int i = 0; i < 9; i++)	{
		values[i] = 0.0f;
	}
}


void Matrix3::SetBalanceInertiaMatrix(float xx, float yy, float zz){
	values[0] = xx;
	values[4] = yy;
	values[8] = zz;
}

Matrix3 Matrix3::Inverse(){
	Matrix3 m;
	m.SetBalanceInertiaMatrix( (1/values[0]), (1/values[4]), (1/values[8]));
	return m;
}

Vector3 Matrix3::Transform(const Matrix3& a, const Vector3& b){
	return Vector3((a.values[0] * b.x) + (a.values[1] * b.y) + (a.values[2] * b.z),
		(a.values[3] * b.x) + (a.values[4] * b.y) + (a.values[5] * b.z),
		(a.values[6] * b.x) + (a.values[7] * b.y) + (a.values[8] * b.z));
}

