#ifndef YOEL_CGSL_SHADER
#define YOEL_CGSL_SHADER

#include <Windows.h>
#include <gl\glu.h>			
#include "glaux.h"
#include "glext.h"
//#include <assert.h>
//#include "ZedTracker.h"

#include "Cg/cg.h"
#include "Cg/cgGL.h"

#ifndef CWD
# define CWD ""
#endif

class CcgslShader
{
public:
	CcgslShader();
	~CcgslShader();
	//Init();

	void ChooseProfiles(void);


	void EnableProfiles(void);
	void DisableProfiles(void);

	void LoadCgPrograms(const char* pVP,const char* pFP,
		CGprogram& vertexProgram, CGprogram& fragmentProgram);


	void LoadAllCgPrograms(void);

	void PerFrameSetupBUMPMAP(void);

	void BindVPBUMPMAP(void);
	void BindFPBUMPMAP(void);

	void BindVPBASIC(void);
	void BindFPBASIC(void);

	void UpdateSpecialTransformBUMPMAP(float* pfSpecialTransform);

	void UpdateModelViewProjectBUMPMAP(void);

	void UpdateDiffuseColorBUMPMAP(float r, float g, float b);

	void UpdateLightPositionBUMPMAP(float x, float y, float z);

	void UpdateLightColorBUMPMAP(float r, float g, float b);

	void UpdateLightRadiusBUMPMAP(float fSet);

	void PerFrameSetupBASIC(void);


	void PerFrameDisable(void);

	void BindCGTexture(CGparameter& param, UINT uiRegularTexture);

	void CgError();



	CGparameter m_cgLightPos;
	CGparameter m_cgEyePos;
	CGparameter m_cgLightRadius;

	CGparameter m_cgDiffuseColor;
	CGparameter m_cgSpecularColor;


	CGparameter m_cgRegularTexture;
	CGparameter m_cgNormalTexture;
	CGparameter m_cgSpecularTexture;

	CGparameter m_cgSpecialTransform;
	CGparameter m_cgLightColor;

	
	

private:
	// hold the Cg context that we're storing our programs
	// in as well as handles to the vertex and fragment program
	
	CGcontext m_context;
	CGprogram m_BUMPMAPvertexProgram, m_BUMPMAPfragmentProgram;
	CGprogram m_BASICvertexProgram, m_BASICfragmentProgram;
	
	CGprofile m_vertexProfile, m_fragmentProfile;

};







#endif