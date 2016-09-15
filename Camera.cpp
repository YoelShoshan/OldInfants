#include "Camera.h"
#include "Math3d.h"
#include "timer.h"
#include "Singleton.h"


///////////////////////////
#include "ZedTracker.h"
///////////////////////////

void CCamera::Init(void)
 {
    m_bIgnoreThisFrame=false;
    Fly=true;
	 //Fly=false;
	RotateYSpeed=1.f;
	RotateXSpeed=0.5f;
	RotateZSpeed=1.f;
	m_Pos.v[0]=m_Pos.v[1]=m_Pos.v[2]=xang=yang=zang=TempPoint.v[0]=TempPoint.v[1]=TempPoint.v[2]=0.0f;
	RotateAroundXAng=RotateAroundYAng=RotateAroundZAng=0.f;
	

	
	//fCameraSpeed = 0.3f;

	fCameraSpeed = 0.1f;

	
	m_mCameraMove.MatMakeUnit();
	m_mCameraRotate.MatMakeUnit();

	m_dwMoveRotFlags = 0;
 }
 
 CVector3f CCamera::GetPosition(void)
 {
	 return m_Pos;
 }
 void CCamera::InitMoveRotFlags(void)
 {
	 m_dwMoveRotFlags = 0;
 }
 

 
 void CCamera::UpdateMatrices(void)
 {
	 m_mCameraMove.MatMakeMove(-m_Pos.v[0],-m_Pos.v[1],-m_Pos.v[2]);

	 CMatrix r1,r2;

	 r1.MatMakeRot(DEG2RAD(-xang),DEG2RAD(0.f),DEG2RAD(0.f));
	 r2.MatMakeRot(DEG2RAD(0.f),DEG2RAD(-yang),DEG2RAD(0.f));

	 m_mCameraRotate = r1*r2;
}

 CMatrix* CCamera::ReturnCameraMatrix(void)
 {
    CMatrix* returnM = new CMatrix;

	(*returnM) = m_mCameraRotate*m_mCameraMove;

	 return returnM;
 }
 
 void CCamera::SetPosition(float fX,float fY,float fZ)
 {
	 m_Pos.Set(fX,fY,fZ);
 }

 void CCamera::SetPosition(const CVector3f& pVec)
 {
	 m_Pos.Set(pVec.v[0],pVec.v[1],pVec.v[2]);
 }

 void CCamera::SetAngles(float fXAng,float fYAng,float fZAng)
 {
	 xang = fXAng;
	 yang = fYAng;
	 zang = fZAng;
 }
 
 
 float CCamera::AngleBetweenVectorAndCamera(CVector3f v)
 {
	 // Changes during cleaning:
     // changed the all m_mCameraRotate into t_mRotate;
	 CVector3f CameraDirectionVector;
	 CMatrix t_mRotate;
	 CameraDirectionVector.v[0]=CameraDirectionVector.v[1]=CameraDirectionVector.v[2]=0.f;
	 t_mRotate.MatMakeRot(DEG2RAD(0),DEG2RAD(yang),DEG2RAD(0));
	 CameraDirectionVector.v[2]=fCameraSpeed;
	 CameraDirectionVector=t_mRotate*CameraDirectionVector;
	
	 return AngleBetweenVectorsANG(CameraDirectionVector,v);
 }

 // it seems like it should be IsAngleBetweenVECTORSBiggerThan90
 bool CCamera::IsAngleBetweenVectorAndCameraBiggerThan90(CVector3f v1,CVector3f v2)
 {
	 float angle;
	 angle = AngleBetweenVectorsANG(v1,v2);

	 if ((angle<-90) || angle>90)
		 return false;
	 else
		 return true;
 }

 bool CCamera::CanISeeCreature(CVector3f v)
 {
	 // Changes during cleaning:
	 // changed the all m_mCameraRotate into t_mRotate;

	CVector3f CameraDirectionVector;
	CMatrix t_mRotate;


	CameraDirectionVector.v[0]=CameraDirectionVector.v[1]=CameraDirectionVector.v[2]=0.f;
	t_mRotate.MatMakeRot(DEG2RAD(xang),DEG2RAD(yang),DEG2RAD(zang));
	CameraDirectionVector.v[2]=fCameraSpeed;
	CameraDirectionVector=t_mRotate*CameraDirectionVector;

	v.v[1]=0;

	AngleBetweenVectorsTemp=AngleBetweenVectorsANG(CameraDirectionVector,v);
	return IsAngleBetweenVectorAndCameraBiggerThan90(CameraDirectionVector,v);

 }

 void CCamera::RotateAround(CVector3f point,float ZDistance,float Xang,float Yang,float Zang)
 {
	CVector3f v;
	CMatrix Move,Rotate;
	Move.MatMakeMove(0.f,m_Pos.v[1]-point.v[1],-ZDistance);
	Rotate.MatMakeRot(DEG2RAD(Xang),DEG2RAD(Yang),DEG2RAD(Zang));
	//Rotate.MatMakeRot(Xang,Yang,Zang);
	v.v[0]=v.v[1]=v.v[2]=0.f;
	v = (Rotate * Move) * v;
	//v = (Move) * v;
	v.v[0]+=point.v[0];
	v.v[1]+=point.v[1];
	v.v[2]+=point.v[2];
	m_Pos.v[0]=v.v[0];
	m_Pos.v[1]=v.v[1];
	m_Pos.v[2]=v.v[2];
	/*x=0.f;
	y=0.f;
	z=0.f;*/
 }

 CVector3f CCamera::ReturnLookDirectionNormalized(void)
   {
	CMatrix t_RotateMatrix;
	m_LookDirection.v[0]=m_LookDirection.v[1]=0.f;
	m_LookDirection.v[2]=-1;

	
	t_RotateMatrix.MatMakeRot(DEG2RAD(xang),DEG2RAD(0),DEG2RAD(0));
	m_LookDirection=t_RotateMatrix*m_LookDirection;
	
	t_RotateMatrix.MatMakeRot(DEG2RAD(0),DEG2RAD(yang),DEG2RAD(0)); 
	m_LookDirection=t_RotateMatrix*m_LookDirection;
	
	return m_LookDirection;
   }

 CVector3f CCamera::RotateWithCameraRotationMatrix(CVector3f RotateMe)
 {
    CMatrix t_RotateMatrix;

	t_RotateMatrix.MatMakeRot(DEG2RAD(xang),DEG2RAD(0),DEG2RAD(0));
	RotateMe=t_RotateMatrix*RotateMe;
	
	t_RotateMatrix.MatMakeRot(DEG2RAD(0),DEG2RAD(yang),DEG2RAD(0)); 
	RotateMe=t_RotateMatrix*RotateMe;

	RotateMe.Normalize();
	return RotateMe;
 }

 CVector3f CCamera::RotateWithCameraRotationAndTranslateMatrix(CVector3f RotateMe)
 {
    static CMatrix t_RotateMatrix;
	static CMatrix t_MoveMatrix;

	t_MoveMatrix.MatMakeMove(-m_Pos.v[0],-m_Pos.v[1],-m_Pos.v[2]);

	t_RotateMatrix.MatMakeRot(DEG2RAD(-xang),DEG2RAD(0),DEG2RAD(0));
	//RotateMe=t_RotateMatrix*RotateMe;
	
	t_RotateMatrix.MatMakeRot(DEG2RAD(0),DEG2RAD(-yang),DEG2RAD(0)); 
	//RotateMe=t_RotateMatrix*RotateMe;



		
	//RotateMe.Normalize();
	return RotateMe;
}

 //////////////////////
 // GetLookAtAngles()
 // (FROM (0,0,1)!)
 //////////////////////
 void CCamera::GetLookAtAngles(CVector3f& from,CVector3f& to,float& xAng, float& yAng)
 {
	 CVector3f vec1,vec2,vec3;

	 //CVector3f pos(0.f,0.f,0.f);

	 vec1.v[0]=to.v[0]-m_Pos.v[0];;
	 vec1.v[1]=to.v[1]-m_Pos.v[1];;
	 vec1.v[2]=to.v[2]-m_Pos.v[2];;

	 vec2.v[0]=to.v[0]-m_Pos.v[0];;
	 vec2.v[1]=0.f;
	 vec2.v[2]=to.v[2]-m_Pos.v[2];;

	 vec3.v[0]=0.f;
	 vec3.v[1]=0.f;
	 vec3.v[2]=1.f;

	 /*xang=-RAD2DEG(acos(Dot(v1,v2) / (LENGTH(v1) * LENGTH(v2))));
	 yang=RAD2DEG(acos(Dot(v2,v3) / (LENGTH(v2) * LENGTH(v3))));*/

	 xAng = -(AngleBetweenVectorsANG(vec1,vec2));
	 yAng = (AngleBetweenVectorsANG(vec2,vec3));


	 if (to.v[0]<from.v[0])
		 yAng=180.f-yAng;
	 else
		 yAng=-(180.f - yAng);

	 if (to.v[1]>from.v[1])
		 xAng=-xAng;

	 //zAng=0.f;
 }
 
 
 void CCamera::LookAt(CVector3f p1)
 {
	GetLookAtAngles(m_Pos,p1,xang,yang);
	zang=0.f;
 }





 /*CVector3f CCamera::GetDesiredPosition(int iCameraMode)
 {

	 BREAKPOINT(1); // THIS CHANGES THE MATRICES!!!!!!!!

	 CVector3f vDesiredPosition = m_Pos;

	     //CVector3f TempCameraSpeed;
	 float fTempCameraSpeed;


	m_mCameraRotate.MatMakeUnit();
	m_mCameraMove.MatMakeUnit();


	//if (RotateYLeft)
	if (m_dwMoveRotFlags & CAMERA_ACTION_ROT_Y_LEFT)
	{
		yang+=st_Timer::Inst()->GetDeltaTimeFAKE() * RotateYSpeed / 15.f;
	}

	//if (RotateYRight)
	if (m_dwMoveRotFlags & CAMERA_ACTION_ROT_Y_RIGHT)
	{
		yang-=st_Timer::Inst()->GetDeltaTimeFAKE() * RotateYSpeed  / 15.f;
	}

	if (yang<0.f) yang +=360.f;
	if (yang>360.f) yang -=360.f;


	//if (RotateXDown)
	if (m_dwMoveRotFlags & CAMERA_ACTION_ROT_X_DOWN)
	{
		if (xang>-89.f) xang-=st_Timer::Inst()->GetDeltaTimeFAKE() * RotateXSpeed /15.f;		
	}

	//if (RotateXUp)
	if (m_dwMoveRotFlags & CAMERA_ACTION_ROT_X_UP)
	{
		if (xang<89.f) xang+=st_Timer::Inst()->GetDeltaTimeFAKE() * RotateYSpeed /15.f;
	}



	if (m_dwMoveRotFlags & CAMERA_ACTION_ROT_Z_LEFT)
	{
		zang+=st_Timer::Inst()->GetDeltaTimeFAKE() * RotateYSpeed / 15.f;
	}

	if (m_dwMoveRotFlags & CAMERA_ACTION_ROT_Z_RIGHT)
	{
		zang-=st_Timer::Inst()->GetDeltaTimeFAKE() * RotateYSpeed  / 15.f;
	}
	
	
	TempPoint.v[0]=TempPoint.v[1]=TempPoint.v[2]=0.f;

	
	fTempCameraSpeed = fCameraSpeed;
	


	fCameraSpeed*=st_Timer::Inst()->GetDeltaTimeFAKE();

	
	if (m_dwMoveRotFlags & CAMERA_ACTION_MOVE_Z_FORWARD)
	{
		TempPoint.v[2]-=fCameraSpeed;
	}

	
	if (m_dwMoveRotFlags & CAMERA_ACTION_MOVE_Z_BACKWARDS)
	{
		TempPoint.v[2]+=fCameraSpeed;
	}

	//if (y>0.3f) 
	if (m_dwMoveRotFlags & CAMERA_ACTION_MOVE_Y_DOWN)
	{
		TempPoint.v[1]-=fCameraSpeed;
	}
	//if (y<3.9f)
	if (m_dwMoveRotFlags & CAMERA_ACTION_MOVE_Y_UP)
	{
		TempPoint.v[1]+=fCameraSpeed;
	}


	if (m_dwMoveRotFlags & CAMERA_ACTION_MOVE_X_LEFT)
	{
		TempPoint.v[0]-=fCameraSpeed;
	}

	if (m_dwMoveRotFlags & CAMERA_ACTION_MOVE_X_RIGHT)
	{
		TempPoint.v[0]+=fCameraSpeed;
	}
	

	// make m_mCameraMove matrix contain all translation needed
	m_mCameraMove.MatMakeMove(TempPoint.v[0],TempPoint.v[1],TempPoint.v[2]);

	
	// make m_mCameraRotate matrix will contain all rotation needed
	if (iCameraMode == CAMERA_MODE_FLY)
	{
		m_mCameraRotate.MatMakeRot(DEG2RAD(xang),DEG2RAD(yang),DEG2RAD(0));
	}
	else 
	if (iCameraMode == CAMERA_MODE_WALK)// a "walk"
	{
		m_mCameraRotate.MatMakeRot(DEG2RAD(0),DEG2RAD(yang),DEG2RAD(0));
	}
	else // unknown
		return m_Pos;

	
	// TODO:
	// an optimization would be to create a method inside CMatrix that does:
	// 1. CMatrix::MatMakeRot() --> this needs to create the matrix in ONE step - without creating
	//                               small (mX*mY*mZ)
	// 2. CMatrix::MatMakeMoveAndRotate(x,y,z,xAng,yAng,zAng);

	m_mCamera = m_mCameraRotate*m_mCameraMove; // m_mCamera has entire WORLD -> CAMERA matrix now

	TempPoint=m_mCamera*TempPoint;


	// actually change the position of the camera

	///m_Pos+=TempPoint;
	vDesiredPosition+=TempPoint;


	// Basic Collision Detection and Restrictions

	if (xang < -70.f)
		xang = -70.f;
	if (xang > 70.f)
		xang = 70.f;

	fCameraSpeed = fTempCameraSpeed;

	return vDesiredPosition;
 }*/

 /*void CCamera::RefreshPosition(void)
 {

	BREAKPOINT(1); // THIS CHANGES THE MATRICES!!!!!!!!

    float fTempCameraSpeed;


	m_mCameraRotate.MatMakeUnit();
	m_mCameraMove.MatMakeUnit();


	//if (RotateYLeft)
	if (m_dwMoveRotFlags & CAMERA_ACTION_ROT_Y_LEFT)
	{
		yang+=st_Timer::Inst()->GetDeltaTimeFAKE() * RotateYSpeed / 15.f;
	}

	//if (RotateYRight)
	if (m_dwMoveRotFlags & CAMERA_ACTION_ROT_Y_RIGHT)
	{
		yang-=st_Timer::Inst()->GetDeltaTimeFAKE() * RotateYSpeed  / 15.f;
	}

	if (yang<0.f) yang +=360.f;
	if (yang>360.f) yang -=360.f;


	//if (RotateXDown)
	if (m_dwMoveRotFlags & CAMERA_ACTION_ROT_X_DOWN)
	{
		if (xang>-89.f) xang-=st_Timer::Inst()->GetDeltaTimeFAKE() * RotateXSpeed /15.f;		
	}

	//if (RotateXUp)
	if (m_dwMoveRotFlags & CAMERA_ACTION_ROT_X_UP)
	{
		if (xang<89.f) xang+=st_Timer::Inst()->GetDeltaTimeFAKE() * RotateYSpeed /15.f;
	}



	if (m_dwMoveRotFlags & CAMERA_ACTION_ROT_Z_LEFT)
	{
		zang+=st_Timer::Inst()->GetDeltaTimeFAKE() * RotateYSpeed / 15.f;
	}

	if (m_dwMoveRotFlags & CAMERA_ACTION_ROT_Z_RIGHT)
	{
		zang-=st_Timer::Inst()->GetDeltaTimeFAKE() * RotateYSpeed  / 15.f;
	}
	
	
	TempPoint.v[0]=TempPoint.v[1]=TempPoint.v[2]=0.f;

	
	fTempCameraSpeed = fCameraSpeed;
	


	fCameraSpeed*=st_Timer::Inst()->GetDeltaTimeFAKE();

	
	if (m_dwMoveRotFlags & CAMERA_ACTION_MOVE_Z_FORWARD)
	{
		TempPoint.v[2]-=fCameraSpeed;
	}

	
	if (m_dwMoveRotFlags & CAMERA_ACTION_MOVE_Z_BACKWARDS)
	{
		TempPoint.v[2]+=fCameraSpeed;
	}

	//if (y>0.3f) 
	if (m_dwMoveRotFlags & CAMERA_ACTION_MOVE_Y_DOWN)
	{
		TempPoint.v[1]-=fCameraSpeed;
	}
	//if (y<3.9f)
	if (m_dwMoveRotFlags & CAMERA_ACTION_MOVE_Y_UP)
	{
		TempPoint.v[1]+=fCameraSpeed;
	}


	if (m_dwMoveRotFlags & CAMERA_ACTION_MOVE_X_LEFT)
	{
		TempPoint.v[0]-=fCameraSpeed;
	}

	if (m_dwMoveRotFlags & CAMERA_ACTION_MOVE_X_RIGHT)
	{
		TempPoint.v[0]+=fCameraSpeed;
	}
	

	// make m_mCameraMove matrix contain all translation needed
	m_mCameraMove.MatMakeMove(TempPoint.v[0],TempPoint.v[1],TempPoint.v[2]);

	
	// make m_mCameraRotate matrix will contain all rotation needed
	if (Fly)
	{
		m_mCameraRotate.MatMakeRot(DEG2RAD(xang),DEG2RAD(yang),DEG2RAD(0));
	}
	else // a fake "walk"
	{
		m_mCameraRotate.MatMakeRot(DEG2RAD(0),DEG2RAD(yang),DEG2RAD(0));
	}

	
	// TODO:
	// an optimization would be to create a method inside CMatrix that does:
	// 1. CMatrix::MatMakeRot() --> this needs to create the matrix in ONE step - without creating
	//                               small (mX*mY*mZ)
	// 2. CMatrix::MatMakeMoveAndRotate(x,y,z,xAng,yAng,zAng);

	m_mCamera = m_mCameraRotate*m_mCameraMove; // m_mCamera has entire WORLD -> CAMERA matrix now

	TempPoint=m_mCamera*TempPoint;


	// actually change the position of the camera

	m_Pos+=TempPoint;


	// Basic angle restrictions

	if (xang < -50.f)
		xang = -50.f;
	if (xang > 50.f)
		xang = 50.f;

	fCameraSpeed = fTempCameraSpeed;
	
 }*/

