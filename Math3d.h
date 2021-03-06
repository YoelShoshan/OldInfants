#ifndef YOEL_MATH3D_H
#define YOEL_MATH3D_H

#pragma once

#include "Defines.h"


#define EPSILON         0.0078125f
#define TINY_EPSILON    0.00000000001f

#include "Math3d.h"


/////////////////////////////////////////////////////////
//GENERAL TODO:
// /*add consts when it's possible (both parameters and function body)
// add memcpy when possible (ask if memcpy of 3 objects is faster)*/
////////////////////////////////////////////////////////////


/////////////////////////////////////////////
// class CColor4
// *****************
// contains a 4 byte (unsigned char) array
//////////////////////////////////////////////

class CColor4
{
	public:
	byte c[4]; // 4 components: Red Green Blue Alpha

	////////////////
	// Constructer
	///////////////
	CColor4();

	////////////////////////////////
	// Constructer, accepts 4 bytes
	////////////////////////////////
	CColor4(const byte r,const byte g,const byte b,const byte a);

	/////////////////////////
	// Set()
	// simply set the values
	/////////////////////////
	void Set(const byte r,const byte g,const byte b,const byte a);

	//////////////////////////////////////////////////
	// Set() -- faster
	// accepts a reference to a 4 byte array
	///////////////////////////////////////////////////
	void Set(const CColor4& color);
};

///////////////////////////////////////////////////
// *********class CVector3f***********/////////////
///////////////////////////////////////////////////

class CVector3f
{
	public:
	float v[3]; // 3 components - x,y,z

	///////////////////////////////////////////////
	// *********** Constructers ******************
	///////////////////////////////////////////////


	////////////////////////////////////////////
	// Basic Constructer
	// notice: v[0,1,2] aren't inited, for speed
	////////////////////////////////////////////
    CVector3f();


	//////////////////////////////////
	// Constructer with set values
	//////////////////////////////////
	CVector3f(float x, float y, float z);

	//////////////////////////////////
	// Constructer with set values
	//////////////////////////////////
	CVector3f(const CVector3f& vSet);

	// TODO:  CVector3f(float* fBlah)

	/////////////////////
	// Destructer()
	////////////////////
	~CVector3f() {};

	/////////////////////////////////////////////////
	// ***********methods to alter data*************
	/////////////////////////////////////////////////

	//TODO: operator = on other things (like float*, so I can memcpy it)
 

	//////////////////////
	// operator: "="
	///////////////////////
	inline CVector3f operator=(const CVector3f &pVec);


	//////////////////////
	// operator: "=="
	///////////////////////
	inline bool operator==(const CVector3f &pVec) const
	{
		/*if ( (v[0] == pVec.v[0]) && (v[1] == pVec.v[1]) && (v[2] == pVec.v[2]))
			return true;*/

		if ( (v[0] < pVec.v[0]+TINY_EPSILON) && (v[0] > pVec.v[0]-TINY_EPSILON) &&
			 (v[1] < pVec.v[1]+TINY_EPSILON) && (v[1] > pVec.v[1]-TINY_EPSILON) &&
			 (v[2] < pVec.v[2]+TINY_EPSILON) && (v[2] > pVec.v[2]-TINY_EPSILON))
			 return true;

		return false;
	}


	/////////////////////
	// Set() 
	/////////////////////
	void Set(float x, float y, float z);   

	///////////////////////
	// Set()
	///////////////////////
	void Set(CVector3f &Vec3f);

	//////////////////////////////////////////////////
	// Set() - FASTER
	// (pfSet must point to at least 3 valid floats)
	//////////////////////////////////////////////////
	void Set(float* pfSet);


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
    CVector3f operator+(float fNum) ;

	////////////////////
	// CVector - float
	////////////////////  
    CVector3f operator-(float fNum) ;

	////////////////////
	// CVector * float
	////////////////////
    CVector3f operator*(float fNum) ;

	////////////////////
	// CVector / float
	////////////////////
    CVector3f operator/(float fNum) ;


	//////////////////////////////////////////////
    // ***********Vector parameter***********
	//////////////////////////////////////////////


   /////////////////////////
   // Vector + Vector
   ///////////////////////////
   CVector3f operator+(const CVector3f& Vec1) const;
   
   /////////////////////////
   // Vector - Vector
   ///////////////////////////
   CVector3f operator-(const CVector3f& Vec1) const;

   
   /////////////////////////////
   // Vector / Vector
   /////////////////////////////
   CVector3f operator/(const CVector3f& Vec1) const;

   /////////////////////////
   // Vector dot Vector
   ///////////////////////////
   float     operator*(const CVector3f& Vec1) const; // dot product

   /////////////////////////
   // Vector cross Vector
   ///////////////////////////
   CVector3f operator^(const CVector3f& Vec1) const; // cross product



   /////////////////////////////////////////////////////////////////
   ///////*************Operators that DO change inner data*************
   /////////////////////////////////////////////////////////////////

   
   ////////////////////////////
   //// a SCALAR parameter
   ////////////////////////////

   /////////////////////////
   // vector+=scalar
   /////////////////////////
   void operator+=(float fNum) ;
   
   /////////////////////////
   // vector-=scalar
   /////////////////////////
   void operator-=(float fNum) ;

   /////////////////////////
   // vector*=scalar
   /////////////////////////
   void operator*=(float fNum) ;

   /////////////////////////
   // vector/=scalar
   /////////////////////////
   void operator/=(float fNum) ;

   ///////////////////////////////////////////////////////////////////////////
   //// a VECTOR parameter
   //// TODO: maybe i should pass the parameter as a reference (also const)
   ///////////////////////////////////////////////////////////////////////////

   /////////////////////////
   // Vector+=Vector
   /////////////////////////
   void operator+=(CVector3f Vec1) ;
   
   /////////////////////////
   // Vector-=Vector
   /////////////////////////
   void operator-=(CVector3f Vec1) ;

   /////////////////////////////////////
   // Vector*=Vector
   // (should work like a dot product)
   ////////////////////////////////////
   void operator*=(CVector3f Vec1) ;

   /////////////////////////
   // Vector/=Vector
   /////////////////////////
   void operator/=(CVector3f Vec1) ;


   ///////////////////////////////////
   // Operating on the Class itself
   // returns a result, no parameters
   ///////////////////////////////////

   ///////////////////////////////////
   // Length()
   // get the length of the vector
   //////////////////////////////////
   float Length(void);


   //////////////////////////////////
   // Inverse()
   // *(-1) for each axis component
   //////////////////////////////////
   void Inverse(void);

   ////////////////////////////////////////////////////////////
   // Normalize()
   // normalize the vector (after normalization, length==1.f)
   ///////////////////////////////////////////////////////////
   CVector3f Normalize(void);

   float GetSize(void);


/*   //// Only use Add,Subtract,Multiply,Divide on Time Critical stuff
   ///  and only after the Program is finished

   void Add(CVector3f& v1,CVector3f& v2);

   void Subtract(CVector3f& v1,CVector3f& v2);
   
   void Multiply(CVector3f& v1,CVector3f& v2);
   
   void Divide(CVector3f& v1,CVector3f& v2);
   
   void Cross(CVector3f& v1,CVector3f& v2);

   void Inverse(CVector3f& Vec1);

   void Normalize(CVector3f& Vec1);

   void PutRayPoint(CVector3f& RayOrigin,CVector3f& RayDirection,float t);*/
  
};


///////////////////////////////////////////////////
// *********class CVector3f***********/////////////
///////////////////////////////////////////////////

class CVector4f
{
public:
	float v[4]; // 4 components - n.x,n.y,n.z,d

	///////////////////////////////////////////////
	// *********** Constructers ******************
	///////////////////////////////////////////////


	////////////////////////////////////////////
	// Basic Constructer
	// notice: v[0,1,2] aren't inited, for speed
	////////////////////////////////////////////
	CVector4f();


	//////////////////////////////////
	// Constructer with set values
	//////////////////////////////////
	CVector4f(float x, float y, float z,float d);

	//////////////////////////////////
	// Constructer with set values
	//////////////////////////////////
	//CVector3f(const CVector4f& vSet);

	// TODO:  CVector3f(float* fBlah)

	/////////////////////
	// Destructer()
	////////////////////
	~CVector4f() {};

	/////////////////////////////////////////////////
	// ***********methods to alter data*************
	/////////////////////////////////////////////////

	//TODO: operator = on other things (like float*, so I can memcpy it)


	//////////////////////
	// operator: "="
	///////////////////////
	//inline CVector4f operator=(const CVector4 &pVec);


	/////////////////////
	// Set() 
	/////////////////////
	void Set(float x, float y, float z, float d);   

	/////////////////////
	// Normalize()
	//////////////////////

	CVector4f Normalize(void);

	///////////////////////
	// Set()
	///////////////////////
	//void Set(CVector4f &Vec4f);
};


///////////////////////////////////////////////////
// *********class CVector2f***********/////////////
///////////////////////////////////////////////////

class CVector2f
{
	public:
	float v[2]; // 2 components - x,y

	///////////////////////////////////////////////
	// *********** Constructers ******************
	///////////////////////////////////////////////
	

	///////////////////////
	//Constructer()
	///////////////////////
	CVector2f();

	////////////////////////////////////
	//Constructer()
	// constructs and inits using x,y
	////////////////////////////////////
	CVector2f(float x, float y);


	/////////////////////////////////
	//Destructer()
	//////////////////////////////////
	~CVector2f() {};
     

	///////////////////////////////
	//Set() - accepts 2 floats
	////////////////////////////////
	void Set(float x, float y);   


	/////////////////////////////////////////
	//Set() // accept a reference to a vector
	/////////////////////////////////////////
	void Set(CVector2f &Vec2f);

	//////////////////////////////////////////////////////////
	//Set() // accepts a float array (of at least 2 elements)
	///////////////////////////////////////////////////////////
	void Set(float* pfSet);

	/////////////////////////////////////////////////////////////////
	///////*************Operators that DO NOT change inner data*************
	/////////////////////////////////////////////////////////////////


	/////////////////////////////////
	//// *****a SCALAR parameter****
	/////////////////////////////////


	////////////////////////////
	//// vector2f + float
	////////////////////////////
    CVector2f operator+(float fNum) ;
   
	////////////////////////////
	//// vector2f - float
	////////////////////////////
    CVector2f operator-(float fNum) ;

	////////////////////////////
	//// vector2f * float
	////////////////////////////
    CVector2f operator*(float fNum) ;

	////////////////////////////
	//// vector2f / float
	////////////////////////////
	CVector2f operator/(float fNum) ;

   
	/////////////////////////////////
	//// *****a VECTOR parameter****
	/////////////////////////////////

	/////////////////////////////////
	//// vector + vector
	/////////////////////////////////
    CVector2f operator+(CVector2f Vec1) ;
   
	/////////////////////////////////
	//// vector - vector
	/////////////////////////////////
    CVector2f operator-(CVector2f Vec1) ;

	/////////////////////////////////
	//// vector dot vector
	/////////////////////////////////
    float     operator*(CVector2f Vec1) ;  // dot product

	/////////////////////////////////
	//// vector / vector
	/////////////////////////////////
	CVector2f operator/(CVector2f Vec1) ;

   
   	/////////////////////////////////////////////////////////////////
	///////*************Operators that CHANGE inner data*************
	/////////////////////////////////////////////////////////////////


	/////////////////////////////////
	//// *****a SCALAR parameter****
	/////////////////////////////////

   
	/////////////////////////////////
	//// vector+=scalar
	/////////////////////////////////
	void operator+=(float fNum) ;
   
	/////////////////////////////////
	//// vector-=scalar
	/////////////////////////////////
    void operator-=(float fNum) ;

	/////////////////////////////////
	//// vector*=scalar
	/////////////////////////////////
    void operator*=(float fNum) ;

	/////////////////////////////////
	//// vector/=scalar
	/////////////////////////////////
    void operator/=(float fNum) ;

   
	/////////////////////////////////
	//// *****a VECTOR parameter****
	/////////////////////////////////

    
	/////////////////////////////////
	//// vector+=vector
	/////////////////////////////////
	void operator+=(CVector2f Vec1) ;
   
	/////////////////////////////////
	//// vector-=vector
	/////////////////////////////////
    void operator-=(CVector2f Vec1) ;

	/////////////////////////////////
	//// vector*=vector
	/////////////////////////////////
	void operator*=(CVector2f Vec1) ;

	/////////////////////////////////
	//// vector/=vector
	/////////////////////////////////
	void operator/=(CVector2f Vec1) ;


/*   //// Only use Add,Subtract,Multiply,Divide on Time Critical stuff
   ///  and only after the Program is finished

   void Add(CVector2f& v1,CVector2f& v2);

   void Subtract(CVector2f& v1,CVector2f& v2);
   
   void Multiply(CVector2f& v1,CVector2f& v2);
   
   void Divide(CVector2f& v1,CVector2f& v2);*/

   
/*   void Cross(CVector2f& v1,CVector2f& v2);

   void Inverse(CVector2f& Vec1);

   void Normalize(CVector2f& Vec1);

   void PutRayPoint(CVector2f& RayOrigin,CVector2f& RayDirection,float t);*/
  

/////////////////////////////////////////////
// Fast Operations End
/////////////////////////////////////////////

   // Operating on the Class itself

/*   float Length(void);

   void Inverse(void);

   void Normalize(void);*/

   // on another CVector3f
   
	
};

////////////////////////////////////////////////////
// ***********Stand Alone Functions***************
////////////////////////////////////////////////////

////////////////////////////////////////////////////
// CVector3f dot CVector3f
////////////////////////////////////////////////////
float Dot(CVector3f v1,CVector3f v2);

////////////////////////////////////////////////////
// CVector2f dot CVector2f
////////////////////////////////////////////////////
float Dot(CVector2f v1,CVector2f v2);

////////////////////////////////////////////////////
// CVector3f cross CVector3f
////////////////////////////////////////////////////
CVector3f Cross(CVector3f v1,CVector3f v2);

///////////////////////////////////////////////////
// AngleBetweenVectorsANG()
// angle between two vectors (angle)
////////////////////////////////////////////////////
float AngleBetweenVectorsANG(CVector3f Vec1,CVector3f Vec2);

////////////////////////////////////////////////////
// AngleBetweenVectorsRAD()
// angle between two vectors (radian)
////////////////////////////////////////////////////
float AngleBetweenVectorsRAD(CVector3f Vec1,CVector3f Vec2);

void GetLookAtAngles(CVector3f& vFrom, CVector3f& vTo,float& angX,float& angY);

CVector3f GetRayPoint(CVector3f RayOrigin,CVector3f RayDirection,float t);

CVector3f RayPlaneIntersection(CVector3f RayOrigin, CVector3f RayDirection, 
							   CVector3f PlaneOrigin ,CVector3f PlaneNormal);




///////////////////////////////////////////////////////
// RayPlaneIntersectionT()
// returns the T intersection between a ray and a plane
// (normal should be just normal+d)
///////////////////////////////////////////////////////
/*float RayPlaneIntersectionT(CVector3f RayOrigin,CVector3f RayDirection,CVector3f PlaneOrigin ,CVector3f PlaneNormal);*/




bool PointInTriangle(CVector3f Point,CVector3f T1,CVector3f T2,CVector3f T3);


bool PointInPolygon(CVector3f& Point, CVector3f* pVerts,int iNum);



float DISTANCE(CVector3f& v1,CVector3f& v2);



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

class CMatrix
{
public:  
  float m[4][4];

  //////////////////////////////////
  // MatMakeUnit()
  // Init matrix to a unit matrix
  //////////////////////////////////
  void MatMakeUnit();

  ////////////////////////////////
  // MatImportFrom3x3()
  // Import from a 3x3 matrix
  ///////////////////////////////
  void MatImportFrom3x3(float* pInput);

  //////////////////////////////////
  // MatMakeMove()
  // a translation matrix
  //////////////////////////////////
  void MatMakeMove(float x,float y,float z);

  //////////////////////////////////
  // MatMakeSize()
  // change size matrix
  //////////////////////////////////
  void MatMakeSize(float x,float y,float z);



  // get determinant of matrix
  float Determinant(void);

  ///////////////////////////////////////////////
  // Inverse()
  // FIXME: change into correct implementation!
  ///////////////////////////////////////////////
  void Inverse(void);


  ///////////////////////////////////////////
  // MatMakeXRot()
  // a rotation matrix (around the x axis)
  ///////////////////////////////////////////
  void MatMakeXRot(float radians);

  ///////////////////////////////////////////
  // MatMakeYRot()
  // a rotation matrix (around the y axis)
  ///////////////////////////////////////////
  void MatMakeYRot(float radians);

  ///////////////////////////////////////////
  // MatMakeZRot()
  // a rotation matrix (around the z axis)
  ///////////////////////////////////////////
  void MatMakeZRot(float radians);

  ///////////////////////////////////////////
  // MatMakeZRot()
  // a rotation matrix (around the x,y,z axis)
  ///////////////////////////////////////////
  void MatMakeRot(float xrad,float yrad,float zrad);


  ///////////////////////////////////////////
  // SetPerspective()
  // sets a perspective matrix
  ///////////////////////////////////////////
  void SetPerspective(	float left, float right, float bottom,
								float top, float n, float f);

  ///////////////////////////////////////////
  // SetPerspective()
  // sets a perspective matrix
  ///////////////////////////////////////////
  void SetPerspective(float fovy, float aspect, float n, float f);

  ////////////////////////////////////////////////////////
  // Turn my ROW based matrix into a COLUMN based matrix
  ////////////////////////////////////////////////////////
  void ConvertToOpenglMatrix(float * faMatrix);



  ///////////////////////////////////////////////////////////////////
  ////////////********* Matrix Operators************/////////////////
  ///////////////////////////////////////////////////////////////////





  ///////////////////////////////////////////
  // vector * matrix
  ///////////////////////////////////////////
  CVector3f operator * (CVector3f v);



  ///////////////////////////////////////////
  // vector * matrix
  ///////////////////////////////////////////
  CVector4f operator * (CVector4f v);

  
  ///////////////////////////////////////////
  // matrix * matrix
  ///////////////////////////////////////////
  CMatrix operator * (CMatrix m1);

  ///////////////////////////////////////////
  // matrix = matrix
  ///////////////////////////////////////////
  CMatrix operator = (const CMatrix m1);

  ///////////////////////////////////////////
  // matrix = pointer to matrix (faster)
  ///////////////////////////////////////////
  CMatrix operator = (const CMatrix* m1);
};



CVector3f GetVectorFromAngles(float fAngX,float fAngY);


///////////////////////////////////////////
// class CPlane
///////////////////////////////////////////

class  CPlane
{
public:
	CPlane() {};
	~CPlane() {};

	void FindPlane(const CVector3f& vec1, const CVector3f& vec2, const CVector3f& vec3);

	void FillFromOtherPlaneAndMultiplyByMatrix(CPlane& from, CMatrix& m);

	bool IsPointOnPlane(CVector3f& pPoint);

	bool IsPointOnPlane(CVector3f& pPoint,float fWithinDistance);

	void Noralize(void);

	CVector3f n;			// Plane normal. 
	float     d;					// The plane distance from origin 
};



CVector3f RayPlaneIntersection(CVector3f& RayStart,CVector3f& RayDirection,CPlane &Plane);

float RayPlaneIntersectionT(CVector3f& RayOrigin,CVector3f& RayDirection,CPlane& plane);



struct BBox
{
	CVector3f min,max;
};

bool PointInBBox(const CVector3f& point,const BBox& bbox);

bool BBoxesIntersect(const CVector3f&min1,const CVector3f&max1,const CVector3f&min2,const CVector3f&max2);

void MakeBrushFromBoundingBox(CVector3f& vMin, CVector3f& vMax,CPlane* pPlanes);

#endif	//YOEL_MATH3D_H