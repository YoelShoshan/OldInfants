#ifndef YOEL_QUATERNIONS_H
#define YOEL_QUATERNIONS_H

#pragma once

#include "Math3d.h"

class CQuaternion 
{

public:

	// init the quaternion to unit quaternion
	CQuaternion() { x = y = z = 0.f;   w = 1.f;  }
	
	CQuaternion(float X, float Y, float Z, float W) 
	{ 
		x = X;		y = Y;		z = Z;		w = W;
	}

	// takes in an array of 16 floats to fill in a 4x4 homogeneous matrix from a quaternion
	void CreateOpenGLMatrix(float *pMatrix);

	void CreateMatrix(CMatrix &m);

	// takes a 3x3 or 4x4 matrix and converts it to a quaternion, depending on rowColumnCount
	void CreateFromMatrix(float *pMatrix, int rowColumnCount);

	// returns a spherical linear interpolated quaternion between q1 and q2, according to t
	CQuaternion Slerp(CQuaternion &q1, CQuaternion &q2, float t);

private:

	// This stores the 4D values for the quaternion
	float x, y, z, w;
};





#endif