#include "Shader.h"

#include <GL/gl.h>
#include "Singleton.h"
#include "Timer.h"
#include "ShaderParser.h"

///////////////////////////
#include "ZedTracker.h"
///////////////////////////

STCModObject::STCModObject()
{
	m_iNum=0;
	m_pfRotate=NULL;
	m_pfScale=NULL;
	m_pfScroll=NULL;
	m_pStretch=NULL;
	m_pfTransform=NULL;
	m_pfTurb=NULL;
};

STCModObject::~STCModObject()
{
	
	SafeDeleteArray(m_pfRotate);
	//SafeDelete(m_pfRotate);
	SafeDeleteArray(m_pfScale);
	SafeDeleteArray(m_pfScroll);
	SafeDeleteArray(m_pStretch);
	//SafeDelete(m_pStretch);
	SafeDeleteArray(m_pfTransform);
	SafeDeleteArray(m_pfTurb);

	
}; 



void STCModObject::AllocAndFillScroll(float fSSpeed,float fTSpeed)
{
	m_dwOrder[m_iNum] = TCMOD_SCROLL;
	m_pfScroll = new float[2];	
	m_pfScroll[0] = fSSpeed;
	m_pfScroll[1] = fTSpeed;
	m_iNum++;
}

void STCModObject::AllocAndFillRotate(float fDeg)
{
	m_dwOrder[m_iNum] = TCMOD_ROTATE;
	m_pfRotate = new float[1];	
	m_pfRotate[0] = fDeg;
	m_iNum++;
}

void STCModObject::AllocAndFillScale(float fSScale,float fTScale)
{
	m_dwOrder[m_iNum] = TCMOD_SCALE;
	m_pfScale = new float[2];	
	m_pfScale[0] = fSScale;
	m_pfScale[1] = fTScale;
	m_iNum++;
}

void STCModObject::AllocAndFillStretch(UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq)
{
	m_dwOrder[m_iNum] = TCMOD_STRETCH;
	m_pStretch = new CWave[1];
	m_pStretch[0].SetParams(uiWaveType,fBase,fAmplitude,fPhase,fFreq);
	m_iNum++;
}


void STCModObject::AllocAndFillTransform(float f0,float f1,float f2,float f3,float f4,float f5)
{
	m_dwOrder[m_iNum] = TCMOD_TRANSFORM;
	m_pfTransform = new float[6];
	m_pfTransform[0] = f0; m_pfTransform[1] = f1; m_pfTransform[2] = f2;
	m_pfTransform[3] = f3; m_pfTransform[4] = f4; m_pfTransform[5] = f5;
    m_iNum++;
}

void STCModObject::AllocAndFillTurb(float fBase,float fAmplitude,float fPhase,float fFreq)
{
	m_dwOrder[m_iNum] = TCMOD_TURB;
	m_pfTurb = new float[4];
	m_pfTurb[0] = fBase;
	m_pfTurb[1] = fAmplitude;
	m_pfTurb[2] = fPhase;
	m_pfTurb[3] = fFreq;
	m_iNum++;
}


/////////////////////////////////////
//
//  CShaderPass class implementation
//
/////////////////////////////////////


CShaderPass::CShaderPass()
{
	m_uiTextureNum=1;
	m_fFreq=1.f;
	//m_uiTexturesIDs    = new UINT[MAX_ANIM_TEX];
	//m_pTextures = new CTexture[MAX_ANIM_TEX];
	

	//TODO: change this so no memory would be wasted (only alloc if needed)
	/*m_ppTextures = new CTexture*[MAX_ANIM_TEX];
	for (int i=0;i<MAX_ANIM_TEX;i++)
		m_ppTextures[i] = new CTexture;*/



	m_uiActiveFrame = 0;
	m_fPassTime = 0;
	/////////////////

	m_bFarBoxUsed=false;

	m_dwFlags = SPF_DEPTHWRITE;

	m_dwSrcBlend = GL_ONE;
	m_dwDstBlend = GL_ZERO;

	m_dwAlphaFunc = ALPHAFUNC_NONE;

	m_dwDEPTHFunc = DEPTHFUNC_LEQUAL;

	m_uiRGBGen=RGBGEN_IDENTITY;
	m_uiALPHAGen=ALPHAGEN_IDENTITY;
	m_dwTCGen=TCGEN_NONE;


	bTCModUsed = false;
}

CShaderPass::~CShaderPass()
{
	
}


void CShaderPass::SetRGBGen(UINT uiRGBGen)
{
	m_uiRGBGen = uiRGBGen;
}

/*UINT CShaderPass::GetRGBGen(void)
{
	return m_uiRGBGen;
}*/



void CShaderPass::SetRGBGenWaveValues(UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq)
{
	m_RGBWave.SetParams(uiWaveType,fBase,fAmplitude,fPhase,fFreq);
}

float CShaderPass::ComputeRGBGenWave(float fTime)
{
	return m_RGBWave.Compute(fTime);
}




void CShaderPass::SetALPHAGen(UINT uiALPHAGen)
{
	m_uiALPHAGen = uiALPHAGen;
}

/*UINT CShaderPass::GetALPHAGen(void)
{
	return m_uiALPHAGen;
}*/

void CShaderPass::SetALPHAGenWaveValues(UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq)
{
	m_ALPHAWave.SetParams(uiWaveType,fBase,fAmplitude,fPhase,fFreq);
}

float CShaderPass::ComputeALPHAGenWave(float fTime)
{
	return m_ALPHAWave.Compute(fTime);
}


void CShaderPass::AddTCModScroll(float fSSpeed, float fTSpeed)
{
	bTCModUsed = true;
	m_TCModObject.AllocAndFillScroll(fSSpeed,fTSpeed);
}

void CShaderPass::AddTCModRotate(float fDeg)
{
	bTCModUsed = true;
	m_TCModObject.AllocAndFillRotate(fDeg);
}

void CShaderPass::AddTCModScale(float fSScale,float fTScale)
{
	bTCModUsed = true;
	m_TCModObject.AllocAndFillScale(fSScale,fTScale);
}

void CShaderPass::AddTCModStretch(UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq)
{
	bTCModUsed = true;
	m_TCModObject.AllocAndFillStretch(uiWaveType,fBase,fAmplitude,fPhase,fFreq);
}

void CShaderPass::AddTCModTransform(float f0,float f1,float f2,float f3,float f4,float f5)
{
	bTCModUsed = true;
	m_TCModObject.AllocAndFillTransform(f0,f1,f2,f3,f4,f5);
}

void CShaderPass::AddTCModTurb(float fBase,float fAmplitude,float fPhase,float fFreq)
{
	bTCModUsed = true;
	m_TCModObject.AllocAndFillTurb(fBase,fAmplitude,fPhase,fFreq);
}

void CShaderPass::SetAnim(UINT uiNum,float fFreq)
{
	m_uiTextureNum = uiNum;
	m_fFreq        = fFreq;
}


/// I HAVE TO TAKE CARE OF THIS SOMEHOW FOR SMART POINTERS!!!!!!!
void CShaderPass::SetAlreadyAllocatedNoAnimTexture(UINT uiTextureID) // for lightmaps
{
	m_uiTextureNum = 1;
	//m_spTextures[0] = NULL;
	//TODO: here I'm supposed to load the actual lightmaps!

	//Explanation - I cannot pass a NULL because then there will be made an access from NULL which would crash,
	// that's why I'm using this "fake" texture to prevent that
	// basically I'm supposed to search through the texturemanager queue list and find a texture with that id,
	// and if it isn't found, I should create a "fake" new texture, containing no data, and only the dimentions and
	// opengl texID !!!!!!!!!!!!!
	// (all that inside the manager)
	m_spTextures[0] = CSingleton<CTextureManager>::Inst()->LoadTexAndPush("ceImages/white.jpg",false,false,true);
}

void CShaderPass::AddNoAnimTexture(char* caTexName,bool bMipMap,bool bClamp)
{
	m_uiTextureNum = 1;

	//m_ppTextures[0] = CSingleton<CTextureManager>::Instance()->LoadTexAndPush(caTexName,bMipMap,bClamp);
	m_spTextures[0] = CSingleton<CTextureManager>::Inst()->LoadTexAndPush(caTexName,bMipMap,bClamp,true);
}

void CShaderPass::AddFrameTexture(UINT uiNum,char* caTexName)
{
	//m_ppTextures[uiNum] = CSingleton<CTextureManager>::Instance()->LoadTexAndPush(caTexName,true,false);
	m_spTextures[uiNum] = CSingleton<CTextureManager>::Inst()->LoadTexAndPush(caTexName,true,false,true);
}


void CShaderPass::SetBlendValues(DWORD dwSrcBlend,DWORD dwDstBlend)
{
	m_dwSrcBlend = dwSrcBlend;
	m_dwDstBlend = dwDstBlend;
}

/*DWORD CShaderPass::GetSrcBlend(void)
{
	return m_dwSrcBlend;
}

DWORD CShaderPass::GetDstBlend(void)
{
	return m_dwDstBlend;
}
*/

bool CShaderPass::IsBlended(void)
{
	if (m_dwSrcBlend != GL_ONE || m_dwDstBlend != GL_ZERO)
		return true;

	return false;
}

UINT CShaderPass::GetTexID(UINT uiNum)
{
	//return m_ppTextures[uiNum]->GetID();
	return m_spTextures[uiNum]->GetID();
}

/*
void CShaderPass::CalcActiveFrame(void)
{	

	if (m_uiTextureNum>1)
	{
		float t_fTemp1 =  st_Timer::Inst()->TimerGetTimeFAKE() / 1000.f * m_fFreq;
		float t_fTemp2=(int)fmodf((float)t_fTemp1,(float)m_uiTextureNum); 

		if (t_fTemp2-floorf(t_fTemp2)>=0.5f)
			m_uiActiveFrame = floorf(t_fTemp2)+1;
		else
			m_uiActiveFrame = floorf(t_fTemp2);
	}
	else
	{
		// TODO: possible to move?
		m_uiActiveFrame = 0;
	}		
}
*/

UINT CShaderPass::GetActiveFrameTexID(void)
{
	if (m_spTextures[m_uiActiveFrame])
		return m_spTextures[m_uiActiveFrame]->GetID();
	else
		return 0; // Todo: return the tex id of white
}



SDeformVertexesObject::SDeformVertexesObject()
{
	m_iMoveNum=m_iWaveNum=m_iNormalNum=m_iBulgeNum=0;

	// move
	m_ppMove = new CWave*[DV_MOVE_MAX];
	m_MoveVert = new CVector3f*[DV_MOVE_MAX];

	// wave

	m_ppWave = new CWave*[DV_WAVE_MAX];
	m_pWaveDivs = new float[DV_WAVE_MAX];

	// autoSprite

	m_bIsAutoSprite = false;

	// bulge

	m_ppfBulgeParms = new float*[DV_BULGE_MAX]; 
}

SDeformVertexesObject::~SDeformVertexesObject()
{
	for (int i=0;i<m_iMoveNum;i++)
	{
        SafeDelete(m_ppMove[i]);
		SafeDelete(m_MoveVert[i]);
	}

	SafeDeleteArray(m_ppMove);
	SafeDeleteArray(m_MoveVert);

	for (int i=0;i<m_iWaveNum;i++)
	{
		SafeDelete(m_ppWave[i]);		
	}

	SafeDeleteArray(m_pWaveDivs);
	SafeDeleteArray(m_ppWave);


	for (int i=0;i<m_iBulgeNum;i++)
	{
		SafeDeleteArray(m_ppfBulgeParms[i]);		
	}

	SafeDeleteArray(m_ppfBulgeParms);



}

void SDeformVertexesObject::AddMove(float x,float y,float z, UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq)
{
	assert(m_iMoveNum<DV_MOVE_MAX-1);
	

	m_ppMove[m_iMoveNum] = new CWave;
	m_ppMove[m_iMoveNum]->SetParams(uiWaveType,fBase,fAmplitude,fPhase,fFreq);

	m_MoveVert[m_iMoveNum] = new CVector3f(x,y,z);
	m_iMoveNum++;
}

void SDeformVertexesObject::AddWave(float fDiv, UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq)
{
	assert(m_iWaveNum<DV_WAVE_MAX-1);


	m_ppWave[m_iWaveNum] = new CWave;
	m_ppWave[m_iWaveNum]->SetParams(uiWaveType,fBase,fAmplitude,fPhase,fFreq);

	m_pWaveDivs[m_iWaveNum] = fDiv;
	m_iWaveNum++;
}

void SDeformVertexesObject::AddBulge(float f1,float f2,float f3)
{
	assert(m_iBulgeNum<DV_BULGE_MAX-1);

	m_ppfBulgeParms[m_iBulgeNum] = new float[3];

	m_ppfBulgeParms[m_iBulgeNum][0] = f1;
	m_ppfBulgeParms[m_iBulgeNum][1] = f2;
	m_ppfBulgeParms[m_iBulgeNum][2] = f3;

	m_iBulgeNum++;
}




/////////////////////////////////////
//
//  CShader class implementation
//
/////////////////////////////////////

CShader::CShader()
{
	
	m_dwShaderFlags = FLAG_CULLFRONT; // default flags
	m_dwSurfaceParm = SURFACEPARM_NONE;

	m_uiShaderSort=SORT_NOT_INIT; //default sort
	//m_cpShaderName="";  // RESTORE THIS RESTORE THIS
	m_cpShaderName = NULL;

	m_bAllBlended = false;

	m_bDVUsed = false;

	m_bFarBoxUsed=false;


	m_uiPassesNum = 0;
	m_ppPasses        = new CShaderPass*[MAX_PASS_COUNT]; // TODO: Maybe make this more dynamic?

	for (int i=0;i<MAX_PASS_COUNT;i++)
		m_ppPasses[i] = NULL;
}

CShader::~CShader()
{

	// DEBUG DEBUG DEBUG
	//m_cpShaderName = NULL;

	// DEBUG DEBUG DEBUG
	//m_ppPasses = NULL;
	//
	st_ShaderManager::Inst()->Unregister(this);
	//
	for (int i=0;i<m_uiPassesNum;i++)
		SafeDelete(m_ppPasses[i]);

	SafeDeleteArray(m_ppPasses);


	SafeDeleteArray(m_cpShaderName);
}


/*
CShaderPass* CShader::GetPass(UINT iPass)
{

	if (iPass>=MAX_PASS_COUNT) return NULL;
		return m_ppPasses[iPass]; 
}
*/


void CShader::AddPass(CShaderPass* pSP)
{	
	m_ppPasses[m_uiPassesNum++] = pSP;
}

void CShader::ChangePass(CShaderPass* pSP,int iPassNum)
{
	m_ppPasses[iPassNum] = pSP;
	if (iPassNum+1 > m_uiPassesNum)
		m_uiPassesNum = iPassNum+1;
}




void CShader::SetName(const char* cpName)
{
	m_cpShaderName = new char[strlen(cpName)+1];
	strcpy(m_cpShaderName,cpName);
}

char* CShader::GetName()
{
	return m_cpShaderName; 
}

void CShader::SetShaderFlags(DWORD dwShaderFlags)
{
	m_dwShaderFlags = dwShaderFlags;
}

DWORD CShader::GetShaderFlags(void)
{
	return m_dwShaderFlags;
}

void CShader::SetShaderSort(UINT uiShaderSort)
{
	m_uiShaderSort = uiShaderSort;
}

UINT CShader::GetShaderSort(void)
{
	return m_uiShaderSort;
}

void CShader::AddDeformVertexesMove(float x,float y,float z, UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq)
{
	m_bDVUsed=true;
	m_DVObject.AddMove(x,y,z,uiWaveType,fBase,fAmplitude,fPhase,fFreq);
}

void CShader::AddDeformVertexesBulge(float f1,float f2,float f3)
{
	m_bDVUsed=true;
	m_DVObject.AddBulge(f1,f2,f3);
}
void CShader::AddDeformVertexesWave(float fDiv, UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq)
{
	m_bDVUsed=true;
	m_DVObject.AddWave(fDiv,uiWaveType,fBase,fAmplitude,fPhase,fFreq);
}

void CShader::AddDeformVertexesAutoSprite(void)
{
	m_bDVUsed=true;
	m_DVObject.AddAutoSprite();
}




/*int CShader::GetPassCount()
{
	return m_uiPassesNum; 
}*/



////////////////////////////////////
// Shaders Manager
///////////////////////////////////

CShaderManager::CShaderManager()
{
	m_sppShaders = new CSmartPointer<CShader>[SHADER_MAN_MAX_SHADERS];

	m_uiLastShaderNum = 0;
}

CShaderManager::~CShaderManager()
{

	SafeDeleteArray(m_sppShaders);
}

bool CShaderManager::Unregister(CShader* pShader)
{
	for (int i=0;i<m_uiLastShaderNum+1;i++)
	{
		if ((CShader*)m_sppShaders[i]==pShader)
		{
			m_sppShaders[i]=NULL;
			return true;
		}
	}
	return false;
}


int CShaderManager::FindFirstFreeShader(void)
{
	for (int i=0;i<SHADER_MAN_MAX_SHADERS;i++)
		if ((CShader*)m_sppShaders[i]==NULL)
		{
			if (i>m_uiLastShaderNum)
				m_uiLastShaderNum=i;
			return i;
		}
	return NULL;
}

/*CShader* CShaderManager::GetShader(UINT uiShaderID)
{
	if (uiShaderID<m_uiShadersNum)
		return m_ppShaders[uiShaderID];
	else
		return m_ppShaders[0]; ////////// JUST FOR DEBUG
}*/


/////////////////////////////
// Findshader in m_ppShaders
// -1 if couldn't find
//////////////////////////////

int CShaderManager::FindShader(char* cpShaderName)
{
	bool bFound=false;
	UINT uiNum;

	for (int i=0;i<m_uiLastShaderNum+1;i++)
	{	

		if ((CShader*)m_sppShaders[i])
			if (strcmp(cpShaderName,m_sppShaders[i]->GetName())==0)
			{
				bFound=true;
				uiNum = (UINT) i;
				break;
			}
		if (bFound==true)
			break;
	}

	if (bFound==true)
		return (int) uiNum;
	else
		return -1;

}

int CShaderManager::GetShadersNum(void)
{
	int iShadersNum=0;
	for (int i=0;i<m_uiLastShaderNum+1;i++)
		if ((CShader*)m_sppShaders[i]!=NULL)
			iShadersNum++;

	return iShadersNum;
}

//////////////////////////////////////////////
// if doesn't exist already Push a new cshader
//////////////////////////////////////////////

/*UINT CShaderManager::Push(CShader* pPushMe)
{

	int AlreadyFound = FindShader((*pPushMe).GetName());

	if (AlreadyFound<0)
	{
		int i=FindFirstFreeShader();
		if (i==SHADER_MAN_MAX_SHADERS)
			return SHADER_MAN_MAX_SHADERS;

		m_ppShaders[i] = pPushMe;
		m_uiShadersNum++;
		return i;
	}
	else
	{
		return (UINT) AlreadyFound;
	}
}*/

////////////////////////////////////////////////////////
// force Push a new cshader (no check for similarity)
////////////////////////////////////////////////////////

/// TODO:CHANGE NAME!!!!!!!!!!!!!!!!!!!!!!!
CShader* CShaderManager::PushWithoutCheck(char* ShaderName)
{
		//CShader* pShader;
		int i=FindFirstFreeShader();
		
		//pShader=FindFirstFreeShader();	
		//pShader = g_ShaderParser.ParseShader(ShaderName);

		//((CShader*)m_sppShaders[i]) = g_ShaderParser.ParseShader(ShaderName);

		m_sppShaders[i] = st_ShaderParser::Inst()->ParseShader(ShaderName);

		
		//pShader = pPushMe;
		return ((CShader*)m_sppShaders[i]);
}


CShader* CShaderManager::Load(char* ShaderName)
{
	//int uiTempShaderIndex=0;
	CShader* pTempShader;

	/**/

	strlwr(ShaderName);

	/**/
	
	int AlreadyFound = FindShader(ShaderName);
	
	bool bFindInCache;

	bool bLoadedFromCache=false;
		
	if (AlreadyFound<0)
	{
		CShader* pShader=NULL;
		CShaderPass* pShaderPass=NULL;

				
		//pShader = g_ShaderParser.ParseShader(ShaderName);
		pTempShader =  PushWithoutCheck(ShaderName);
		
		//
		if (m_uiLastShaderNum<SHADER_MAN_MAX_SHADERS)
			m_uiLastShaderNum++;

		return pTempShader;

	}
	else
		return ((CShader*)m_sppShaders[AlreadyFound]);
}

