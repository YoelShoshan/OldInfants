#include "md3.h"

#include <windows.h>
#include "Defines.h"


#include "Shader.h"
#include "Renderer.h"


#include <assert.h>
#include <crtdbg.h>

#include "MD3MeshManager.h"
#include "MD3Manager.h"

#include "LogFile.h"
extern CLogFile g_LogFile;

extern int      g_iFrameNum;

// CMD3MeshBase

CMD3MeshBase::CMD3MeshBase()
{
	m_pMD3Skins	 = NULL;
	m_pMD3TexCoords = NULL;
	m_pMD3Faces	 = NULL;
	m_pMD3Verts = NULL;


	m_pOriginalVerts = NULL;
	m_pNormals = NULL;
	m_pMD3TexCoords = NULL;
	m_uiIndices = NULL;

	m_pcPlayerSkin=NULL;
	//m_pcPlayerSkinNormalMap=NULL;

	m_pcMD3Name = NULL;
	m_iMD3MeshNum = 0;
}
CMD3MeshBase::~CMD3MeshBase()
{	
	SafeDeleteArray(m_pMD3Skins);
	SafeDeleteArray(m_pMD3TexCoords);
	SafeDeleteArray(m_pMD3Faces);
	SafeDeleteArray(m_pMD3Verts);

	SafeDeleteArray(m_pcPlayerSkin);
	//SafeDeleteArray(m_pcPlayerSkinNormalMap);

	SafeDeleteArray(m_pOriginalVerts);
	SafeDeleteArray(m_pNormals);
	SafeDeleteArray(m_pMD3TexCoords);
	SafeDeleteArray(m_uiIndices);

	st_MD3MeshBaseManager::Inst()->Unregister(this); // unregister this texture from the tex manager (to invoke an auto delete)
}



void   CMD3Mesh::ComputeInterpolatedVerts(float fT,bool bChangingToAnotherAnimation)
{

	// in a case that there's no animation, there's no need to copmute interpolated values

	// todo: perhaps for the head animation i DO need vertex morphing??
	if (m_pSharedMesh->m_iMeshFramesNum<=1 || !m_pNextFrameVerts || !m_pCurrentFrameVerts)
	{
		m_pCurrentInterpolatedVerts = m_pCurrentFrameVerts;
		
	}
	else // we DO have an animation to handle
	{


//		ComputeInterpolatedVerts

		if (bChangingToAnotherAnimation)
		{
			for (int i=0;i<m_pSharedMesh->m_iVerticesNum;i++)
			{
				m_pCurrentInterpolatedVerts[i] = m_pBackupVerts[i] + (m_pCurrentFrameVerts[i] - m_pBackupVerts[i])*fT;
			}
		}
		else
		{
			for (int i=0;i<m_pSharedMesh->m_iVerticesNum;i++)
			{
				m_pCurrentInterpolatedVerts[i] = m_pCurrentFrameVerts[i] + (m_pNextFrameVerts[i] - m_pCurrentFrameVerts[i])*fT;
			}
		}
	}

	m_VB.Set(VB_HAS_VERTS,m_pCurrentInterpolatedVerts);
}









CMD3Mesh::CMD3Mesh()
{

	

	m_pCurrentInterpolatedVerts = NULL;
	m_pCurrentFrameVerts = NULL;
	m_pNextFrameVerts = NULL;

	m_pSharedMesh = NULL;


	/**/

	/*m_pSharedMesh = new CMD3MeshBase;*/
	m_pSharedMesh = NULL;

	/**/

	m_VB.Set(VB_HAS_VERTS,NULL);
	m_VB.Set(VB_HAS_NORMALS,NULL);
	m_VB.SetSize(0);
	m_VB.Set(VB_HAS_TEXCOORDS,NULL);
	//m_VB[i].SetIndexCount(0);
	m_VB.SetIndices(NULL);
}
CMD3Mesh::~CMD3Mesh()
{
	m_VB.SetIndices(NULL);
	m_VB.SetAllNull();


	
}


CMD3Shared::CMD3Shared()
{
	m_pTags = NULL;
	m_iNumOfAnimations = 0;

	
	m_iMeshesNum = 0;

	m_pcMD3Name = NULL;

	m_iNumOfTags = 0; // number of tags in the model

	

	

	m_cpFileName = NULL;

	m_pBones = NULL;


}


CMD3Shared::~CMD3Shared()
{
	SafeDeleteArray(m_pTags);
	SafeDeleteArray(m_pBones);

	SafeDeleteArray(m_pTags);	
	
	st_MD3BaseManager::Inst()->Unregister(this); // unregister this texture from the tex manager (to invoke an auto delete)
}






CMD3::CMD3()
{	
	m_pFile = NULL;
	
	m_pLinks = NULL;
	

	m_iCurrentDrawnFrame = 0;
	
	m_bChangingAni = false;
	m_fChangeAniT = 0.f;


	
	
	m_pMeshes = NULL;


	
	m_iCurrentAnim = 0;			
	m_iCurrentFrame = 0;		
	m_iNextFrame = 1;			
	m_fT = 0.f;					
	m_fLastTime;			


	/*m_pMD3Shared = new CMD3Shared;*/
	m_pMD3Shared = NULL;

}

CMD3::~CMD3()
{
	SafeDeleteArray(m_pMeshes);	
}


int CMD3::GetAnimationByID(int iNum)
{
	int iReturn = 0;

	for (int i=0;i<m_pMD3Shared->m_iNumOfAnimations;i++)
	{
		if (m_pMD3Shared->animations[i].iAnimationID==iNum)
			return iReturn;

		iReturn++;
	}
	return -1;
}

/*void CMD3::IfAnimationDoneGoToAnimationID(int iNum)
{
	if (m_bAnimationEndedInThisFrame)
	{
		int iAbsoluteIndex = GetAnimationByID(iNum);

		m_iCurrentAnim = iAbsoluteIndex;

		m_iCurrentFrame = animations[m_iCurrentAnim].startFrame;
		m_iNextFrame = animations[m_iCurrentAnim].startFrame+1;

	}

}*/



void CMD3::SetAlwaysDraw(bool bSet)
{
	for (int m=0;m<m_pMD3Shared->m_iMeshesNum;m++) // for each model
		{        
			m_pMeshes[m].m_VB.m_bAlwaysDraw = bSet;
		}
}

void CMD3::SetIgnoreCameraMatrix(bool bSet)
{
	for (int m=0;m<m_pMD3Shared->m_iMeshesNum;m++) // for each model
		{        
			m_pMeshes[m].m_VB.m_bIgnoreCameraMatrix = bSet;
		}
}

void CMD3::ForceAnimByGeneralAnimationName(int iNum)
{
	m_iCurrentAnim = GetAnimationByID(iNum);
	m_iCurrentFrame = m_pMD3Shared->animations[m_iCurrentAnim].startFrame;
	m_iNextFrame = m_pMD3Shared->animations[m_iCurrentAnim].startFrame + 1;
	m_fT=0.f; //

	
}


void CMD3::ForceAnimByLocalArrayIndex(int iNum)
{
	m_iCurrentAnim = iNum;
	m_iCurrentFrame = m_pMD3Shared->animations[m_iCurrentAnim].startFrame;
	m_iNextFrame = m_pMD3Shared->animations[m_iCurrentAnim].startFrame + 1;
	m_fT=0.f; // ??
	

}

//#define MOVE_FROM_ANIMATION_TO_ANOTHER_STEPS 20.f
//#define MOVE_FROM_ANIMATION_TO_ANOTHER_STEPS 10.f
#define MOVE_FROM_ANIMATION_TO_ANOTHER_STEPS 5.f

void CMD3::ComputeMeshesInterpolatedVertices(float fT)
{
	
	// if moving from one animation to another
	if (m_bChangingAni)
	{
		m_fChangeAniT+=st_Timer::Inst()->GetDeltaTimeFAKE() / (1000.0f / MOVE_FROM_ANIMATION_TO_ANOTHER_STEPS);
		if (m_fChangeAniT>=1.f)
		{
			m_fChangeAniT=0.f;
			m_bChangingAni=false;
		}
	}

	for (int i=0;i<m_pMD3Shared->m_iMeshesNum;i++)
	{
		if (m_bChangingAni) // moving from one animation to another
			m_pMeshes[i].ComputeInterpolatedVerts(m_fChangeAniT,true);
		else // inside animation
			m_pMeshes[i].ComputeInterpolatedVerts(fT,false);
	}

}

void CMD3::UpdateAnimation(void)
{
	if (m_pMD3Shared->m_iNumOfAnimations<1)
	{
		m_iCurrentFrame = m_pMD3Shared->animations[m_iCurrentAnim].startFrame;
		m_iNextFrame = m_pMD3Shared->animations[m_iCurrentAnim].endFrame;
		m_bAnimationEndedInThisFrame=true;
		return;
	}


	//if only one frame - should be static
	if (m_pMD3Shared->animations[m_iCurrentAnim].endFrame - m_pMD3Shared->animations[m_iCurrentAnim].startFrame == 1)
	{
		m_fT = 0.f;
		m_iCurrentFrame = m_pMD3Shared->animations[m_iCurrentAnim].startFrame;
		m_iNextFrame = m_pMD3Shared->animations[m_iCurrentAnim].endFrame; // should i do this?

		// RESTORE RESTORE RESTORE
		//m_bAnimationEndedInThisFrame=false;

		// DEBUG DEBUG DEBUG
		m_bAnimationEndedInThisFrame=true;
		return;
	}



	m_bAnimationEndedInThisFrame=false;


	float  fAnimationSpeed = m_pMD3Shared->animations[m_iCurrentAnim].framesPerSecond;

	//fAnimationSpeed/=10.f;
	//float fAnimationSpeed = 4.f;
	//float fAnimationSpeed = 1.f;

	float fT = st_Timer::Inst()->GetDeltaTimeFAKE() / (1000.0f / fAnimationSpeed);
		

	m_fT += fT;

	if (m_fT >= 1.f )
	{

		if (m_pMD3Shared->animations[m_iCurrentAnim].bShouldFreezeAtEnd && m_iCurrentFrame == m_pMD3Shared->animations[m_iCurrentAnim].endFrame-1)
		//if (animations[m_iCurrentAnim].bShouldFreezeAtEnd && m_iCurrentFrame == animations[m_iCurrentAnim].startFrame + animations[m_iCurrentAnim].loopingFrames-1)
		{
			m_fT=1.f;
			return;
		}

		m_iCurrentFrame++; 
		m_iNextFrame = m_iCurrentFrame+1;
		if (m_iNextFrame > m_pMD3Shared->animations[m_iCurrentAnim].endFrame-1)
		//if (m_iNextFrame > animations[m_iCurrentAnim].startFrame + animations[m_iCurrentAnim].loopingFrames-1)
		{
			if (!m_pMD3Shared->animations[m_iCurrentAnim].bIsRepetetive)
			{
				m_bAnimationEndedInThisFrame=true;
			}

			m_iNextFrame = m_pMD3Shared->animations[m_iCurrentAnim].startFrame;
		}

		if (m_iCurrentFrame > m_pMD3Shared->animations[m_iCurrentAnim].endFrame-1) // animation ended
		//if (m_iCurrentFrame > animations[m_iCurrentAnim].startFrame + animations[m_iCurrentAnim].loopingFrames-1) // animation ended
		{
			m_iCurrentFrame = m_pMD3Shared->animations[m_iCurrentAnim].startFrame;
			m_iNextFrame = m_pMD3Shared->animations[m_iCurrentAnim].startFrame+1;
		}


		/*if (m_iCurrentFrame > animations[m_iCurrentAnim].endFrame)
			m_iCurrentFrame = animations[m_iCurrentAnim].startFrame;*/
		

		m_fT = 0.f;
	}
	
	


}

void CMD3::SetMeshesCurrentFramePointers(int iFrameNum)
{
	for (int m=0;m<m_pMD3Shared->m_iMeshesNum;m++) // for each model
		{   

			//m_iCurrentDrawnFrame = iFrameNum;

			if (iFrameNum<m_pMeshes[m].m_pSharedMesh->m_iMeshFramesNum)
			{
				
				// original
				//m_pMeshes[m].m_pCurrentFrameVerts = &(m_pMeshes[m].m_pSharedMesh->m_pOriginalVerts[m_pMeshes[m].m_pSharedMesh->m_iVerticesNum * iFrameNum]);
				
				m_pMeshes[m].m_pCurrentFrameVerts = &(m_pMeshes[m].m_pSharedMesh->m_pOriginalVerts[m_pMeshes[m].m_pSharedMesh->m_iVerticesNum * iFrameNum]);
				

			}	
		}
}


void CMD3::SetMeshesNextFramePointers(int iFrameNum)
{
	for (int m=0;m<m_pMD3Shared->m_iMeshesNum;m++) // for each model
		{   

			//m_iCurrentDrawnFrame = iFrameNum;

			if (iFrameNum<m_pMeshes[m].m_pSharedMesh->m_iMeshFramesNum)
			{
				

				m_pMeshes[m].m_pNextFrameVerts = &(m_pMeshes[m].m_pSharedMesh->m_pOriginalVerts[m_pMeshes[m].m_pSharedMesh->m_iVerticesNum * iFrameNum]);
				//m_pMeshes[m].m_VB.Set(VB_HAS_VERTS,m_pMeshes[m].m_pCurrentFrameVerts);

			}	
		}
}

void CMD3::Update(bool bSpecialWorld,bool bIgnoreCameraMatrix,float* pfWorldMatrix)
{	
    if (bSpecialWorld)
	{
		for (int m=0;m<m_pMD3Shared->m_iMeshesNum;m++) // for each model
		{

			///

			m_pMeshes[m].m_VB.m_bSpecialWorldMatrix = true;		
			
			m_pMeshes[m].m_VB.m_bIgnoreCameraMatrix = bIgnoreCameraMatrix;

			if (pfWorldMatrix)
				m_pMeshes[m].m_VB.FillSpecialWorldMatrix(pfWorldMatrix);
			else
				assert(1);

			///
			     
		}
	}

	UpdateAnimation();
}

void CMD3::PushToRenderer(bool bPPLused)
{
	for (int m=0;m<m_pMD3Shared->m_iMeshesNum;m++) // for each model
	{		
		// temp
		if (bPPLused)
			m_pMeshes[m].m_VB.m_bUsingPPL = true;
		else
			m_pMeshes[m].m_VB.m_bUsingPPL = false;

		st_Renderer::Inst()->Push(&m_pMeshes[m].m_VB);		
	}		
}

/*void CMD3::PushToRendererForceShader(char *cpShaderName)
{
	CShader* pShader = st_ShaderManager::Inst()->Load(cpShaderName);

	for (int m=0;m<m_pMD3Shared->m_iMeshesNum;m++) // for each model
	{
		m_pMeshes[m].m_VB.SetShaderSP(pShader);
		st_Renderer::Inst()->Push(&m_pMeshes[m].m_VB);		
	}		
}*/

void CMD3::SetSubMeshesColorArrays(byte r, byte g, byte b, byte a)
{
	for (int m=0;m<m_pMD3Shared->m_iMeshesNum;m++) // for each model
	{
		for (int j=0;j<m_pMeshes[m].m_pSharedMesh->m_iVerticesNum;j++)
		{
			m_pMeshes[m].m_pColors[j].c[0] = r;
			m_pMeshes[m].m_pColors[j].c[1] = g;
			m_pMeshes[m].m_pColors[j].c[2] = b;
			m_pMeshes[m].m_pColors[j].c[3] = a;
		}
	}


}

void CMD3::SetSpecialCounter(void)
{
	for (int m=0;m<m_pMD3Shared->m_iMeshesNum;m++) // for each model
	{
		m_pMeshes[m].m_VB.SetSpecialCounter();
	}

}

void CMD3::DisableSpecialCounter(void)
{
	for (int m=0;m<m_pMD3Shared->m_iMeshesNum;m++) // for each model
	{
		m_pMeshes[m].m_VB.DisableSpecialCounter();
	}

}

#define MAX_SKIN_NAME_SIZE 128


bool CMD3::ExistInManager(const char* cpFileName)
{

	int iAlreadyInMD3MeshManager=0;

	int iAlreadyInTheMD3Manager = st_MD3BaseManagerI->AlreadyInManager(cpFileName);

	if (iAlreadyInTheMD3Manager<0) // not in the md3 manager
		return false;

	return true;

}

bool CMD3::Load(const char *cpFileName,const char* skin, const char* texture,bool bPPL)
{

	//assert(_CrtCheckMemory());

	////////////////////////////////////////////////////////////////////////
	// check if they already exist in the manager	

	char* pFileName;

	if (texture) // a specified texture
	{
		pFileName = new char[strlen(cpFileName)+1];
		strcpy(pFileName,cpFileName);

		m_cpFileName = new char[strlen(cpFileName)+strlen(texture)+1];
		strcpy(m_cpFileName,cpFileName);

		for (int i=0;i<strlen(texture)+1;i++)
		{
			m_cpFileName[strlen(cpFileName)+i] = texture[i];
		}

	}
	else
	{
		pFileName = new char[strlen(cpFileName)+1];
		strcpy(pFileName,cpFileName);

		m_cpFileName = new char[strlen(cpFileName)+1];
		strcpy(m_cpFileName,cpFileName);
	}

	//assert(_CrtCheckMemory());

	
	int iAlreadyInMD3MeshManager=0;

	int iAlreadyInTheMD3Manager = st_MD3BaseManagerI->AlreadyInManager(m_cpFileName);

	if (iAlreadyInTheMD3Manager<0) // not in the md3 manager
	{

		m_pMD3Shared = st_MD3BaseManager::Inst()->AllocFirstFreeModel();

		m_pMD3Shared->m_pcMD3Name = m_cpFileName;
	} // end of if iAlreadyInMD3Manager
	else // is in manager
	{
		m_pMD3Shared = st_MD3BaseManager::Inst()->GetModel(m_cpFileName);
	}



		iAlreadyInMD3MeshManager = st_MD3MeshBaseManager::Inst()->AlreadyInManager(m_cpFileName);

		if (iAlreadyInMD3MeshManager>=0)
		{
			int iNumOfSubMeshes = st_MD3MeshBaseManager::Inst()->GetNumOfSubMeshes(m_cpFileName);//

			m_pMeshes = new CMD3Mesh [ iNumOfSubMeshes ];

			for (int i = 0; i < iNumOfSubMeshes; i++)
				{
					m_pMeshes[i].m_pSharedMesh = st_MD3MeshBaseManager::Inst()->GetModel(m_cpFileName,i);
				}
		}
		
		//assert(_CrtCheckMemory());


	if (iAlreadyInTheMD3Manager<0) // not in the md3 manager
	{

		////////////////////////////////////////////////////////////////////////
	
		// if we got to here it means they aren't found in the manager,
		// and we need to actually load the MD3 from the file.

		// Open the MD3 file in binary
		m_pFile = fopen(cpFileName, "rb");

		// display an error message if can't find the file
		if(!m_pFile) 
		{
			char* strMessage = new char[300];
			sprintf(strMessage, "Unable to find the file: %s!", cpFileName);
			MessageBox(NULL, strMessage, "Error", MB_OK);
			SafeDeleteArray(strMessage);
			return false;
		}

		//cpFileName
		

		tMd3Header header;

		// Read the header data and store it in our m_Header member variable
		fread(&header, 1, sizeof(tMd3Header), m_pFile);


		//assert(_CrtCheckMemory());

		// Get the 4 character ID
		char *ID = header.fileID;

		if((ID[0] != 'I' || ID[1] != 'D' || ID[2] != 'P' || ID[3] != '3') || header.version != 15)
		{
			char* strMessage = new char[300];
			sprintf(strMessage, "Invalid file format (Version not 15): %s!", cpFileName);
			MessageBox(NULL, strMessage, "Error", MB_OK);
			SafeDeleteArray(strMessage);
			return false;
		}



		// load the bones
		m_pMD3Shared->m_pBones = new tMd3Bone [header.numFrames];
		fread(m_pMD3Shared->m_pBones, sizeof(tMd3Bone), header.numFrames, m_pFile);


		// load the tags
		m_pMD3Shared->m_pTags = new tMd3Tag [header.numFrames * header.numTags];
		fread(m_pMD3Shared->m_pTags, sizeof(tMd3Tag), header.numFrames * header.numTags, m_pFile);


		m_pMD3Shared->m_iNumOfTags = header.numTags;



		/*if (m_pMD3Shared->m_iNumOfTags>0)
		{
			m_pLinks = new CMD3*[m_pMD3Shared->m_iNumOfTags];
			for (int l=0;l<m_pMD3Shared->m_iNumOfTags;l++)
				m_pLinks[l]=NULL;
		}
		else
			m_pLinks=NULL;*/

			


		//assert(_CrtCheckMemory());


		// Get the current offset into the file
		long meshOffset = ftell(m_pFile);

		m_pMD3Shared->m_iMeshesNum = header.numMeshes;

		if (iAlreadyInMD3MeshManager<0) // if we didn't already load it from the manager
			m_pMeshes = new CMD3Mesh [ m_pMD3Shared->m_iMeshesNum ];

		// Create a local meshHeader that stores the info about the mesh
		tMd3MeshInfo meshHeader;

	
		//assert(_CrtCheckMemory());

		// Go through all of the sub-objects in this mesh
		if (iAlreadyInMD3MeshManager<0) // if we didn't already load it from the manager
		for (int i = 0; i < header.numMeshes; i++)
		{
			// Seek to the start of this mesh and read in it's header
			fseek(m_pFile, meshOffset, SEEK_SET);
			fread(&meshHeader, sizeof(tMd3MeshInfo), 1, m_pFile);

			m_pMeshes[i].m_pSharedMesh = st_MD3MeshBaseManager::Inst()->AllocFirstFreeModel();

			//m_pMeshes[i].m_pSharedMesh->m_pcMD3Name = pFileName;
			m_pMeshes[i].m_pSharedMesh->m_pcMD3Name = m_cpFileName;
			m_pMeshes[i].m_pSharedMesh->m_iMD3MeshNum = i;

		

			m_pMeshes[i].m_pSharedMesh->m_iSkinsNum    = meshHeader.numSkins;		
			m_pMeshes[i].m_pSharedMesh->m_iFacesNum    =  meshHeader.numTriangles;
			m_pMeshes[i].m_pSharedMesh->m_iVerticesNum = meshHeader.numVertices;
			m_pMeshes[i].m_pSharedMesh->m_iTexCoordNum = meshHeader.numVertices;

			// Here we allocate all of our memory from the header's information
			m_pMeshes[i].m_pSharedMesh->m_pMD3Skins     = new tMd3Skin [meshHeader.numSkins];
			m_pMeshes[i].m_pSharedMesh->m_pMD3TexCoords = new tMd3TexCoord [meshHeader.numVertices];
			m_pMeshes[i].m_pSharedMesh->m_pMD3Faces = new tMd3Face [meshHeader.numTriangles];
			m_pMeshes[i].m_pSharedMesh->m_pMD3Verts  = new tMd3Vert [meshHeader.numVertices * meshHeader.numMeshFrames];

			m_pMeshes[i].m_pSharedMesh->m_iMeshFramesNum = meshHeader.numMeshFrames;

			// Read in the skin information
			fread(m_pMeshes[i].m_pSharedMesh->m_pMD3Skins, sizeof(tMd3Skin), meshHeader.numSkins, m_pFile);
		
			// Seek to the start of the triangle/face data, then read it in
			fseek(m_pFile, meshOffset + meshHeader.triStart, SEEK_SET);
			fread(m_pMeshes[i].m_pSharedMesh->m_pMD3Faces, sizeof(tMd3Face), meshHeader.numTriangles, m_pFile);

			// Seek to the start of the UV coordinate data, then read it in
			fseek(m_pFile, meshOffset + meshHeader.uvStart, SEEK_SET);
			fread(m_pMeshes[i].m_pSharedMesh->m_pMD3TexCoords, sizeof(tMd3TexCoord), meshHeader.numVertices, m_pFile);

			// Seek to the start of the vertex/face index information, then read it in.
			fseek(m_pFile, meshOffset + meshHeader.vertexStart, SEEK_SET);
			fread(m_pMeshes[i].m_pSharedMesh->m_pMD3Verts, sizeof(tMd3Vert), meshHeader.numMeshFrames * meshHeader.numVertices, m_pFile);


			// Increase the offset into the file
			meshOffset += meshHeader.meshSize;
		}

	
		fclose(m_pFile);
	

	} // end of if iAlreadyInMD3Manager


	m_pLinks = new CMD3*[m_pMD3Shared->m_iNumOfTags];
			for (int l=0;l<m_pMD3Shared->m_iNumOfTags;l++)
				m_pLinks[l]=NULL;



	//assert(_CrtCheckMemory());

	if (iAlreadyInMD3MeshManager<0) // if we didn't already load it from the manager
	{

		if (skin)
			LoadSkin(skin,bPPL);
		else
		if (texture)
		{
			for (int i=0;i<m_pMD3Shared->m_iMeshesNum;i++)
			{
				//here here here

				m_pMeshes[i].m_pSharedMesh->m_pcPlayerSkin = new char[MAX_SKIN_NAME_SIZE];
				strcpy(m_pMeshes[i].m_pSharedMesh->m_pcPlayerSkin,texture);
				
			}
		}
	}

	//assert(_CrtCheckMemory());
	


	ConvertToVertexBuffers(iAlreadyInMD3MeshManager,bPPL);


	SetMeshesCurrentFramePointers(0);



	return true;
}



bool CMD3::LoadSkin(const char* cpFileName,bool bPPL)
{
	std::ifstream skinFile;

	std::string t_CurrentLine;

	skinFile.open(cpFileName);

	int iLineNum=0;
	int iSubMeshesNum = 0;

	char *pcTemp1,*pcTemp2;

	if(skinFile.fail())
			{	
				assert(1);
				// LOG: log here
				return false;										
			}


	while(std::getline(skinFile,t_CurrentLine))
		{	
		iLineNum++;

		if (t_CurrentLine[0]=='/')  // also checking before because of that old bug...
		continue;

		if (strstr(t_CurrentLine.c_str(),"tag_")==NULL) // if this is NOT a tag
		{

			//here here 

			m_pMeshes[iSubMeshesNum].m_pSharedMesh->m_pcPlayerSkin = new char[MAX_SKIN_NAME_SIZE];

			pcTemp1 = (char * ) strstr(t_CurrentLine.c_str(),",");
			pcTemp1 = &pcTemp1[1];

			pcTemp2 = strstr(pcTemp1,"//");

			if (pcTemp2)
				pcTemp2[0]=NULL;
			
			strcpy(m_pMeshes[iSubMeshesNum].m_pSharedMesh->m_pcPlayerSkin,pcTemp1);


			//hjkhjkhjkhkjh

			/*m_pMeshes[iSubMeshesNum].m_pSharedMesh->m_pcPlayerSkinNormalMap = new char[MAX_SKIN_NAME_SIZE+6];


			strcpy(m_pMeshes[iSubMeshesNum].m_pSharedMesh->m_pcPlayerSkinNormalMap,pcTemp1);

			int iOriginalLength = strlen(pcTemp1);

			m_pMeshes[iSubMeshesNum].m_pSharedMesh->m_pcPlayerSkinNormalMap[iOriginalLength-4] = '_';
			m_pMeshes[iSubMeshesNum].m_pSharedMesh->m_pcPlayerSkinNormalMap[iOriginalLength-3] = 'l';
			m_pMeshes[iSubMeshesNum].m_pSharedMesh->m_pcPlayerSkinNormalMap[iOriginalLength-2] = 'o';
			m_pMeshes[iSubMeshesNum].m_pSharedMesh->m_pcPlayerSkinNormalMap[iOriginalLength-1] = 'c';
			m_pMeshes[iSubMeshesNum].m_pSharedMesh->m_pcPlayerSkinNormalMap[iOriginalLength] = 'a';
			m_pMeshes[iSubMeshesNum].m_pSharedMesh->m_pcPlayerSkinNormalMap[iOriginalLength+1] = 'l';

			m_pMeshes[iSubMeshesNum].m_pSharedMesh->m_pcPlayerSkinNormalMap[iOriginalLength+2] = pcTemp1[iOriginalLength-4];
			m_pMeshes[iSubMeshesNum].m_pSharedMesh->m_pcPlayerSkinNormalMap[iOriginalLength+3] = pcTemp1[iOriginalLength-3];
			m_pMeshes[iSubMeshesNum].m_pSharedMesh->m_pcPlayerSkinNormalMap[iOriginalLength+4] = pcTemp1[iOriginalLength-2];
			m_pMeshes[iSubMeshesNum].m_pSharedMesh->m_pcPlayerSkinNormalMap[iOriginalLength+5] = pcTemp1[iOriginalLength-1];
			
			m_pMeshes[iSubMeshesNum].m_pSharedMesh->m_pcPlayerSkinNormalMap[iOriginalLength+6] = NULL;



			// DEBUG DEBUG DEBUG

			strcpy(pcTemp1,m_pMeshes[iSubMeshesNum].m_pSharedMesh->m_pcPlayerSkinNormalMap);*/


			iSubMeshesNum++;
		}
		
		}

		skinFile.close();

	return true;
}

void CMD3::ConvertToVertexBuffers(int iInManager,bool bPPL)
{
	char* pDebugString = new char[400];

	CShader* pShader = NULL;
	CShaderPass* pShaderPass = NULL;

	CShader* pTempShader;

	//assert(_CrtCheckMemory());




	for (int m=0;m<m_pMD3Shared->m_iMeshesNum;m++) // for each model
	{
			

		int i = m_pMeshes[m].m_pSharedMesh->m_iFacesNum;

		char *pcTempShaderName = new char[200];

		char *pcTempShaderNameNormalMap = new char[200];


		//assert(_CrtCheckMemory());

		if (m_pMeshes[m].m_pSharedMesh->m_pcPlayerSkin!=NULL)
		{
			// magdalena head
			strcpy(pcTempShaderName,m_pMeshes[m].m_pSharedMesh->m_pcPlayerSkin);

			/*if (m_pMeshes[m].m_pSharedMesh->m_pcPlayerSkinNormalMap!=NULL)
				strcpy(pcTempShaderNameNormalMap,m_pMeshes[m].m_pSharedMesh->m_pcPlayerSkinNormalMap);	*/

		}
		else
		{
	
		// RESTORE RESTORE RESTORE 
		strcpy(pcTempShaderName,m_pMeshes[m].m_pSharedMesh->m_pMD3Skins[0].strName);

					
		}


		/*if (strstr(pcTempShaderName,"models/weaphits/newexplo1"))
			strcpy(pcTempShaderName,"rocketExplosion");*/

		
		
		// debug
			//strcpy(pcTempShaderName,"bloodExplosion");
			//strcpy(pcTempShaderName,"plasmaExplosion");
			
			

		//assert(_CrtCheckMemory());

		strlwr(pcTempShaderName);
		

		// original
		if ( strstr(pcTempShaderName,".jpg")!=NULL || strstr(pcTempShaderName,".tga")!=NULL || strstr(pcTempShaderName,".bmp")!=NULL)
			pcTempShaderName[ strlen(pcTempShaderName)-4 ] =NULL;

				

		


		//assert(_CrtCheckMemory());

		/*sprintf(pDebugString,"Model:%d - Texture:%s - Vertices:%d:",m,pcTempShaderName,m_pMeshes[m].m_iVerticesNum);
		g_LogFile.OutPutPlainText(pDebugString,"3","009900",true);*/


		m_pMeshes[m].m_pBackupVerts   = new CVector3f[m_pMeshes[m].m_pSharedMesh->m_iVerticesNum];
		m_pMeshes[m].m_pCurrentInterpolatedVerts = new CVector3f[m_pMeshes[m].m_pSharedMesh->m_iVerticesNum];

		if (iInManager<0)
		{
			m_pMeshes[m].m_pSharedMesh->m_pOriginalVerts = new CVector3f[m_pMeshes[m].m_pSharedMesh->m_iVerticesNum * m_pMeshes[m].m_pSharedMesh->m_iMeshFramesNum];
			

		
			for (int v=0;v<m_pMeshes[m].m_pSharedMesh->m_iVerticesNum* m_pMeshes[m].m_pSharedMesh->m_iMeshFramesNum;v++)
			{
				
				
				if (strcmp(m_cpFileName,"models/weaphits/boom01.md3gfx/damage/bullet_mrk")==0)
				{
					m_pMeshes[m].m_pSharedMesh->m_pOriginalVerts[v].v[0] = ((float) m_pMeshes[m].m_pSharedMesh->m_pMD3Verts[v].position[0])/512.f;
					m_pMeshes[m].m_pSharedMesh->m_pOriginalVerts[v].v[1] = ((float) m_pMeshes[m].m_pSharedMesh->m_pMD3Verts[v].position[1])/512.f;
					m_pMeshes[m].m_pSharedMesh->m_pOriginalVerts[v].v[2] = ((float) m_pMeshes[m].m_pSharedMesh->m_pMD3Verts[v].position[2])/512.f;
					
				}
				else				
				if ( (strcmp(m_cpFileName,"models/weaphits/boom01.md3gfx/damage/plasma_mrk")==0) ||
					(strcmp(m_cpFileName,"models/weaphits/boom01.md3gfx/damage/hole_lg_mrk")==0)	)
				{
					m_pMeshes[m].m_pSharedMesh->m_pOriginalVerts[v].v[0] = ((float) m_pMeshes[m].m_pSharedMesh->m_pMD3Verts[v].position[0])/256.f;
					m_pMeshes[m].m_pSharedMesh->m_pOriginalVerts[v].v[1] = ((float) m_pMeshes[m].m_pSharedMesh->m_pMD3Verts[v].position[1])/256.f;
					m_pMeshes[m].m_pSharedMesh->m_pOriginalVerts[v].v[2] = ((float) m_pMeshes[m].m_pSharedMesh->m_pMD3Verts[v].position[2])/256.f;
					
				}
				else
				if (strcmp(m_cpFileName,"models/weaphits/boom01.md3rocketExplosion")==0)
				{
					m_pMeshes[m].m_pSharedMesh->m_pOriginalVerts[v].v[0] = ((float) m_pMeshes[m].m_pSharedMesh->m_pMD3Verts[v].position[0])/128.f;
					m_pMeshes[m].m_pSharedMesh->m_pOriginalVerts[v].v[1] = ((float) m_pMeshes[m].m_pSharedMesh->m_pMD3Verts[v].position[1])/128.f;
					m_pMeshes[m].m_pSharedMesh->m_pOriginalVerts[v].v[2] = ((float) m_pMeshes[m].m_pSharedMesh->m_pMD3Verts[v].position[2])/128.f;
					
				}
				else
				{
					m_pMeshes[m].m_pSharedMesh->m_pOriginalVerts[v].v[0] = ((float) m_pMeshes[m].m_pSharedMesh->m_pMD3Verts[v].position[0])/64.f;
					m_pMeshes[m].m_pSharedMesh->m_pOriginalVerts[v].v[1] = ((float) m_pMeshes[m].m_pSharedMesh->m_pMD3Verts[v].position[1])/64.f;
					m_pMeshes[m].m_pSharedMesh->m_pOriginalVerts[v].v[2] = ((float) m_pMeshes[m].m_pSharedMesh->m_pMD3Verts[v].position[2])/64.f;
					

				}

				//m_pMeshes[m].m_pUpdatedVerts[v] = m_pMeshes[m].m_pOriginalVerts[v];

				/*sprintf(pDebugString,"Position:%.3f,%.3f,%.3f.",m_pMeshes[m].m_pOriginalVerts[v].v[0],
						m_pMeshes[m].m_pOriginalVerts[v].v[1],m_pMeshes[m].m_pOriginalVerts[v].v[2]);
				g_LogFile.OutPutPlainText(pDebugString,"3","000099",true);*/
			}
		}


		m_pMeshes[m].m_pColors = new CColor4[m_pMeshes[m].m_pSharedMesh->m_iVerticesNum];
						
			for (int c=0;c<m_pMeshes[m].m_pSharedMesh->m_iVerticesNum;c++)
			{
				m_pMeshes[m].m_pColors[c].c[0] = 255;
				m_pMeshes[m].m_pColors[c].c[1] = 255;
				m_pMeshes[m].m_pColors[c].c[2] = 255;
				//m_pMeshes[m].m_pColors[c].c[3] = 255;
				m_pMeshes[m].m_pColors[c].c[3] = 125;
			}


		// decode and convert the normals to my format
		//(Code in q3tools/q3map/misc_model.c:InsertMD3Model) 
		//lat <- (normal binary-and 255) * (2 * pi) / 255
		//lng <- ((normal shift-right 8) binary-and 255) * (2 * pi ) / 255
		//x <- cos ( lat ) * sin ( lng )
		//y <- sin ( lat ) * sin ( lng )
		//z <- cos ( lng )

		float lat,lng;
        
		
		
		//assert(_CrtCheckMemory());

		if (iInManager<0)
		{
			m_pMeshes[m].m_pSharedMesh->m_pNormals = new CVector3f[m_pMeshes[m].m_pSharedMesh->m_iVerticesNum];


			for (int v=0;v<m_pMeshes[m].m_pSharedMesh->m_iVerticesNum;v++)
			{
				lat = ((float)(m_pMeshes[m].m_pSharedMesh->m_pMD3Verts[v].normal & 255)) * (2 * PI) / 255.f;
				lng = ((float) ((m_pMeshes[m].m_pSharedMesh->m_pMD3Verts[v].normal >> 8) & 255)) * (2 * PI) / 255.f;

				m_pMeshes[m].m_pSharedMesh->m_pNormals[v].v[0] = cosf(lat) * sinf(lng);
				m_pMeshes[m].m_pSharedMesh->m_pNormals[v].v[1] = cosf(lng);
				m_pMeshes[m].m_pSharedMesh->m_pNormals[v].v[2] = -sinf(lat) * sinf(lng);
			}
		


			// convert the texcoords to my format
			//CVector2f* pTexCoords = new CVector2f[m_pMeshes[m].m_pSharedMesh->m_iVerticesNum];

			m_pMeshes[m].m_pSharedMesh->m_pTexCoords = new CVector2f[m_pMeshes[m].m_pSharedMesh->m_iVerticesNum];

			for (int v=0;v<m_pMeshes[m].m_pSharedMesh->m_iVerticesNum;v++)
			{
				m_pMeshes[m].m_pSharedMesh->m_pTexCoords[v].v[0] = ((float) m_pMeshes[m].m_pSharedMesh->m_pMD3TexCoords[v].textureCoord[0]);
				m_pMeshes[m].m_pSharedMesh->m_pTexCoords[v].v[1] = ((float) m_pMeshes[m].m_pSharedMesh->m_pMD3TexCoords[v].textureCoord[1]);
			}


			m_pMeshes[m].m_pSharedMesh->m_pTangents = (CVector3f*) m_pMeshes[m].m_VB.Get(VB_HAS_TANGENTS);
			m_pMeshes[m].m_pSharedMesh->m_pTangents = new CVector3f[m_pMeshes[m].m_pSharedMesh->m_iVerticesNum];

			m_pMeshes[m].m_pSharedMesh->m_pBinormals = (CVector3f*) m_pMeshes[m].m_VB.Get(VB_HAS_BINORMALS);
			m_pMeshes[m].m_pSharedMesh->m_pBinormals = new CVector3f[m_pMeshes[m].m_pSharedMesh->m_iVerticesNum];
	
		
		}
		

		//while (i--) // loop on all faces
		//for (i=0;i<m_pMeshes[m].m_iFacesNum;i++)
		i=0;
		{



			//assert(_CrtCheckMemory());
			m_pMeshes[m].m_VB.SetFaceType(VB_REGULAR);
			//assert(_CrtCheckMemory());				
			pTempShader = CSingleton<CShaderManager>::Inst()->Load(pcTempShaderName);
			//assert(_CrtCheckMemory());
			m_pMeshes[m].m_VB.SetShaderSP(pTempShader);
			//assert(_CrtCheckMemory());


			if (bPPL && st_RendererI->m_bPPLused)
			{
			
			int iOriginalLength = strlen(pcTempShaderName);
			strcpy(pcTempShaderNameNormalMap,pcTempShaderName);

			pcTempShaderNameNormalMap[iOriginalLength] = '_';
			pcTempShaderNameNormalMap[iOriginalLength+1] = 'l';
			pcTempShaderNameNormalMap[iOriginalLength+2] = 'o';
			pcTempShaderNameNormalMap[iOriginalLength+3] = 'c';
			pcTempShaderNameNormalMap[iOriginalLength+4] = 'a';
			pcTempShaderNameNormalMap[iOriginalLength+5] = 'l';
			pcTempShaderNameNormalMap[iOriginalLength+6] = '.';
			pcTempShaderNameNormalMap[iOriginalLength+7] = 't';
			pcTempShaderNameNormalMap[iOriginalLength+8] = 'g';
			pcTempShaderNameNormalMap[iOriginalLength+9] = 'a';
			pcTempShaderNameNormalMap[iOriginalLength+10] = NULL;
								

				strlwr(pcTempShaderNameNormalMap);
				/*if ( strstr(pcTempShaderNameNormalMap,".jpg")!=NULL || strstr(pcTempShaderNameNormalMap,".tga")!=NULL || strstr(pcTempShaderNameNormalMap,".bmp")!=NULL)
					pcTempShaderNameNormalMap[ strlen(pcTempShaderNameNormalMap)-4 ] =NULL;*/


				m_pMeshes[m].m_VB.SetNormalTexture(st_TextureManager::Inst()->LoadTexAndPush(pcTempShaderNameNormalMap,false,true,false));

				

				FindTextureExtension(pcTempShaderName);
				m_pMeshes[m].m_VB.SetBaseTexture(st_TextureManager::Inst()->LoadTexAndPush(pcTempShaderName,false,true,true));
			}

		
			DWORD dwTempVBHas = 0;

			dwTempVBHas |= VB_HAS_VERTS;
			dwTempVBHas |= VB_HAS_NORMALS;
			dwTempVBHas |= VB_HAS_TEXCOORDS;

			dwTempVBHas |= VB_HAS_TANGENTS;

			dwTempVBHas |= VB_HAS_BINORMALS;

			// debug
			dwTempVBHas |= VB_HAS_COLOR;


			m_pMeshes[m].m_VB.SetVBHasType(dwTempVBHas);


			m_pMeshes[m].m_VB.SetSize(m_pMeshes[m].m_pSharedMesh->m_iVerticesNum);
			
			m_pMeshes[m].m_VB.Set(VB_HAS_VERTS,m_pMeshes[m].m_pSharedMesh->m_pOriginalVerts);

			m_pMeshes[m].m_VB.Set(VB_HAS_NORMALS,m_pMeshes[m].m_pSharedMesh->m_pNormals);

			m_pMeshes[m].m_VB.Set(VB_HAS_TEXCOORDS,m_pMeshes[m].m_pSharedMesh->m_pTexCoords);

			m_pMeshes[m].m_VB.Set(VB_HAS_COLOR,m_pMeshes[m].m_pColors);

			m_pMeshes[m].m_VB.Set(VB_HAS_TANGENTS,m_pMeshes[m].m_pSharedMesh->m_pTangents);
			
			m_pMeshes[m].m_VB.Set(VB_HAS_BINORMALS,m_pMeshes[m].m_pSharedMesh->m_pBinormals);



			
		//

	


		if (iInManager<0)
		{
			m_pMeshes[m].m_pSharedMesh->m_uiIndices = new unsigned int [m_pMeshes[m].m_pSharedMesh->m_iFacesNum*3];

			for (int s=0;s<m_pMeshes[m].m_pSharedMesh->m_iFacesNum;s++)
			{
				m_pMeshes[m].m_pSharedMesh->m_uiIndices[s*3] = (unsigned int) m_pMeshes[m].m_pSharedMesh->m_pMD3Faces[s].vertexIndices[0];
				m_pMeshes[m].m_pSharedMesh->m_uiIndices[s*3+1] = (unsigned int) m_pMeshes[m].m_pSharedMesh->m_pMD3Faces[s].vertexIndices[1];
				m_pMeshes[m].m_pSharedMesh->m_uiIndices[s*3+2] = (unsigned int) m_pMeshes[m].m_pSharedMesh->m_pMD3Faces[s].vertexIndices[2];
			}
		}

		m_pMeshes[m].m_VB.SetIndices(m_pMeshes[m].m_pSharedMesh->m_uiIndices);
		m_pMeshes[m].m_VB.SetIndexCount(m_pMeshes[m].m_pSharedMesh->m_iFacesNum*3);


		if ((pTempShader)->GetDVObject()->m_bIsAutoSprite == true)
			{
				// RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE 
				m_pMeshes[m].m_VB.preTransformBillboard();
			}

			m_pMeshes[m].m_VB.DisableSpecialCounter();

		if (iInManager<0)
		{
			m_pMeshes[m].m_VB.CalculateTangentAndBinormalArrays();
			/*m_pMeshes[m].m_pSharedMesh->m_uiIndices = new unsigned int [m_pMeshes[m].m_pSharedMesh->m_iFacesNum*3];

			for (int s=0;s<m_pMeshes[m].m_pSharedMesh->m_iFacesNum;s++)
			{
				m_pMeshes[m].m_pSharedMesh->m_uiIndices[s*3] = (unsigned int) m_pMeshes[m].m_pSharedMesh->m_pMD3Faces[s].vertexIndices[0];
				m_pMeshes[m].m_pSharedMesh->m_uiIndices[s*3+1] = (unsigned int) m_pMeshes[m].m_pSharedMesh->m_pMD3Faces[s].vertexIndices[1];
				m_pMeshes[m].m_pSharedMesh->m_uiIndices[s*3+2] = (unsigned int) m_pMeshes[m].m_pSharedMesh->m_pMD3Faces[s].vertexIndices[2];
			}*/
		}
			

	}

	//SafeDeleteArray(pcTempShaderName);
		
	}



	//assert(_CrtCheckMemory());

	//Update(CVector3f(0.f,0.f,0.f),CVector3f(0.f,0.f,0.f));
	Update(true,false,NULL);

	/*for (int i=0;i<m_pMeshes[0].m_iVerticesNum;i++)
	{
	}*/

	SafeDeleteArray(pDebugString);
}