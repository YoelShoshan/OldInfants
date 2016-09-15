#include "cgslShader.h"

#include <Windows.h>
#include <gl\glu.h>			
#include "glaux.h"
#include "glext.h"
#include <assert.h>
//#include "ZedTracker.h"

#include "Cg/cg.h"
#include "Cg/cgGL.h"

#include "LogFile.h"
extern CLogFile g_LogFile;

extern float g_fDebugRotate;

#include "Camera.h"
extern CCamera g_Camera1;

#include "Singleton.h"

#include "Timer.h"

#include "Engine.h"

#include "GMath.h"

#include "CheckersBoard.h"
extern CCheckersBoard* g_pCheckersBoard;

extern void handleCgError();

extern int  g_PointedSquareIndexX;
extern int  g_PointedSquareIndexY;
extern CVector3f g_vBoardRayCollision;

///////////////////////////
#include "ZedTracker.h"
///////////////////////////

CcgslShader::CcgslShader()
{
	m_BUMPMAPvertexProgram=NULL;
	m_BUMPMAPfragmentProgram=NULL;

	cgSetErrorCallback(handleCgError);
	m_context = cgCreateContext();

	/////////////
	m_cgLightPos=NULL;
	m_cgDiffuseColor=NULL;
	m_cgSpecularColor=NULL;

	m_cgRegularTexture=NULL;
	m_cgNormalTexture=NULL;
	m_cgSpecularTexture=NULL;

	m_cgSpecialTransform=NULL;
	m_cgLightColor=NULL;


}

CcgslShader::~CcgslShader()
{	
}

void CcgslShader::EnableProfiles(void)
{
	cgGLEnableProfile(m_vertexProfile);
	cgGLEnableProfile(m_fragmentProfile);
}


void CcgslShader::DisableProfiles(void)
{
	cgGLDisableProfile(m_vertexProfile);
	cgGLDisableProfile(m_fragmentProfile);
}

void CcgslShader::ChooseProfiles(void)
{
/*	// Make sure that the appropriate profiles are available on the
	// user's system.
	if (cgGLIsProfileSupported(CG_PROFILE_ARBVP1))
		m_vertexProfile = CG_PROFILE_ARBVP1;
	else {
		// try VP30
		if (cgGLIsProfileSupported(CG_PROFILE_VP30))
			m_vertexProfile = CG_PROFILE_VP30;
		else {
			//fprintf(stderr, "Neither arbvp1 or vp30 vertex profiles supported on this system.\n");
			exit(1);
			}
	}

	if (cgGLIsProfileSupported(CG_PROFILE_ARBFP1))
		m_fragmentProfile = CG_PROFILE_ARBFP1;
	else {
		// try FP30
		if (cgGLIsProfileSupported(CG_PROFILE_FP30))
			m_fragmentProfile = CG_PROFILE_FP30;
		else {
			//fprintf(stderr, "Neither arbfp1 or fp30 fragment profiles supported on this system.\n");
			exit(1);
			}
	}*/


	m_vertexProfile=cgGLGetLatestProfile(CG_GL_VERTEX);  
	m_fragmentProfile=cgGLGetLatestProfile(CG_GL_FRAGMENT);
	cgGLSetOptimalOptions(m_vertexProfile);  
	cgGLSetOptimalOptions(m_fragmentProfile);
}


void CcgslShader::LoadCgPrograms(const char* pVP,const char* pFP,
								 CGprogram& vertexProgram, CGprogram& fragmentProgram)
{
	//assert(cgIsContext(m_context));

	// Load and compile the vertex program from demo_vert.cg; hold on to the
	// handle to it that is returned.

	 const char** args=NULL;
     args=new const char*[2];
     args[0]="-posinv";
     args[1]=NULL;

	vertexProgram = cgCreateProgramFromFile(m_context, CG_SOURCE,pVP,
		m_vertexProfile, "main", args);
		//m_vertexProfile, "main", NULL);

	if (!cgIsProgramCompiled(vertexProgram))
		cgCompileProgram(vertexProgram);

	// Enable the appropriate vertex profile and load the vertex program.
	cgGLEnableProfile(m_vertexProfile);
	cgGLLoadProgram(vertexProgram);

	// And similarly set things up for the fragment program.
	fragmentProgram = cgCreateProgramFromFile(m_context, CG_SOURCE,pFP,
		m_fragmentProfile, "main", NULL);

	if (!cgIsProgramCompiled(fragmentProgram))
		cgCompileProgram(fragmentProgram);

	cgGLEnableProfile(m_fragmentProfile);
	cgGLLoadProgram(fragmentProgram);
}

void CcgslShader::LoadAllCgPrograms(void)
{	
	ChooseProfiles();

	// RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE 

	LoadCgPrograms("cgShaders/BumpMapVP.cg","cgShaders/BumpMapFP.cg",
		m_BUMPMAPvertexProgram,m_BUMPMAPfragmentProgram);

	// DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG

	/*LoadCgPrograms("cgShaders/BasicVP.cg","cgShaders/BasicFP.cg",
	m_BUMPMAPvertexProgram,m_BUMPMAPfragmentProgram);*/

	LoadCgPrograms("cgShaders/BasicVP.cg","cgShaders/BasicFP.cg",
		m_BASICvertexProgram,m_BASICfragmentProgram);

}

void CcgslShader::PerFrameSetupBASIC(void)
{
	// Now make sure that the vertex and fragment programs, loaded
	// in LoadCgPrograms() are bound.
	cgGLBindProgram(m_BASICvertexProgram);
	cgGLBindProgram(m_BASICfragmentProgram);

	
	cgGLSetStateMatrixParameter(cgGetNamedParameter(m_BASICvertexProgram, "modelViewProj"),
		CG_GL_MODELVIEW_PROJECTION_MATRIX,
		CG_GL_MATRIX_IDENTITY);


	m_cgRegularTexture = cgGetNamedParameter(m_BASICfragmentProgram,
		"RegularTexture");
}


void CcgslShader::UpdateSpecialTransformBUMPMAP(float* pfSpecialTransform)
{	
	/*m_cgSpecialTransform = cgGetNamedParameter(m_BUMPMAPvertexProgram, "specialTransform");
	cgGLSetParameterArray4f(m_cgSpecialTransform, 0, 16, &pfSpecialTransform[0]); */

	cgGLBindProgram(m_BUMPMAPvertexProgram);

	m_cgSpecialTransform = cgGetNamedParameter(m_BUMPMAPvertexProgram, "specialTransform");
	cgGLSetMatrixParameterfc(m_cgSpecialTransform,&pfSpecialTransform[0]); 
	//cgGLSetMatrixParameterfr(m_cgSpecialTransform,&pfSpecialTransform[0]); 
	//void cgGLSetMatrixParameter{fd}{rc}(CGparameter param, const type *matrix); 
}

void CcgslShader::BindVPBUMPMAP(void)
{
	cgGLBindProgram(m_BUMPMAPvertexProgram);
}

void CcgslShader::BindFPBUMPMAP(void)
{
	cgGLBindProgram(m_BUMPMAPfragmentProgram);
}


void CcgslShader::BindVPBASIC(void)
{
	cgGLBindProgram(m_BASICvertexProgram);
}

void CcgslShader::BindFPBASIC(void)
{
	cgGLBindProgram(m_BASICfragmentProgram);

}

void CcgslShader::UpdateModelViewProjectBUMPMAP(void)
{
	cgGLBindProgram(m_BUMPMAPvertexProgram);

	cgGLSetStateMatrixParameter(cgGetNamedParameter(m_BUMPMAPvertexProgram, "modelViewProj"),
		CG_GL_MODELVIEW_PROJECTION_MATRIX,
		CG_GL_MATRIX_IDENTITY);

}

void CcgslShader::UpdateDiffuseColorBUMPMAP(float r, float g, float b)
{
	m_cgDiffuseColor = cgGetNamedParameter(m_BUMPMAPfragmentProgram, "uDiffuseColor");
	if(m_cgDiffuseColor)
		cgGLSetParameter3f(m_cgDiffuseColor, r,g,b);
}

void CcgslShader::UpdateLightPositionBUMPMAP(float x, float y, float z)
{
	cgGLBindProgram(m_BUMPMAPvertexProgram);

	m_cgLightPos = cgGetNamedParameter(m_BUMPMAPvertexProgram, "uLightPos");
	if(m_cgLightPos)
		cgGLSetParameter3f(m_cgLightPos,x,y,z);
}

void CcgslShader::UpdateLightColorBUMPMAP(float r, float g, float b)
{
	cgGLBindProgram(m_BUMPMAPvertexProgram);

	m_cgLightColor = cgGetNamedParameter(m_BUMPMAPfragmentProgram, "uLightColor");
	if(m_cgLightColor)
		cgGLSetParameter3f(m_cgLightColor, r,g,b);
}

void CcgslShader::UpdateLightRadiusBUMPMAP(float fSet)
{
	cgGLBindProgram(m_BUMPMAPvertexProgram);

	m_cgLightRadius = cgGetNamedParameter(m_BUMPMAPvertexProgram, "uLightRadius");
	if(m_cgLightRadius)
		cgGLSetParameter1f(m_cgLightRadius,fSet);

}

void CcgslShader::PerFrameSetupBUMPMAP(void)
{

	///////////////////
	// vertex program
	///////////////////

	// Now make sure that the vertex and fragment programs, loaded
	// in LoadCgPrograms() are bound.
	cgGLBindProgram(m_BUMPMAPvertexProgram);
	

	CVector3f t_vec(0.f,5.f,5.f);
	//CVector3f t_vec(0.f,10.f,80.f);
	//CVector3f t_vec(30.f,20.f,10.f);
	CMatrix mX;
	mX.MatMakeXRot(DEG2RAD(g_fDebugRotate*3.f));
	CMatrix mY;
	mY.MatMakeYRot(DEG2RAD(g_fDebugRotate*3.f));

	t_vec = mX*t_vec;
	t_vec = mY*t_vec;
	t_vec+=g_Camera1.m_Pos;



	




	// ORIGINAL ORIGINAL ORIGINAL ORIGINAL ORIGINAL ORIGINAL ORIGINAL 
	m_cgLightPos = cgGetNamedParameter(m_BUMPMAPvertexProgram, "uLightPos");
	if(m_cgLightPos)
	{
		CVector3f v1 = g_vBoardRayCollision+CVector3f(0.f,12.f,0.f);
		cgGLSetParameter3f(m_cgLightPos,v1.v[0] , v1.v[1], v1.v[2]);		
	}


	g_fDebugRotate+=2.0f;
	if (g_fDebugRotate>360.f)
		g_fDebugRotate-=360.f;


	m_cgEyePos = cgGetNamedParameter(m_BUMPMAPvertexProgram, "uEyePos");
	if(m_cgEyePos)
		cgGLSetParameter3f(m_cgEyePos, g_Camera1.m_Pos.v[0],g_Camera1.m_Pos.v[1],g_Camera1.m_Pos.v[2]);
	// ????
	//cgGLEnableClientState(m_cgEyePos);


	m_cgLightRadius = cgGetNamedParameter(m_BUMPMAPvertexProgram, "uLightRadius");
	if(m_cgLightRadius)
		//cgGLSetParameter1f(m_cgLightRadius,3.f);
		cgGLSetParameter1f(m_cgLightRadius,3000.f);



	// ??? why the second one is identity?????

	cgGLSetStateMatrixParameter(cgGetNamedParameter(m_BUMPMAPvertexProgram, "modelViewProj"),
		CG_GL_MODELVIEW_PROJECTION_MATRIX,
		CG_GL_MATRIX_IDENTITY);





	/**********/


	/*vec4f *pg = new vec4f[16];
	pgArray = cgGetNamedParameter(program, "pg");
	cgGLSetParameterArray4f(pgArray, 0, 16, pg[0].v); */



	/**********/


	/*cgGLSetStateMatrixParameter(cgGetNamedParameter(m_BUMPMAPvertexProgram, "modelView"),
	CG_GL_MODELVIEW_MATRIX,
	CG_GL_MATRIX_IDENTITY);*/

	//cgGLSetStateMatrixParameter(cgGetNamedParameter(m_BUMPMAPvertexProgram, "ModelViewIT"),
	//CG_GL_MODELVIEW_MATRIX,
	//CG_GL_MATRIX_INVERSE_TRANSPOSE);



	//////////////////////////
	// fragment program
	//////////////////////////

	cgGLBindProgram(m_BUMPMAPfragmentProgram);






	//wave sawtooth 0 1 0 5
	//wave sawtooth 0 1 0 0.5
 


	CWave wave;
	//wave.SetParams(WF_SIN,0,1,0,5);
	//wave.SetParams(WF_SIN,0.5,0.25,0,1);
	//wave.SetParams(WF_SIN,0.75,0.25,0,1000);
	
	//float fWave1 = wave.Compute(st_Timer::Inst()->TimerGetTimeFAKE());

	float fWave1 = 1.8f;
	//float fWave1 = 0.9f;


	m_cgDiffuseColor = cgGetNamedParameter(m_BUMPMAPfragmentProgram, "uDiffuseColor");
	if(m_cgDiffuseColor)
		cgGLSetParameter3f(m_cgDiffuseColor, 2.f,2.f,2.f);
		//cgGLSetParameter3f(m_cgDiffuseColor, 5.f,5.f,5.f);

		//cgGLSetParameter3f(m_cgDiffuseColor, fWave1,fWave1,fWave1);
		//cgGLSetParameter3f(m_cgDiffuseColor, fWave1,0.1f,0.1f);
//	cgGLEnableClientState(m_cgDiffuseColor);



	m_cgSpecularColor = cgGetNamedParameter(m_BUMPMAPfragmentProgram, "uSpecularColor");
	if(m_cgSpecularColor)
		//cgGLSetParameter3f(m_cgSpecularColor, 0.3,0.3,0.3);		
		cgGLSetParameter3f(m_cgSpecularColor, 1.5f,1.5f,1.5f);		
//	cgGLEnableClientState(m_cgSpecularColor);

	
	CGparameter m_LightRadius;


	m_cgRegularTexture = cgGetNamedParameter(m_BUMPMAPfragmentProgram,
		"RegularTexture");

	m_cgNormalTexture = cgGetNamedParameter(m_BUMPMAPfragmentProgram,
		"NormalTexture");

	m_cgSpecularTexture = cgGetNamedParameter(m_BUMPMAPfragmentProgram,
		"SpecularTexture");

	
	
}

void CcgslShader::PerFrameDisable(void)
{
/*	cgGLDisableClientState(m_cgLightPos);
	cgGLDisableClientState(m_cgEyePos);
	cgGLDisableClientState(m_cgLightRadius);
	cgGLDisableClientState(m_cgDiffuseColor);
	cgGLDisableClientState(m_cgSpecularColor);

	cgGLDisableClientState(m_cgRegularTexture);
	cgGLDisableClientState(m_cgNormalTexture);
	cgGLDisableClientState(m_cgSpecularTexture);*/
}






/*void CcgslShader::BindCGTexture(UINT uiRegularTexture)
{
	cgGLSetTextureParameter(m_cgRegularTexture,uiRegularTexture);
	cgGLEnableTextureParameter(m_cgRegularTexture);	
}*/

void CcgslShader::BindCGTexture(CGparameter& param, UINT uiRegularTexture)
{
	cgGLSetTextureParameter(param,uiRegularTexture);
	cgGLEnableTextureParameter(param);	
}

void CcgslShader::CgError() 
{

	const char *pchListing = cgGetLastListing(m_context);  
	/*CLog::Inst()->Error("CCGWrapper::ErrorCallback %s",cgGetErrorString(cgLastError));  
	CLog::Inst()->Error("CCGWrapper::ErrorCallback %s",pchListing);*/

	static char buffer[1024];
	sprintf(buffer, "Cg error: %s",cgGetErrorString(cgGetError()));
	g_LogFile.OutPutPlainText(buffer,"3","009900",false);
	g_LogFile.OutPutPlainText(":","3","000000",false);
	sprintf(buffer, "Cg error: %s",cgGetLastListing(m_context));
	g_LogFile.OutPutPlainText(buffer,"3","000099",true);
}