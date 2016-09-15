#ifndef YOEL_CAMERA_H
#define YOEL_CAMERA_H

#pragma once

#include "Math3d.h"

///////////////////////////
//#include "ZedTracker.h"
///////////////////////////

///////////////////////////////////////////////////////////////////////////
// TODO:
// ///////////////* Add m_pmWorldMove, m_pmWorldRotate////////
// * all mouse input handling TRANSLATION into rotation 
//   maybe should be done here
///////////////////////////////////////////////////////////////////////////


#define CAMERA_ACTION_ROT_X_UP         (1<<1)
#define CAMERA_ACTION_ROT_X_DOWN       (1<<2)
#define CAMERA_ACTION_ROT_Y_LEFT       (1<<3)
#define CAMERA_ACTION_ROT_Y_RIGHT      (1<<4)
#define CAMERA_ACTION_ROT_Z_LEFT       (1<<5)
#define CAMERA_ACTION_ROT_Z_RIGHT      (1<<6)

#define CAMERA_ACTION_MOVE_X_LEFT      (1<<7)
#define CAMERA_ACTION_MOVE_X_RIGHT     (1<<8)
#define CAMERA_ACTION_MOVE_Y_UP        (1<<9)
#define CAMERA_ACTION_MOVE_Y_DOWN      (1<<10)
#define CAMERA_ACTION_MOVE_Z_FORWARD   (1<<11)
#define CAMERA_ACTION_MOVE_Z_BACKWARDS (1<<12)




#define CAMERA_MODE_FLY  0
#define CAMERA_MODE_WALK 1

class CCamera
{
public:

 void SetPosition(float fX,float fY,float fZ);
 void SetPosition(const CVector3f& pVec);
 void SetAngles(float fXAng,float fYAng,float fZAng);
 
 CMatrix* ReturnCameraMatrix(void);

 // TODO:
 // ModelToWorldARRAY(CVector3f* pVectors,int iArraySize;
 // ModelToWorldVECTOR(CVector3f* pVec);
 // etc.

  
 void Init(void);
 
 CVector3f GetPosition(void);
 
 void InitMoveRotFlags(void);
    
 float AngleBetweenVectorAndCamera(CVector3f v);
 
 bool IsAngleBetweenVectorAndCameraBiggerThan90(CVector3f v1,CVector3f v2);
 
 bool CanISeeCreature(CVector3f v);
 
 void RotateAround(CVector3f point,float ZDistance,float Xang,float Yang,float Zang);
 
 CVector3f ReturnLookDirectionNormalized(void);
 
 
 CVector3f RotateWithCameraRotationMatrix(CVector3f RotateMe);

 CVector3f RotateWithCameraRotationAndTranslateMatrix(CVector3f RotateMe);

 void UpdateMatrices(void);

 //////////////////////
 // GetLookAtAngles()
 // (FROM (0,0,1)!)
 //////////////////////
 void GetLookAtAngles(CVector3f& from,CVector3f& to,float& xAng, float& yAng);
 
 void LookAt(CVector3f p1);
 
 //void Write(CVector3f p1);
 
 //void RefreshPosition(void);

 //CVector3f GetDesiredPosition(int iCameraMode);

 // all data should be private...

 CVector3f m_Pos;
 CVector3f m_PosZNegated; // for sound...

 CVector3f m_LookDirection;
 CVector3f m_LookDirectionZNegated; // for sound...

 CVector3f m_vLookAt;
 ///CVector3f CameraSpeed;

 float fCameraSpeed;

 float xang,yang,zang;
 float RotateYSpeed,RotateXSpeed,RotateZSpeed;

 // world to camera matrix

 CMatrix m_mCameraMove;
 CMatrix m_mCameraRotate;
 CMatrix m_mCamera; 

 // model to world matrix

 CMatrix* m_pmWorldMove;
 CMatrix* m_pmWorldRotate;
 CMatrix* m_pmWorld; // m_pmWorldRotate*m_pmWorldMove

 //TODO: CVector3f m_CameraLookDirection
 
 
 // DEBUG or NEED TO CHANGE
 CVector3f TempPoint;
 float AngleBetweenVectorsTemp;
 float RotateAroundXAng,RotateAroundYAng,RotateAroundZAng;
 //float TimePassed;


 DWORD m_dwMoveRotFlags;
 
 // Booleans

 bool Fly;

 
 bool m_bIgnoreThisFrame;
  
};


#endif // YOEL_CAMERA_H