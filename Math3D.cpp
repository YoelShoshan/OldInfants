
#include <math.h>
#include "GMath.h"
#include "Math3d.h"

///////////////////////////
#include "ZedTracker.h"
///////////////////////////

/////////////////////////////////////////////
// class CColor4
// *****************
// contains a 4 byte (unsigned char) array
//////////////////////////////////////////////

///////////////////////////
// Constructer
///////////////////////////
CColor4::CColor4()
{
	c[0]=c[1]=c[2]=c[3]=0.f;
}


////////////////////////////////
// Constructer, accepts 4 bytes
////////////////////////////////
CColor4::CColor4(const byte r,const byte g,const byte b,const byte a)
{
	c[0] = r;
	c[1] = g;
	c[2] = b;
	c[3] = a;
	
}

/////////////////////////
// Set()
// simply set the values
/////////////////////////
void CColor4::Set(const byte r,const byte g,const byte b,const byte a)
{
	c[0] = r;
	c[1] = g;
	c[2] = b;
	c[3] = a;
}

//////////////////////////////////////////////////
// Set() -- faster
// accepts a reference to a 4 byte array
///////////////////////////////////////////////////
void CColor4::Set(const CColor4& color)
{
// optimize this - memcpy or sth
	c[0] = color.c[0];
	c[1] = color.c[1];
	c[2] = color.c[2];
	c[3] = color.c[3];
}















///////////////////////////////////////////////////
// *********class CVector3f***********/////////////
///////////////////////////////////////////////////

////////////////////////////////////////////
// Basic Constructer
// notice: v[0,1,2] aren't inited, for speed
////////////////////////////////////////////
CVector3f::CVector3f()
   {
	  // commenting this would be not safe but faster  
      //v[0] = v[1] = v[2] = 0;
   };


//////////////////////////////////
// Constructer with set values
//////////////////////////////////
CVector3f::CVector3f(float x, float y, float z)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
}


//////////////////////////////////
// Constructer with set values
//////////////////////////////////
CVector3f::CVector3f(const CVector3f& vSet)
{
	v[0] = vSet.v[0];
	v[1] = vSet.v[1];
	v[2] = vSet.v[2];
}


/////////////////////////////////////////////////
// ***********methods to alter data*************
/////////////////////////////////////////////////

//TODO: operator = on other things (like float*, so I can memcpy it)

//////////////////////
// operator: "="
///////////////////////
CVector3f CVector3f::operator=(const CVector3f &pVec)
{
	return CVector3f(v[0] = pVec.v[0],v[1] = pVec.v[1],v[2] = pVec.v[2]);
}

/////////////////////
// Set() 
/////////////////////
void CVector3f::Set(float x, float y, float z)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
}

///////////////////////
// Set()
///////////////////////
void CVector3f::Set(CVector3f &Vec3f)
{
	v[0] = Vec3f.v[0];
	v[1] = Vec3f.v[1];
	v[2] = Vec3f.v[2];
}

//////////////////////////////////////////////////
// Set() - FASTER
// (pfSet must point to at least 3 valid floats)
//////////////////////////////////////////////////
void CVector3f::Set(float* pfSet)
{

	///fixme: an obvious memcpy should be here ;)

	v[0] = pfSet[0];
	v[1] = pfSet[1];
	v[2] = pfSet[2];
}



///////////////////////////////////////////////////////////////////////////////////////////
///////*************Operators that return a result and don't change inner data*************
/////// FIXME: make the functions themselves const +++++++ the parameters
///////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////
// *******SCALAR parameter*********
////////////////////////////////////////


////////////////////
// CVector + float
////////////////////
CVector3f CVector3f::operator+(float fNum) 
{
 // Return the scaled vector
   return CVector3f(v[0] + fNum, v[1] + fNum, v[2] + fNum);
}


////////////////////
// CVector - float
////////////////////
CVector3f CVector3f::operator-(float fNum) 
{
 // Return the scaled vector
   return CVector3f(v[0] - fNum, v[1] - fNum, v[2] - fNum);
}

////////////////////
// CVector * float
////////////////////
CVector3f CVector3f::operator*(float fNum) 
{
 // Return the scaled vector
   return CVector3f(v[0] * fNum, v[1] * fNum, v[2] * fNum);
}


////////////////////
// CVector / float
////////////////////
CVector3f CVector3f::operator/(float fNum) 
{
 // Return the scaled vector
   return CVector3f(v[0] / fNum, v[1] / fNum, v[2] / fNum);
}

//////////////////////////////////////////////
// ***********Vector parameter***********
//////////////////////////////////////////////


/////////////////////////
// Vector + Vector
///////////////////////////
CVector3f CVector3f::operator+(const CVector3f& Vec1) const
{
	return CVector3f(v[0]+Vec1.v[0],v[1]+Vec1.v[1],v[2]+Vec1.v[2]);
}

/////////////////////////
// Vector - Vector
///////////////////////////
CVector3f CVector3f::operator-(const CVector3f& Vec1) const
{
	return CVector3f(v[0]-Vec1.v[0],v[1]-Vec1.v[1],v[2]-Vec1.v[2]);
}

/////////////////////////////
// Vector / Vector
/////////////////////////////
float CVector3f::operator*(const CVector3f& Vec1) const
{
	return v[0]*Vec1.v[0]+v[1]*Vec1.v[1]+v[2]*Vec1.v[2];
}


/*/////////////////////////////
// Vector / Vector
/////////////////////////////
float CVector3f::operator*(CVector3f& Vec1) const
{
	return v[0]*Vec1.v[0]+v[1]*Vec1.v[1]+v[2]*Vec1.v[2];
}*/


/////////////////////////
// Vector dot Vector
///////////////////////////
CVector3f CVector3f::operator/(const CVector3f& Vec1) const
{
return CVector3f(v[0]/Vec1.v[0],v[1]/Vec1.v[1],v[2]/Vec1.v[2]);
}

/////////////////////////
// Vector cross Vector
///////////////////////////
CVector3f CVector3f::operator^(const CVector3f& Vec1) const
{
	return CVector3f(v[1] * Vec1.v[2] - v[2] * Vec1.v[1],
		             v[2] * Vec1.v[0] - v[0] * Vec1.v[2],
				     v[0] * Vec1.v[1] - v[1] * Vec1.v[0]);
}

/////////////////////////////////////////////////////////////////
///////*************Operators that DO change inner data*************
/////////////////////////////////////////////////////////////////


////////////////////////////////////////////
// note: NO CHECK FOR DEVIDE BY ZERO!!!!!
////////////////////////////////////////////


////////////////////////////
//// a SCALAR parameter
////////////////////////////

/////////////////////////
// vector+=scalar
/////////////////////////
void CVector3f::operator+=(float fNum) 
{
 v[0]+= fNum;
 v[1]+= fNum;
 v[2]+= fNum;
}

/////////////////////////
// vector-=scalar
/////////////////////////
void CVector3f::operator-=(float fNum) 
{
 v[0]-= fNum;
 v[1]-= fNum;
 v[2]-= fNum;
}

/////////////////////////
// vector*=scalar
/////////////////////////
void CVector3f::operator*=(float fNum) 
{
 v[0]*= fNum;
 v[1]*= fNum;
 v[2]*= fNum;
}


/////////////////////////
// vector/=scalar
/////////////////////////
void CVector3f::operator/=(float fNum) 
{
 v[0]/= fNum;
 v[1]/= fNum;
 v[2]/= fNum;
}

///////////////////////////////////////////////////////////////////////////
//// a VECTOR parameter
//// TODO: maybe i should pass the parameter as a reference (also const)
///////////////////////////////////////////////////////////////////////////

/////////////////////////
// Vector+=Vector
/////////////////////////
void CVector3f::operator+=(CVector3f Vec1) 
{
 v[0]+= Vec1.v[0];
 v[1]+= Vec1.v[1];
 v[2]+= Vec1.v[2];
}

/////////////////////////
// Vector-=Vector
/////////////////////////
void CVector3f::operator-=(CVector3f Vec1) 
{
 v[0]-= Vec1.v[0];
 v[1]-= Vec1.v[1];
 v[2]-= Vec1.v[2];
}

/////////////////////////////////////
// Vector*=Vector
// (should work like a dot product)
////////////////////////////////////
void CVector3f::operator*=(CVector3f Vec1) 
{
 v[0]*= Vec1.v[0];
 v[1]*= Vec1.v[1];
 v[2]*= Vec1.v[2];
}

/////////////////////////
// Vector/=Vector
/////////////////////////
void CVector3f::operator/=(CVector3f Vec1) 
{
 v[0]/= Vec1.v[0];
 v[1]/= Vec1.v[1];
 v[2]/= Vec1.v[2];
}

///

/*void CVector3f::Cross(CVector3f& v1,CVector3f& v2)
{
	v[0] = v1.v[1] * v2.v[2] - v1.v[2] * v2.v[1];
	v[1] = v1.v[2] * v2.v[0] - v1.v[0] * v2.v[2];
	v[2] = v1.v[0] * v2.v[1] - v1.v[1] * v2.v[0];
}

void CVector3f::Add(CVector3f& v1,CVector3f& v2)
{
	v[0] = v1.v[0] + v2.v[0];
	v[1] = v1.v[1] + v2.v[1];
	v[2] = v1.v[2] + v2.v[2];
}

void CVector3f::Subtract(CVector3f& v1,CVector3f& v2)
{
	v[0] = v1.v[0] - v2.v[0];
	v[1] = v1.v[1] - v2.v[1];
	v[2] = v1.v[2] - v2.v[2];
}
   
void CVector3f::Multiply(CVector3f& v1,CVector3f& v2)
{
	v[0] = v1.v[0] * v2.v[0];
	v[1] = v1.v[1] * v2.v[1];
	v[2] = v1.v[2] * v2.v[2];
}
   
void CVector3f::Divide(CVector3f& v1,CVector3f& v2)
{
	// TODO: Should I check for divide by Zero here?
	v[0] = v1.v[0] / v2.v[0];
	v[1] = v1.v[1] / v2.v[1];
	v[2] = v1.v[2] / v2.v[2];
}
 
void CVector3f::Inverse(CVector3f& Vec1)
{
	v[0] = -( Vec1.v[0] );
	v[1] = -( Vec1.v[1] );
	v[2] = -( Vec1.v[2] );
}

void CVector3f::Normalize(CVector3f& Vec1)
{
	float t;
	t = (float) sqrt( Vec1.v[0]*Vec1.v[0]+Vec1.v[1]*Vec1.v[1]+Vec1.v[2]*Vec1.v[2]);
	
	// Replace this with Set(blah) or sth
	v[0] = Vec1.v[0];
	v[1] = Vec1.v[1];
	v[2] = Vec1.v[2];

	v[0] /= t;
	v[1] /= t;
	v[2] /= t;
}*/



///////////////////////////////////
// Operating on the Class itself
// returns a result, no parameters
///////////////////////////////////

///////////////////////////////////
// Length()
// get the length of the vector
//////////////////////////////////
float CVector3f::Length(void)
{
	//return (float)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
	return sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
	
}

//////////////////////////////////
// Inverse()
// *(-1) for each axis component
//////////////////////////////////
void CVector3f::Inverse(void)
{
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
}

////////////////////////////////////////////////////////////
// Normalize()
// normalize the vector (after normalization, length==1.f)
///////////////////////////////////////////////////////////

CVector3f CVector3f::Normalize(void)
{
	float t;
	t = (float) sqrt( v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
		
	v[0] /= t;
	v[1] /= t;
	v[2] /= t;

	return (*this);
}


float CVector3f::GetSize(void)
{
	return (float) sqrt( v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
}


/*void CVector3f::PutRayPoint(CVector3f& RayOrigin,CVector3f& RayDirection,float t)
{	
	v[0]=RayOrigin.v[0] + t * RayDirection.v[0];
	v[1]=RayOrigin.v[1] + t * RayDirection.v[1];
	v[2]=RayOrigin.v[2] + t * RayDirection.v[2];
}*/


















///////////////////////////////////////////////////
// *********class CVector4f***********/////////////
///////////////////////////////////////////////////

////////////////////////////////////////////
// Basic Constructer
// notice: v[0,1,2] aren't inited, for speed
////////////////////////////////////////////
CVector4f::CVector4f()
{
	// commenting this would be not safe but fast 
	//v[0] = v[1] = v[2] = v[3]=0;
};


//////////////////////////////////
// Constructer with set values
//////////////////////////////////
CVector4f::CVector4f(float x, float y, float z,float d)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
	v[3] = d;
}


//////////////////////////////////
// Constructer with set values
//////////////////////////////////
void CVector4f::Set(float x, float y, float z,float d)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
	v[3] = d;
}


//////////////////////////////////
// Constructer with set values
//////////////////////////////////
/*CVector4f::CVector4f(const CVector4f& vSet)
{
	v[0] = vSet.v[0];
	v[1] = vSet.v[1];
	v[2] = vSet.v[2];
	v[3] = vSet.v[3];
}*/



CVector4f CVector4f::Normalize(void)
{
	float t;
	t = (float) sqrt( v[0]*v[0]+v[1]*v[1]+v[2]*v[2]+v[3]*v[3]);

	v[0] /= t;
	v[1] /= t;
	v[2] /= t;
	v[3] /= t;

	return (*this);

}




















///////////////////////////////////////////////////
// *********class CVector2f***********/////////////
///////////////////////////////////////////////////


///////////////////////////////////////////////
// *********** Constructers ******************
///////////////////////////////////////////////


///////////////////////
//Constructer()
///////////////////////
CVector2f::CVector2f()
   {
      v[0] = v[1] = 0;
   }

////////////////////////////////////
//Constructer()
// constructs and inits using x,y
////////////////////////////////////
CVector2f::CVector2f(float x, float y)
{
	v[0] = x;
	v[1] = y;
}

///////////////////////////////
//Set() - accepts 2 floats
////////////////////////////////
void CVector2f::Set(float x, float y)
{
	v[0] = x;
	v[1] = y;
}

/////////////////////////////////////////
//Set() // accept a reference to a vector
/////////////////////////////////////////
void CVector2f::Set(CVector2f &Vec2f)
{
	v[0] = Vec2f.v[0];
	v[1] = Vec2f.v[1];
}

//////////////////////////////////////////////////////////
//Set() // accepts a float array (of at least 2 elements)
///////////////////////////////////////////////////////////
void CVector2f::Set(float* pfSet)
{
	v[0] = pfSet[0];
	v[1] = pfSet[1];
}



/////////////////////////////////////////////////////////////////
///////*************Operators that DO NOT change inner data*************
/////////////////////////////////////////////////////////////////


/////////////////////////////////
//// *****a SCALAR parameter****
/////////////////////////////////


////////////////////////////
//// vector2f + float
////////////////////////////
CVector2f CVector2f::operator+(float fNum) 
{
 // Return the scaled vector
   return CVector2f(v[0] + fNum, v[1] + fNum);
}

////////////////////////////
//// vector2f - float
////////////////////////////
CVector2f CVector2f::operator-(float fNum) 
{
 // Return the scaled vector
   return CVector2f(v[0] - fNum, v[1] - fNum);
}

////////////////////////////
//// vector2f * float
////////////////////////////
CVector2f CVector2f::operator*(float fNum) 
{
 // Return the scaled vector
   return CVector2f(v[0] * fNum, v[1] * fNum);
}

////////////////////////////
//// vector2f / float
////////////////////////////
CVector2f CVector2f::operator/(float fNum) 
{
 // Return the scaled vector
   return CVector2f(v[0] / fNum, v[1] / fNum);
}

/////////////////////////////////
//// *****a VECTOR parameter****
/////////////////////////////////

/////////////////////////////////
//// vector + vector
/////////////////////////////////
CVector2f CVector2f::operator+(CVector2f Vec1)
{
	return CVector2f(v[0]+Vec1.v[0],v[1]+Vec1.v[1]);
}

/////////////////////////////////
//// vector - vector
/////////////////////////////////
CVector2f CVector2f::operator-(CVector2f Vec1)
{
	return CVector2f(v[0]-Vec1.v[0],v[1]-Vec1.v[1]);
}

/////////////////////////////////
//// vector dot vector
/////////////////////////////////
float CVector2f::operator*(CVector2f Vec1)
{
	return v[0]*Vec1.v[0]+v[1]*Vec1.v[1];
}

/////////////////////////////////
//// vector / vector
/////////////////////////////////
CVector2f CVector2f::operator/(CVector2f Vec1)
{
	return CVector2f(v[0]/Vec1.v[0],v[1]/Vec1.v[1]);
}


/////////////////////////////////////////////////////////////////
///////*************Operators that CHANGE inner data*************
/////////////////////////////////////////////////////////////////


/////////////////////////////////
//// *****a SCALAR parameter****
/////////////////////////////////


/////////////////////////////////
//// vector+=scalar
/////////////////////////////////
void CVector2f::operator+=(float fNum) 
{
 v[0]+= fNum;
 v[1]+= fNum;
}

/////////////////////////////////
//// vector-=scalar
/////////////////////////////////
void CVector2f::operator-=(float fNum) 
{
 v[0]-= fNum;
 v[1]-= fNum;
}

/////////////////////////////////
//// vector*=scalar
/////////////////////////////////
void CVector2f::operator*=(float fNum) 
{
 v[0]*= fNum;
 v[1]*= fNum;
}

/////////////////////////////////
//// vector/=scalar
/////////////////////////////////
void CVector2f::operator/=(float fNum) 
{
 v[0]/= fNum;
 v[1]/= fNum;
}


/////////////////////////////////
//// *****a VECTOR parameter****
/////////////////////////////////


/////////////////////////////////
//// vector+=vector
/////////////////////////////////
void CVector2f::operator+=(CVector2f Vec1) 
{
 v[0]+= Vec1.v[0];
 v[1]+= Vec1.v[1];
}

/////////////////////////////////
//// vector-=vector
/////////////////////////////////
void CVector2f::operator-=(CVector2f Vec1) 
{
 v[0]-= Vec1.v[0];
 v[1]-= Vec1.v[1];
}

/////////////////////////////////
//// vector*=vector
/////////////////////////////////
void CVector2f::operator*=(CVector2f Vec1) 
{
 v[0]*= Vec1.v[0];
 v[1]*= Vec1.v[1];
}

/////////////////////////////////
//// vector/=vector
/////////////////////////////////
void CVector2f::operator/=(CVector2f Vec1) 
{
 v[0]/= Vec1.v[0];
 v[1]/= Vec1.v[1];
}


/*
void CVector2f::Add(CVector2f& v1,CVector2f& v2)
{
	v[0] = v1.v[0] + v2.v[0];
	v[1] = v1.v[1] + v2.v[1];
}

void CVector2f::Subtract(CVector2f& v1,CVector2f& v2)
{
	v[0] = v1.v[0] - v2.v[0];
	v[1] = v1.v[1] - v2.v[1];
}
   
void CVector2f::Multiply(CVector2f& v1,CVector2f& v2)
{
	v[0] = v1.v[0] * v2.v[0];
	v[1] = v1.v[1] * v2.v[1];
	v[2] = v1.v[2] * v2.v[2];
}
   
void CVector2f::Divide(CVector2f& v1,CVector2f& v2)
{
	// TODO: Should I check for divide by Zero here?
	v[0] = v1.v[0] / v2.v[0];
	v[1] = v1.v[1] / v2.v[1];
}
 


*/




////////////////////////////////////////////////////
// ***********Stand Alone Functions***************
////////////////////////////////////////////////////

////////////////////////////////////////////////////
// CVector3f dot CVector3f
////////////////////////////////////////////////////
float Dot(CVector3f v1,CVector3f v2)
{
	return v1.v[0] * v2.v[0] + v1.v[1] * v2.v[1] + v1.v[2] * v2.v[2];
}

////////////////////////////////////////////////////
// CVector2f dot CVector2f
////////////////////////////////////////////////////
float Dot(CVector2f v1,CVector2f v2)
{
	return v1.v[0] * v2.v[0] + v1.v[1] * v2.v[1];
}

////////////////////////////////////////////////////
// CVector3f cross CVector3f
////////////////////////////////////////////////////
CVector3f Cross(CVector3f v1,CVector3f v2)
{
	return CVector3f((v1.v[1] * v2.v[2]) - (v1.v[2] * v2.v[1]),
					 (v1.v[2] * v2.v[0]) - (v1.v[0] * v2.v[2]),
					 (v1.v[0] * v2.v[1]) - (v1.v[1] * v2.v[0]));
}

///////////////////////////////////////////////////
// AngleBetweenVectorsANG()
// angle between two vectors (angle)
////////////////////////////////////////////////////
float AngleBetweenVectorsANG(CVector3f Vec1,CVector3f Vec2)
{
    //return RAD2DEG(	 acos(Dot(Vec1,Vec2) / (Vec1.Length() * Vec2.Length()))	 );
	return RAD2DEG(	 acosf(Dot(Vec1,Vec2) / (Vec1.Length() * Vec2.Length()))	 );
}

////////////////////////////////////////////////////
// AngleBetweenVectorsRAD()
// angle between two vectors (radian)
////////////////////////////////////////////////////
float AngleBetweenVectorsRAD(CVector3f Vec1,CVector3f Vec2)
{
    return 	 acosf(Dot(Vec1,Vec2) / (Vec1.Length() * Vec2.Length()))	 ;
}


void GetLookAtAngles(CVector3f& vFrom, CVector3f& vTo,float& angX,float& angY)
{

	CVector3f vDir = (vTo-vFrom).Normalize();

    if(vDir.v[0] == 0) 
	{

        if(vDir.v[2] < 0)
            angY = 180;
        else
            angY = 0;
    }
    else
        angY = RAD2DEG( -atan2f(vDir.v[0], vDir.v[2]) );

    angX = RAD2DEG( asinf(vDir.v[1]));


	angY = - angY;

	angY+=180.f;






	// all of this function is really buggy and causing bad float values

/*	
	// DEBUG DEBUG DEBUG
	angX = 0.f;
	angY = 0.f;

	return;
	// DEBUG DEBUG DEBUG
	
*/



/*


	// handle angY

	CVector3f v1;
	v1.v[0] = vTo.v[0] - vFrom.v[0];
	v1.v[1] = 0.f;
	v1.v[2] = vTo.v[2] - vFrom.v[2];


	float ang1;

	if (DISTANCE(CVector3f(0.f,0.f,-1.f),v1)>0.01f)
		ang1 = AngleBetweenVectorsANG( (vTo - vFrom).Normalize() ,v1.Normalize());
	else 
		ang1 = 0.f;



	if (vTo.v[0]<vFrom.v[0])
		angY = ang1;
	else
		angY = -ang1;
	
	
	if (angY>=180.f && angY<=360.f)
		angY = angX - 360.f;
	else
	if (angY<=-180.f && angY>=-360.f)
		angY = angY + 360.f;
	

	
	// handle angX

	
	// DEBUG DEBUG DEBUG
	//angX = 0.f;
	//return;
	// DEBUG DEBUG DEBUG
	


	v1.v[0] = vTo.v[0] - vFrom.v[0];
	v1.v[1] = 0.f;
	v1.v[2] = vTo.v[2] - vFrom.v[2];




	if (DISTANCE((vTo - vFrom).Normalize(),v1.Normalize())>0.01f)
		ang1 = AngleBetweenVectorsANG( (vTo - vFrom).Normalize() ,v1.Normalize());
	else 
		ang1 = 0.f;


	if (vTo.v[1]>vFrom.v[1])
		angX = ang1;
	else
		angX = -ang1;
		
	


	if (angX>=180.f && angX<=360.f)
		angX = angX - 360.f;
	else
	if (angX<=-180.f && angX>=-360.f)
		angX = angX + 360.f;
	

	//angX = 0.f;

	
*/


}

////////////////////////////////////////////////////
// GetRayPoint()
// returns a point on a ray
////////////////////////////////////////////////////
CVector3f GetRayPoint(CVector3f RayOrigin,CVector3f RayDirection,float t)
{	

	return CVector3f(RayOrigin.v[0] + t * RayDirection.v[0],
					 RayOrigin.v[1] + t * RayDirection.v[1],
					 RayOrigin.v[2] + t * RayDirection.v[2]);
}

///////////////////////////////////////////////////////
// RayPlaneIntersection()
// returns the intersection between a ray and a plane
// (normal should be just normal+d)
///////////////////////////////////////////////////////
CVector3f RayPlaneIntersection(CVector3f RayOrigin,CVector3f RayDirection,CVector3f PlaneOrigin ,CVector3f PlaneNormal)
{
	float d = - (Dot(PlaneNormal,PlaneOrigin));
	float numer = Dot(PlaneNormal,RayOrigin) + d;
	float denom = Dot(PlaneNormal,RayDirection);
	float t =  - (numer / denom);
	return GetRayPoint(RayOrigin,RayDirection,t);
}



///////////////////////////////////////////////////////
// RayPlaneIntersectionT()
// returns the T intersection between a ray and a plane
// (normal should be just normal+d)
///////////////////////////////////////////////////////
/*float RayPlaneIntersectionT(CVector3f RayOrigin,CVector3f RayDirection,CVector3f PlaneOrigin ,CVector3f PlaneNormal)
{
	//float d = - (Dot(PlaneNormal,PlaneOrigin));

	float d = (Dot(PlaneNormal,PlaneOrigin));

	float numer = Dot(PlaneNormal,RayOrigin) + d;
	float denom = Dot(PlaneNormal,RayDirection);
	float t =  - (numer / denom);
	return t;
}*/





///////////////////////////////////////////////////////
// PointInTriangle()
// false = point OUTSIDE triangle
// true  = point ON triangle
///////////////////////////////////////////////////////
bool PointInTriangle(CVector3f Point,CVector3f T1,CVector3f T2,CVector3f T3)
{
	    // TODO: change this
		CVector3f v1,v2,v3;
		float angle1,angle2,angle3,AnglesSum;
		v1 = T1 - Point;
		v2 = T2 - Point;
		v3 = T3 - Point;
		angle1 = AngleBetweenVectorsANG(v1,v2);
		angle2 = AngleBetweenVectorsANG(v2,v3);
		angle3 = AngleBetweenVectorsANG(v1,v3);
		AnglesSum = angle1+angle2+angle3;

		if (AnglesSum > 359.5f && AnglesSum<360.5f)
		{
			return true;
		}
		return false;
}


bool PointInPolygon(CVector3f& Point, CVector3f* pVerts,int iNum)
{
	int iFirst,iSecond;

	float AnglesSum=0.f;

	CVector3f v1,v2;



	for (int i=0;i<iNum;i++)
	{	
		if (i==iNum-1) // last edge
		{
			iFirst=i;
			iSecond=0;
		}
		else
		{
			iFirst=i;
			iSecond=i+1;
		}

		v1 = pVerts[iFirst]  - Point;
		v2 = pVerts[iSecond] - Point;

		v1.Normalize();
		v2.Normalize();

		AnglesSum+= AngleBetweenVectorsANG( v1,v2 );
		//AnglesSum+= AngleBetweenVectorsANG( v2,v1 );


	}

	if (AnglesSum > 359.0f && AnglesSum<361.0f)
	{
		return true;
	}
	return false;

}



float DISTANCE(CVector3f& v1,CVector3f& v2)
{
	float a = (v1.v[0] - v2.v[0]);
	float b = (v1.v[1] - v2.v[1]);
	float c = (v1.v[2] - v2.v[2]);

	return sqrt (a*a + b*b + c*c);
}



////////////////////////////////////////////////
//*************CMatrix Class*************
////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////
// TODO:
// an optimization would be to create a method inside CMatrix that does:
// 1. CMatrix::MatMakeRot() --> this needs to create the matrix in ONE step - without creating
//                               small (mX*mY*mZ)
// 2. CMatrix::MatMakeMoveAndRotate(x,y,z,xAng,yAng,zAng);
//
// FIXME:
// move to opengl column based system
//////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////
// MatMakeUnit()
// Init matrix to a unit matrix
//////////////////////////////////
void CMatrix::MatMakeUnit()
{
	int i,j;
	for (i=0;i<4;i++)
		for (j=0;j<4;j++)
			if (j!=i) m[i][j]=0; 
			else m[i][j]=1;
}


//////////////////////////////////
// MatMakeMove()
// a translation matrix
//////////////////////////////////
void CMatrix::MatMakeMove(float x,float y,float z)
{
	int i,j;
	for (i=0;i<3;i++)
		for (j=0;j<4;j++)
			if (j!=i) m[i][j]=0;else m[i][j]=1;

	m[3][0] = x;
	m[3][1] = y;
	m[3][2] = z;
	m[3][3] = 1;
}


//////////////////////////////////
// MatMakeSize()
// change size matrix
//////////////////////////////////
void CMatrix::MatMakeSize(float x,float y,float z)
{
	int i,j;
	for (i=0;i<4;i++)
		for (j=0;j<4;j++)
			m[i][j]=0;

	m[0][0] = x;
	m[1][1] = y;
	m[2][2] = z;
	m[3][3] = 1;
}

///////////////////////////////////////////////
// Inverse()
// FIXME: change into correct implementation!
///////////////////////////////////////////////
void CMatrix::Inverse(void)
{
/*	float d = Determinant();

	if (d==0.f)
		return;

	d = 1.0 / d;

	//Matrix		result;

	m[0][0] = d * (m[1][1] * (m[2][2] * m[3][3] - m[3][2] * m[2][3]) + m[2][1] * (m[3][2] * m[1][3] - m[1][2] * m[3][3]) + m[3][1] * (m[1][2] * m[2][3] - m[2][2] * m[1][3]));
	m[1][0] = d * (m[1][2] * (m[2][0] * m[3][3] - m[3][0] * m[2][3]) + m[2][2] * (m[3][0] * m[1][3] - m[1][0] * m[3][3]) + m[3][2] * (m[1][0] * m[2][3] - m[2][0] * m[1][3]));
	m[2][0] = d * (m[1][3] * (m[2][0] * m[3][1] - m[3][0] * m[2][1]) + m[2][3] * (m[3][0] * m[1][1] - m[1][0] * m[3][1]) + m[3][3] * (m[1][0] * m[2][1] - m[2][0] * m[1][1]));
	m[3][0] = d * (m[1][0] * (m[3][1] * m[2][2] - m[2][1] * m[3][2]) + m[2][0] * (m[1][1] * m[3][2] - m[3][1] * m[1][2]) + m[3][0] * (m[2][1] * m[1][2] - m[1][1] * m[2][2]));
	m[0][1] = d * (m[2][1] * (m[0][2] * m[3][3] - m[3][2] * m[0][3]) + m[3][1] * (m[2][2] * m[0][3] - m[0][2] * m[2][3]) + m[0][1] * (m[3][2] * m[2][3] - m[2][2] * m[3][3]));
	m[1][1] = d * (m[2][2] * (m[0][0] * m[3][3] - m[3][0] * m[0][3]) + m[3][2] * (m[2][0] * m[0][3] - m[0][0] * m[2][3]) + m[0][2] * (m[3][0] * m[2][3] - m[2][0] * m[3][3]));
	m[2][1] = d * (m[2][3] * (m[0][0] * m[3][1] - m[3][0] * m[0][1]) + m[3][3] * (m[2][0] * m[0][1] - m[0][0] * m[2][1]) + m[0][3] * (m[3][0] * m[2][1] - m[2][0] * m[3][1]));
	m[3][1] = d * (m[2][0] * (m[3][1] * m[0][2] - m[0][1] * m[3][2]) + m[3][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2]) + m[0][0] * (m[2][1] * m[3][2] - m[3][1] * m[2][2]));
	m[0][2] = d * (m[3][1] * (m[0][2] * m[1][3] - m[1][2] * m[0][3]) + m[0][1] * (m[1][2] * m[3][3] - m[3][2] * m[1][3]) + m[1][1] * (m[3][2] * m[0][3] - m[0][2] * m[3][3]));
	m[1][2] = d * (m[3][2] * (m[0][0] * m[1][3] - m[1][0] * m[0][3]) + m[0][2] * (m[1][0] * m[3][3] - m[3][0] * m[1][3]) + m[1][2] * (m[3][0] * m[0][3] - m[0][0] * m[3][3]));
	m[2][2] = d * (m[3][3] * (m[0][0] * m[1][1] - m[1][0] * m[0][1]) + m[0][3] * (m[1][0] * m[3][1] - m[3][0] * m[1][1]) + m[1][3] * (m[3][0] * m[0][1] - m[0][0] * m[3][1]));
	m[3][2] = d * (m[3][0] * (m[1][1] * m[0][2] - m[0][1] * m[1][2]) + m[0][0] * (m[3][1] * m[1][2] - m[1][1] * m[3][2]) + m[1][0] * (m[0][1] * m[3][2] - m[3][1] * m[0][2]));
	m[0][3] = d * (m[0][1] * (m[2][2] * m[1][3] - m[1][2] * m[2][3]) + m[1][1] * (m[0][2] * m[2][3] - m[2][2] * m[0][3]) + m[2][1] * (m[1][2] * m[0][3] - m[0][2] * m[1][3]));
	m[1][3] = d * (m[0][2] * (m[2][0] * m[1][3] - m[1][0] * m[2][3]) + m[1][2] * (m[0][0] * m[2][3] - m[2][0] * m[0][3]) + m[2][2] * (m[1][0] * m[0][3] - m[0][0] * m[1][3]));
	m[2][3] = d * (m[0][3] * (m[2][0] * m[1][1] - m[1][0] * m[2][1]) + m[1][3] * (m[0][0] * m[2][1] - m[2][0] * m[0][1]) + m[2][3] * (m[1][0] * m[0][1] - m[0][0] * m[1][1]));
	m[3][3] = d * (m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) + m[1][0] * (m[2][1] * m[0][2] - m[0][1] * m[2][2]) + m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]));*/


//	CMatrix4& src=*this;

float a00 = m[0][0];

float a01 = m[1][0];

float a02 = m[2][0];

float a20 = m[0][2];

float a11 = m[1][1];

float a22 = m[2][2];

float a12 = m[2][1];

float a21 = m[2][1];

float a10 = m[0][1];

float a11_22 = a11 * a22;

float a12_21 = a12 * a21;

float a10_22 = a10 * a22;

float a10_21 = a10 * a21;

float a11_20 = a11 * a20;

float a12_20 = a12 * a20;

 

 

float fDetInv = 

a00 * ( a11_22 - a12_21 ) -

a01 * ( a10_22 - a12_20 ) +

a02 * ( a10_21 - a11_20 );

 

const float EPS=1e-12;

if (fabs(fDetInv) < EPS)
	return;
//return false;

 

fDetInv = 1.0 / fDetInv;

 

float b00;

float b10;

float b20;

float b01;

float b11;

float b21;

float b02;

float b12;

float b22;

CMatrix inverse;

inverse.m[0][0] = b00 = fDetInv * ( a11_22 - a12_21 );

inverse.m[1][0] = b01 = -fDetInv * ( a01 * a22 - a02 * a21 );

inverse.m[2][0] = b02 = fDetInv * ( a01 * a12 - a02 * a11 );

inverse.m[3][0] = 0.0f;

inverse.m[0][1] = b10 = -fDetInv * ( a10_22 - a12_20 );

inverse.m[1][1] = b11 = fDetInv * ( a00 * a22 - a02 * a20 );

inverse.m[2][1] = b12 = -fDetInv * ( a00 * a12 - a02 * a10 );

inverse.m[3][1] = 0.0f;

inverse.m[0][2] = b20 = fDetInv * ( a10_21 - a11_20 );

inverse.m[1][2] = b21 = -fDetInv * ( a00 * a21 - a01 * a20 );

inverse.m[2][2] = b22 = fDetInv * ( a00 * a11 - a01 * a10 );

inverse.m[3][2] = 0.0f;

float a30 = m[0][3];

float a31 = m[1][3];

float a32 = m[2][3];

 

inverse.m[0][3] = -( a30 * b00 + a31 * b10 + a32 * b20 );

inverse.m[1][3] = -( a30 * b01 + a31 * b11 + a32 * b21 );

inverse.m[2][3] = -( a30 * b02 + a31 * b12 + a32 * b22 );

inverse.m[3][3] = 1.0f;



for (int i=0;i<4;i++)
	for (int j=0;j<4;j++)
		m[i][j] = inverse.m[i][j];


}

float CMatrix::Determinant(void)
{
	return	  (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * (m[2][2] * m[3][3] - m[3][2] * m[2][3])
		- (m[0][0] * m[2][1] - m[2][0] * m[0][1]) * (m[1][2] * m[3][3] - m[3][2] * m[1][3])
		+ (m[0][0] * m[3][1] - m[3][0] * m[0][1]) * (m[1][2] * m[2][3] - m[2][2] * m[1][3])
		+ (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * (m[0][2] * m[3][3] - m[3][2] * m[0][3])
		- (m[1][0] * m[3][1] - m[3][0] * m[1][1]) * (m[0][2] * m[2][3] - m[2][2] * m[0][3])
		+ (m[2][0] * m[3][1] - m[3][0] * m[2][1]) * (m[0][2] * m[1][3] - m[1][2] * m[0][3]);
	
}

///////////////////////////////////////////
// MatMakeXRot()
// a rotation matrix (around the x axis)
///////////////////////////////////////////
void CMatrix::MatMakeXRot(float radians )
{
  float c = (float)cos( radians );
  float s = (float)sin( radians );


  m[0][0] = 1.0f;  m[0][1] = 0.0f;    m[0][2] = 0.0f;    m[0][3] = 0.0f;
  m[1][0] = 0.0f;  m[1][1] = c;       m[1][2] = s;       m[1][3] = 0.0f;
  m[2][0] = 0.0f;  m[2][1] = -s;      m[2][2] = c;       m[2][3] = 0.0f;
  m[3][0] = 0.0f;  m[3][1] = 0.0f;    m[3][2] = 0.0f;    m[3][3] = 1.0f;

}

///////////////////////////////////////////
// MatMakeYRot()
// a rotation matrix (around the y axis)
///////////////////////////////////////////
void CMatrix::MatMakeYRot(float radians )
{
  float c = (float)cos( radians );
  float s = (float)sin( radians );
	


  m[0][0] = c;     m[0][1] = 0.0f;    m[0][2] = -s;      m[0][3] = 0.0f;
  m[1][0] = 0.0f;  m[1][1] = 1.0f;    m[1][2] = 0.0f;    m[1][3] = 0.0f;
  m[2][0] = s;     m[2][1] = 0.0f;    m[2][2] = c;       m[2][3] = 0.0f;
  m[3][0] = 0.0f;  m[3][1] = 0.0f;    m[3][2] = 0.0f;    m[3][3] = 1.0f;
}

///////////////////////////////////////////
// MatMakeZRot()
// a rotation matrix (around the z axis)
///////////////////////////////////////////
void CMatrix::MatMakeZRot(float radians)
{
  float c = (float)cos( radians );
  float s = (float)sin( radians );

  
  m[0][0] = c;     m[0][1] = s;       m[0][2] = 0.0f;    m[0][3] = 0.0f;
  m[1][0] = -s;    m[1][1] = c;       m[1][2] = 0.0f;    m[1][3] = 0.0f;
  m[2][0] = 0.0f;  m[2][1] = 0.0f;    m[2][2] = 1.0f;    m[2][3] = 0.0f;
  m[3][0] = 0.0f;  m[3][1] = 0.0f;    m[3][2] = 0.0f;    m[3][3] = 1.0f;
}


///////////////////////////////////////////
// MatMakeZRot()
// a rotation matrix (around the x,y,z axis)
///////////////////////////////////////////
void CMatrix::MatMakeRot(float xrad,float yrad,float zrad)
{
  CMatrix x, y, z;
  x.MatMakeXRot (xrad);
  y.MatMakeYRot (yrad);
  z.MatMakeZRot (zrad);
  
  (*this)= (y*x*z);
  //(*this)= (z*y*x);
  //(*this)= (y*z*x);
}

///////////////////////////////////////////
// SetPerspective()
// sets a perspective matrix
///////////////////////////////////////////
void CMatrix::SetPerspective(	float left, float right, float bottom,
								float top, float n, float f)
{
	float nudge=0.999f;	//prevent artifacts with infinite far plane
	
	MatMakeUnit();
	//LoadZero();

	//check for division by 0
	if(left==right || top==bottom || n==f)
		return;


	
	m[0][0]=(2*n)/(right-left);
	m[1][1]=(2*n)/(top-bottom);

	m[2][0]=(right+left)/(right-left);
	m[3][1]=(top+bottom)/(top-bottom);

	if(f!=-1)
	{
		m[2][2]=-(f+n)/(f-n);
	}
	else		//if f==-1, use an infinite far plane
	{
		m[2][2]=-1*nudge;
	}

	/////00/01/02/03
	//0//00 01 02 03
	//1//04 05 06 07
	//2//08 09 10 11
	//3//12 13 14 15

	m[2][3]=-1;

	if(f!=-1)
	{
		m[3][2]=-(2*f*n)/(f-n);
	}
	else		//if f==-1, use an infinite far plane
	{
		m[3][2]=-2*n*nudge;
	}
}

///////////////////////////////////////////
// SetPerspective()
// sets a perspective matrix
///////////////////////////////////////////
void CMatrix::SetPerspective(float fovy, float aspect, float n, float f)
{
	float left, right, top, bottom;

	//convert fov from degrees to radians
	fovy*=0.017453295f;

	top=n*(float)tan(double(fovy/2.0f));
	bottom=-top;

	left=aspect*bottom;
	right=aspect*top;

	SetPerspective(left, right, bottom, top, n, f);
}

////////////////////////////////
  // MatImportFrom3x3()
  // Import from a 3x3 matrix
  ///////////////////////////////
void CMatrix::MatImportFrom3x3(float* pInput)
{
	/*m[0][0] = pInput[0 + 0*3];
	m[0][1] = pInput[0 + 1*3];
	m[0][2] = pInput[0 + 2*3];
	m[0][3] = 0.f;

	m[1][0] = pInput[1 + 0*3];
	m[1][1] = pInput[1 + 1*3];
	m[1][2] = pInput[1 + 2*3];
	m[1][3] = 0.f;

	m[2][0] = pInput[2 + 0*3];
	m[2][1] = pInput[2 + 1*3];
	m[2][2] = pInput[2 + 2*3];
	m[2][3] = 0.f;*/


	m[0][0] = pInput[0*3 + 0];
	m[0][1] = pInput[0*3 + 1];
	m[0][2] = pInput[0*3 + 2];
	m[0][3] = 0.f;

	m[1][0] = pInput[1*3 + 0];
	m[1][1] = pInput[1*3 + 1];
	m[1][2] = pInput[1*3 + 2];
	m[1][3] = 0.f;

	m[2][0] = pInput[2*3 + 0];
	m[2][1] = pInput[2*3 + 1];
	m[2][2] = pInput[2*3 + 2];
	m[2][3] = 0.f;
	
	// translation

	m[3][0] = 0;
	m[3][1] = 0;
	m[3][2] = 0;
	m[3][3] = 1.f;

}


////////////////////////////////////////////////////////
// Turn my ROW based matrix into a COLUMN based matrix
////////////////////////////////////////////////////////
void CMatrix::ConvertToOpenglMatrix(float * faMatrix)
{
/*glGetFloatv( GL_PROJECTION_MATRIX, proj );			// Grab The Current PROJECTION Matrix
glGetFloatv( GL_MODELVIEW_MATRIX, modl );			// Grab The Current MODELVIEW Matrix*/


/*for (int i=0;i<4;i++)
	for (int j=0;j<4;j++)
	{
		faMatrix[i + j*4] = m[i][j];
	}*/


for (int i=0;i<4;i++)
	for (int j=0;j<4;j++)
	{
		faMatrix[i*4 + j] = m[i][j];
	}

}

///////////////////////////////////////////////////////////////////
////////////********* Matrix Operators************/////////////////
///////////////////////////////////////////////////////////////////



///////////////////////////////////////////
// vector3 * matrix
///////////////////////////////////////////
CVector3f CMatrix::operator * (CVector3f v)
{
  CVector3f res;
  res.v[0] = ( v.v[0] * m[0][0] ) + ( v.v[1] * m[1][0] ) + ( v.v[2] * m[2][0] ) + m[3][0];
  res.v[1] = ( v.v[0] * m[0][1] ) + ( v.v[1] * m[1][1] ) + ( v.v[2] * m[2][1] ) + m[3][1];
  res.v[2] = ( v.v[0] * m[0][2] ) + ( v.v[1] * m[1][2] ) + ( v.v[2] * m[2][2] ) + m[3][2];
  return res;  
}

///////////////////////////////////////////
// vector4 * matrix
///////////////////////////////////////////
CVector4f CMatrix::operator * (CVector4f v)
{
	CVector4f res;
	res.v[0] = ( v.v[0] * m[0][0] ) + ( v.v[1] * m[1][0] ) + ( v.v[2] * m[2][0] ) + (v.v[3] * m[3][0]);
	res.v[1] = ( v.v[0] * m[0][1] ) + ( v.v[1] * m[1][1] ) + ( v.v[2] * m[2][1] ) + (v.v[3] * m[3][1]);
	res.v[2] = ( v.v[0] * m[0][2] ) + ( v.v[1] * m[1][2] ) + ( v.v[2] * m[2][2] ) + (v.v[3] * m[3][2]);
	res.v[3] = ( v.v[0] * m[0][3] ) + ( v.v[1] * m[1][3] ) + ( v.v[2] * m[2][3] ) + (v.v[3] * m[3][3]);

	return res;  
}

///////////////////////////////////////////
// matrix * matrix
///////////////////////////////////////////
CMatrix CMatrix::operator * (CMatrix m1)
{
  int i,j,k;
  CMatrix res;

  for (i=0;i<4;i++)
    for (j=0;j<4;j++)
    {
      res.m[i][j]=0;
      for (k=0;k<4;k++)
	res.m[i][j] += m[k][j] * m1.m[i][k];
    }       
  return res;
}

///////////////////////////////////////////
// matrix = matrix
///////////////////////////////////////////
CMatrix CMatrix::operator = (const CMatrix m1)
{
  int i,j;
 
  for (i=0;i<4;i++)
    for (j=0;j<4;j++)
      m[i][j] = m1.m[i][j]; 
  return *this;
}

///////////////////////////////////////////
// matrix = pointer to matrix (faster)
///////////////////////////////////////////
CMatrix CMatrix::operator = (const CMatrix* m1)
{
	int i,j;

	if (m1==NULL)
	{
	   MatMakeUnit();	
	}
	else
	{	
		for (i=0;i<4;i++)
			for (j=0;j<4;j++)
				m[i][j] = (*m1).m[i][j]; 
	}
	return *this;
}



CVector3f GetVectorFromAngles(float fAngX,float fAngY)
   {
	CMatrix t_RotateMatrix;
	CVector3f LookDirection;
	LookDirection.v[0]=LookDirection.v[1]=0.f;
	LookDirection.v[2]=-1;

	
	t_RotateMatrix.MatMakeRot(DEG2RAD(fAngX),DEG2RAD(0),DEG2RAD(0));
	LookDirection=t_RotateMatrix*LookDirection;
	
	t_RotateMatrix.MatMakeRot(DEG2RAD(0),DEG2RAD(fAngY),DEG2RAD(0)); 
	LookDirection=t_RotateMatrix*LookDirection;
	
	return LookDirection.Normalize();
   }


void CPlane::FindPlane(const CVector3f& vec1, const CVector3f& vec2, const CVector3f& vec3)
{
	n = ((vec2-vec1)^(vec3-vec1));
	n.Normalize();
	d = n*vec1;
}


void CPlane::FillFromOtherPlaneAndMultiplyByMatrix(CPlane& from, CMatrix& m)
{
	CVector4f t_v;
	t_v.Set(from.n.v[0],from.n.v[1],from.n.v[2],from.d);
	t_v = m*t_v;
}

bool CPlane::IsPointOnPlane(CVector3f& pPoint)
{

	float	newD = Dot(pPoint,n) - d;  

	if (newD>EPSILON) return false; // infront

	if (newD<-EPSILON) return false; // behind

	return true; //on plane
	
}


bool CPlane::IsPointOnPlane(CVector3f& pPoint,float fWithinDistance)
{
	float	newD = Dot(pPoint,n) - d;  

	if (newD>0.1f+fWithinDistance) return false; // infront

	if (newD<-0.1f-fWithinDistance) return false; // behind

	return true; //on plane

}

void CPlane::Noralize(void)
{
	float magnitude = (float)sqrt( n.v[0] * n.v[0] + 
								   n.v[1] * n.v[1] + 
								   n.v[2] * n.v[2] );

	n.v[0] /= magnitude;
	n.v[1] /= magnitude;
	n.v[2] /= magnitude;
	d /= magnitude; 
}

/*inline int PointOnPlaneSide(const VECTOR3 &V,const PLANE &P) 
{  
	float	d = V*P.n - P.d;  
	if (d>PLANE_EPSILON) return FRONTSIDE;  
	if (d<-PLANE_EPSILON) return BACKSIDE;  
	return ON_PLANE; 
}*/



CVector3f RayPlaneIntersection(CVector3f& RayStart,CVector3f& RayEnd,CPlane &Plane)
{

	CVector3f RayDirection=RayEnd-RayStart;

	float numer = Dot(Plane.n,RayStart) - Plane.d;
	float denom = Dot(Plane.n,RayDirection);
	float t =  - (numer / denom);

	float a = RayEnd.v[0]-RayStart.v[0];
	float b = RayEnd.v[1]-RayStart.v[1];
	float c = RayEnd.v[2]-RayStart.v[2];

/*	float distance = sqrt( (a*a)+(b*b)+(c*c));

	if (distance<=t)*/
	//{
	return GetRayPoint(RayStart,RayDirection,t);
	//}
	/*else
		return NULL;*/
}







float RayPlaneIntersectionT(CVector3f& RayOrigin,CVector3f& RayDirection,CPlane& plane)
{
//	float d = (Dot(PlaneNormal,PlaneOrigin));

	float numer = Dot(plane.n,RayOrigin) + (plane.d );
	float denom = Dot(plane.n,RayDirection);
	float t =  - (numer / denom);
	return t;
}






bool PointInBBox(const CVector3f& point,const BBox& bbox)
{
	if (point.v[0]<bbox.min.v[0] || point.v[0]>bbox.max.v[0] ||
		point.v[1]<bbox.min.v[1] || point.v[1]>bbox.max.v[1] ||
		point.v[2]<bbox.min.v[2] || point.v[2]>bbox.max.v[2])
		return false;

	return true;

}

bool BBoxesIntersect(const CVector3f&min1,const CVector3f&max1,const CVector3f&min2,const CVector3f&max2)
{
	if (max1.v[0]<min2.v[0]||min1.v[0]>max2.v[0]) return false;
	if (max1.v[1]<min2.v[1]||min1.v[1]>max2.v[1]) return false;
	if (max1.v[2]<min2.v[2]||min1.v[2]>max2.v[2]) return false;

	return true;


/*inline bool BoundingBoxIntersect(const BBOX &bbox1,const BBOX &bbox2)
{
	if (bbox1.max.x<bbox2.min.x||bbox1.min.x>bbox2.max.x) return false;
	if (bbox1.max.y<bbox2.min.y||bbox1.min.y>bbox2.max.y) return false;
	if (bbox1.max.z<bbox2.min.z||bbox1.min.z>bbox2.max.z) return false;

	return true;
}*/

}



// LEFT
// RIGHT
// TOP
// BOTTOM
// FRONT
// BACK

void MakeBrushFromBoundingBox(CVector3f& vMin, CVector3f& vMax,CPlane* pPlanes)
{
	//original

	//LEFT

	pPlanes[0].n.Set(-1.f,0.f,0.f);
	pPlanes[0].d = -vMin.v[0];

	//RIGHT

	pPlanes[1].n.Set(1.f,0.f,0.f);
	pPlanes[1].d = vMax.v[0];



	//TOP

	pPlanes[2].n.Set(0.f,1.f,0.f);
	pPlanes[2].d = vMax.v[1];

	//BOTTOM

	pPlanes[3].n.Set(0.f,-1.f,0.f);
	pPlanes[3].d = -vMin.v[1];


	//FRONT

	pPlanes[4].n.Set(0.f,0.f,-1.f);
	pPlanes[4].d = -vMin.v[2];

	//BACK

	pPlanes[5].n.Set(0.f,0.f,1.f);
	pPlanes[5].d = vMax.v[2];

}
