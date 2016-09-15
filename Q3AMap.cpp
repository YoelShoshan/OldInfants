#include "q3amap.h"

#include "LogFile.h"
extern CLogFile g_LogFile;

#include "Singleton.h"

#include "NeHeGL.h"
extern GL_Window* g_pWindow; // Main Window

#include "EntitiesParser.h"

#include "Camera.h"
extern CCamera g_Camera1;  // the global camera

#include "Renderer.h"

int CurrentCameraCluster;


extern float g_fDistanceBetweenPlayer;

#include "Engine.h"

#include "Frustum.h"
extern CFrustum g_OpenglCameraFrustum;

extern int g_iUserDefinedBezierPatchTessLevel;


extern int g_iPlayerPosNum;

#include "PhysicsModel.h"
//extern CPhysicsModel g_Player;
#include "PlayerClient.h"
extern CPlayerClient *g_pCurrentlyProcessedPlayerClient,*g_pPlayerClient1,*g_pPlayerClient2;
#include "Bot.h"
extern CBot *g_pCurrentlyProcessedBot, *g_pCurrentlyProcessedBot1, *g_pCurrentlyProcessedBot2;

extern bool      g_bCollidedWithStaticEntity;

extern CVector3f g_vGravity;

#include "MD3.h"
#include "MD3MeshManager.h"
#include "MD3Manager.h"

#include <crtdbg.h>
#include <assert.h>

extern CPlayerClient *g_pHumanPlayerClient;


extern int g_iPlayerScore;

#include "CheckersBoard.h"
extern CCheckersBoard* g_pCheckersBoard;

#include "Communication.h"
extern CCommunication g_Communication;

///////////////////////////
#include "ZedTracker.h"
///////////////////////////



//////////////////////////// CQuake3BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
// Constructer
//////////////////////////// CQuake3BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
CQuake3BSP::CQuake3BSP()
{
	m_bRecievedFirstUDPMessage=false;
	m_bFightOver = false;

	m_iStaticEntitiesNum = 0;
	m_iDynamicEntitiesNum = 0;
	m_iPlayersNum = 0;

	m_numOfVerts    = 0;	
	m_numOfFaces    = 0;	
	m_numOfTextures = 0;

	
	m_pcMapFileName = NULL;
    	
	m_pVBArray = NULL;
	m_pVerts = NULL;
	m_pFaces = NULL;	
	m_pIndexArray = NULL; 
	m_strEntities = NULL; 
	m_pNodes = NULL;
	m_pLeafs = NULL;
	m_pPlanesORIGINAL = NULL;
	m_pPlanes = NULL;
	m_pLeafFaces = NULL;
	m_pTextures = NULL;
	m_pLeafBrushes = NULL;
	m_pBrushesORIGINAL = NULL;
	m_pBrushes = NULL;
	m_pBrushSides = NULL;

	m_sppLightMaps=NULL;

	m_clusters.pBitsets = NULL;

	m_numOfModels = 0;
	m_pModels = NULL;

	m_iStartPositionsNum=0;
	m_pStartPositions = NULL;


	m_ppAllStaticEntities = new CEntity*[MAX_STATIC_ENTITIES_NUM];

	for (int i=0;i<MAX_STATIC_ENTITIES_NUM;i++)
	{
		m_ppAllStaticEntities[i] = NULL;
	}

	m_ppAllDynamicEntities = new CEntity*[MAX_DYNAMIC_ENTITIES_NUM];

	for (int i=0;i<MAX_DYNAMIC_ENTITIES_NUM;i++)
	{
		m_ppAllDynamicEntities[i] = NULL;
	}


	m_pSampleYOUWIN=NULL;
	m_pSampleYOULOST=NULL;

	
	g_SoundManager.LoadSample("sound/player/announce/youwin.wav",m_pSampleYOUWIN);
	// todo: replace this with "enemy wins"
	g_SoundManager.LoadSample("sound/player/announce/xaero_wins.wav",m_pSampleYOULOST);


	m_fDelayGoingBackToBoard = 0.f;
}



//////////////////////////// ~CQuake3BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
// Destructer
//////////////////////////// ~CQuake3BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
CQuake3BSP::~CQuake3BSP()
{
	for (int i=0;i<m_iStaticEntitiesNum;i++)
	{
		SafeDelete((m_ppAllStaticEntities[i]));
	}

	SafeDeleteArray(m_ppAllStaticEntities);


	for (int i=2;i<m_iDynamicEntitiesNum;i++)
	{
		SafeDelete((m_ppAllDynamicEntities[i]));
	}

	SafeDeleteArray(m_ppAllDynamicEntities);

	m_iStaticEntitiesNum = 0;
	

/*	if (m_pcMapFileName)
	{*/
		g_LogFile.OutPutPlainText("Freeing Quake 3 Arena BSP File:","3","000099",false);	
		g_LogFile.OutPutPlainText(m_pcMapFileName,"3","000000",false);
		g_LogFile.OutPutPlainText(" ","3","000099",false);
/*	}
	else
	{
		///
	}*/

	
	SafeDeleteArray(m_pStartPositions);

	SafeDeleteArray(m_pVBArray);

	int i = st_ShaderManager::Inst()->GetShadersNum();

	
	SafeDeleteArray(m_pFaces);		// The faces information of the object
	
	SafeDeleteArray(m_strEntities);            // our Entities String

	SafeDeleteArray(m_pNodes);
	SafeDeleteArray(m_pLeafs);
	SafeDeleteArray(m_pPlanesORIGINAL);
	SafeDeleteArray(m_pPlanes);
	SafeDeleteArray(m_pLeafFaces);


	SafeDeleteArray(m_pTextures);
	SafeDeleteArray(m_pLeafBrushes);           // The index into the brush array
	SafeDeleteArray(m_pBrushesORIGINAL);
	SafeDeleteArray(m_pBrushes);
	SafeDeleteArray(m_pBrushSides);

	SafeDeleteArray(m_pVerts);
	SafeDeleteArray(m_pIndexArray);            // The indices array

	SafeDeleteArray(m_sppLightMaps);

	SafeDeleteArray(m_pcMapFileName);

	SafeDeleteArray(m_pModels);

	SafeDeleteArray(m_clusters.pBitsets);

	g_SoundManager.FreeSound(m_pSampleYOUWIN);
	// todo: replace this with "enemy wins"
	g_SoundManager.FreeSound(m_pSampleYOULOST);

	g_LogFile.OutPutPlainText("Successful.","3","009900",true);


}


//////////////////////////// LoadBSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
// load the q3 bsp map from file
//////////////////////////// LoadBSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
bool CQuake3BSP::LoadBSP(const char *strFileName)
{

	FILE *fp = NULL;
	int i = 0;
	
	g_LogFile.OutPutPlainText("Attempting to load:","3","000099",false);
	g_LogFile.OutPutPlainText(strFileName,"3","000000",false);
	g_LogFile.OutPutPlainText(".","3","000000",true);
	
	g_LogFile.OutPutPlainText(strFileName,"3","000000",false);
	g_LogFile.OutPutPlainText(" is ","3","000000",false);

	
	if((fp = fopen(strFileName, "rb")) == NULL)
	{
		g_LogFile.OutPutPlainText("NOT FOUND!!!","3","990000",true);
		MessageBox(g_pWindow->hWnd, "Could not find BSP file!", "Error", MB_OK);
		return false;
	}
	else
		g_LogFile.OutPutPlainText("found.","3","009900",true);


	g_LogFile.OutPutPlainText("Creating Temp BSP data arrays:","3","000000",false);

	// Initialize the header and lump structures
	tBSPHeader header = {0};
	tBSPLump lumps[kMaxLumps] = {0};

	// Read in the header and lump data
	fread(&header, 1, sizeof(tBSPHeader), fp);
	fread(&lumps, kMaxLumps, sizeof(tBSPLump), fp);


	// Allocate the vertex memory
	m_numOfVerts = lumps[kVertices].length / sizeof(tBSPVertex);
	m_pVerts     = new tBSPVertex [m_numOfVerts];

	// Allocate the face memory
	m_numOfFaces = lumps[kFaces].length / sizeof(tBSPFace);
	m_pFaces     = new tBSPFace [m_numOfFaces];

	// Allocate memory to read in the texture information.
	m_numOfTextures = lumps[kTextures].length / sizeof(tBSPTexture);
	m_pTextures = new tBSPTexture [m_numOfTextures];

	// Allocate memory to read in the lightmap data.  
	m_numOfLightmaps = lumps[kLightmaps].length / sizeof(tBSPLightmap);
	tBSPLightmap *pLightmaps = new tBSPLightmap [m_numOfLightmaps ];
	
	m_sppLightMaps = new CSmartPointer<CTexture>[m_numOfLightmaps];
	

	/*m_ppLightMaps = new CTexture*[m_numOfLightmaps];
	for (int iCount=0;iCount<m_numOfLightmaps;iCount++)
		m_ppLightMaps[iCount] = new CTexture;*/


	// Seek to the position in the file that stores the vertex information
	fseek(fp, lumps[kVertices].offset, SEEK_SET);

	float temp=0.f;
	float temp2=0.f;
	
	// Go through all of the vertices that need to be read and swap axises
	for(i = 0; i < m_numOfVerts; i++)
	{
		// Read in the current vertex
		fread(&m_pVerts[i], 1, sizeof(tBSPVertex), fp);
		
		// Swap the y and z values, and negate the new z so Y is up.
		temp = m_pVerts[i].vPosition.v[1];
		m_pVerts[i].vPosition.v[1] = m_pVerts[i].vPosition.v[2];
		m_pVerts[i].vPosition.v[2] = -temp;

		// Swap the y and z values, and negate the new z so Y is up.
		temp2 = m_pVerts[i].vNormal.v[1];
		m_pVerts[i].vNormal.v[1] = m_pVerts[i].vNormal.v[2];
		m_pVerts[i].vNormal.v[2] = -temp2;
		
		// Negate the V texture coordinate because it is upside down otherwise...
		//m_pVerts[i].vTextureCoord.v[1] = 1 - m_pVerts[i].vTextureCoord.v[1];
	}	

	//assert(_CrtCheckMemory());

	// Seek to the position in the file that stores the face information
	fseek(fp, lumps[kFaces].offset, SEEK_SET);

	// Read in all the face information
	fread(m_pFaces, m_numOfFaces, sizeof(tBSPFace), fp);

	// Seek to the position in the file that stores the texture information
	fseek(fp, lumps[kTextures].offset, SEEK_SET);
	
	// Read in all the texture information
	fread(m_pTextures, m_numOfTextures, sizeof(tBSPTexture), fp);

	// DEBUG
/*
	int iDebugContents=0;
	int iDebugFlags=0;

	char* cpDebugText = new char[400];


	for (int t=0;t<m_numOfTextures;t++)
	{
		iDebugContents = m_pTextures[t].contents;
		iDebugFlags = m_pTextures[t].flags;

		sprintf(cpDebugText,"Texture #%d: Contents:%d, Flags:%d",t,iDebugContents,iDebugFlags);
		g_LogFile.OutPutPlainText(cpDebugText,"3","000099",true);	


	}


	SafeDeleteArray(cpDebugText);*/
	////////

	

	char paImageName[200];
		

	// Seek to the position in the file that stores the lightmap information
	fseek(fp, lumps[kLightmaps].offset, SEEK_SET);

	// Go through all of the lightmaps and read them in
	for(i = 0; i < m_numOfLightmaps ; i++)
	{
		// Read in the RGB data for each lightmap
		fread(&pLightmaps[i], 1, sizeof(tBSPLightmap), fp);

		// Create a texture map for each lightmap that is read in.  The lightmaps
		// are always 128 by 128.

		//m_lightmaps[i] = CSingleton<CTextureManager>::Instance()->LoadLightMapAndPush((unsigned char *)pLightmaps[i].imageBits, 128, 128);

		//assert(_CrtCheckMemory());
		m_sppLightMaps[i] = CSingleton<CTextureManager>::Inst()->LoadLightMapAndPush((unsigned char *)pLightmaps[i].imageBits, 128, 128);
		//assert(_CrtCheckMemory());

	}


	//assert(_CrtCheckMemory());

	// Delete the image bits because we are already done with them
	delete [] pLightmaps;

	// the BSP nodes, the leafs, the leaf faces, BSP splitter planes and
	// visibility data (clusters).
	/****************************************************************/

	// Store the number of nodes and allocate the memory to hold them
	m_numOfNodes = lumps[kNodes].length / sizeof(tBSPNode);
	m_pNodes     = new tBSPNode [m_numOfNodes];

	// Seek to the position in the file that hold the nodes and store them in m_pNodes
	fseek(fp, lumps[kNodes].offset, SEEK_SET);
	fread(m_pNodes, m_numOfNodes, sizeof(tBSPNode), fp);

	for(i = 0; i < m_numOfNodes; i++)
	{
		// Swap the y and z values, and negate the new z so Y is up.
		float temp = m_pNodes[i].max.y;
		m_pNodes[i].max.y = m_pNodes[i].max.z;
		m_pNodes[i].max.z = -temp;

		temp = m_pNodes[i].min.y;
		m_pNodes[i].min.y = m_pNodes[i].min.z;
		m_pNodes[i].min.z = -temp;


		///// DEBUG DEBUG DEBUG

		temp = m_pNodes[i].max.z;
		m_pNodes[i].max.z = m_pNodes[i].min.z;
		m_pNodes[i].min.z = temp;
	}	


	tBSPLeafORIGINAL* pTempLeafs;

	// Store the number of leafs and allocate the memory to hold them
	m_numOfLeafs = lumps[kLeafs].length / sizeof(tBSPLeafORIGINAL);
	pTempLeafs     = new tBSPLeafORIGINAL [m_numOfLeafs];
	m_pLeafs       = new tBSPLeaf [m_numOfLeafs];


	//assert(_CrtCheckMemory());

	// Seek to the position in the file that holds the leafs and store them in pTempLeafs
	fseek(fp, lumps[kLeafs].offset, SEEK_SET);

	//assert(_CrtCheckMemory());

	fread(pTempLeafs, m_numOfLeafs, sizeof(tBSPLeafORIGINAL), fp);

	//assert(_CrtCheckMemory());

	// Now we need to go through and convert all the leaf bounding boxes
	// to the normal OpenGL Y up axis.
	for(i = 0; i < m_numOfLeafs; i++)
	{
		// Swap the min y and z values, then negate the new Z
		float temp = pTempLeafs[i].min.y;
		pTempLeafs[i].min.y = pTempLeafs[i].min.z;
		pTempLeafs[i].min.z = -temp;

		// Swap the max y and z values, then negate the new Z
		temp = pTempLeafs[i].max.y;
		pTempLeafs[i].max.y = pTempLeafs[i].max.z;
		pTempLeafs[i].max.z = -temp;


		temp = pTempLeafs[i].max.z;
		pTempLeafs[i].max.z = pTempLeafs[i].min.z;
		pTempLeafs[i].min.z = temp;

	}


	//assert(_CrtCheckMemory());

	for (int l=0;l<m_numOfLeafs;l++)
	{
		m_pLeafs[l].cluster = pTempLeafs[l].cluster;
		m_pLeafs[l].area = pTempLeafs[l].area;
		m_pLeafs[l].min.x = pTempLeafs[l].min.x;
		m_pLeafs[l].min.y = pTempLeafs[l].min.y;
		m_pLeafs[l].min.z = pTempLeafs[l].min.z;
		m_pLeafs[l].max.x = pTempLeafs[l].max.x;
		m_pLeafs[l].max.y = pTempLeafs[l].max.y;
		m_pLeafs[l].max.z = pTempLeafs[l].max.z;
		m_pLeafs[l].leafface = pTempLeafs[l].leafface;
		m_pLeafs[l].numOfLeafFaces = pTempLeafs[l].numOfLeafFaces;
		m_pLeafs[l].leafBrush = pTempLeafs[l].leafBrush;
		m_pLeafs[l].numOfLeafBrushes = pTempLeafs[l].numOfLeafBrushes;
	}


	SafeDeleteArray(pTempLeafs);

	// Store the number of leaf faces and allocate the memory for them
	m_numOfLeafFaces = lumps[kLeafFaces].length / sizeof(int);
	m_pLeafFaces     = new int [m_numOfLeafFaces];

	// Seek to the leaf faces lump, then read it's data
	fseek(fp, lumps[kLeafFaces].offset, SEEK_SET);
	fread(m_pLeafFaces, m_numOfLeafFaces, sizeof(int), fp);

	// Store the number of planes, then allocate memory to hold them
	m_numOfPlanes = lumps[kPlanes].length / sizeof(tBSPPlane);
	m_pPlanesORIGINAL     = new tBSPPlane [m_numOfPlanes];
	m_pPlanes             = new CPlane [m_numOfPlanes];

	// Seek to the planes lump in the file, then read them into m_pPlanesORIGINAL
	fseek(fp, lumps[kPlanes].offset, SEEK_SET);
	fread(m_pPlanesORIGINAL, m_numOfPlanes, sizeof(tBSPPlane), fp);

	// Go through every plane and convert it's normal to the Y-axis being up
	for(i = 0; i < m_numOfPlanes; i++)
	{
		m_pPlanes[i].n.v[0] = m_pPlanesORIGINAL[i].vNormal.v[0];
		m_pPlanes[i].n.v[1] = m_pPlanesORIGINAL[i].vNormal.v[2];
		m_pPlanes[i].n.v[2] = -m_pPlanesORIGINAL[i].vNormal.v[1];

		m_pPlanes[i].d = m_pPlanesORIGINAL[i].d;

	}

	SafeDeleteArray(m_pPlanesORIGINAL);

	// Seek to the position in the file that holds the visibility lump
	fseek(fp, lumps[kVisData].offset, SEEK_SET);

	// Check if there is any visibility information first
	if(lumps[kVisData].length) 
	{
		// Read in the number of vectors and each vector's size
		fread(&(m_clusters.numOfClusters),	 1, sizeof(int), fp);
		fread(&(m_clusters.bytesPerCluster), 1, sizeof(int), fp);

		// Allocate the memory for the cluster bitsets
		int size = m_clusters.numOfClusters * m_clusters.bytesPerCluster;
		m_clusters.pBitsets = new byte [size];

		// Read in the all the visibility bitsets for each cluster
		fread(m_clusters.pBitsets, 1, sizeof(byte) * size, fp);
	}
	// Otherwise, we don't have any visibility data (prevents a crash)
	else
		m_clusters.pBitsets = NULL;

	// BRUSHES

	// Read in index to leaf brushes
	m_numOfLeafBrushes = lumps[kLeafBrushes].length / sizeof (int);
	m_pLeafBrushes = new int[m_numOfLeafBrushes];
	fseek (fp, lumps[kLeafBrushes].offset, SEEK_SET);
	fread (m_pLeafBrushes, m_numOfLeafBrushes, sizeof (int), fp);


	// read the models
	m_numOfModels = lumps[kModels].length / sizeof (tBSPModel);
	m_pModels = new tBSPModel [m_numOfModels];



	// Seek to the position in the file that stores the models
	fseek(fp, lumps[kModels].offset, SEEK_SET);
	//fread (m_pModels, m_numOfModels, sizeof (tBSPModel), fp);

	
	for (i=0;i<m_numOfModels;i++)
	{
		// Read in the current vertex
		fread(&m_pModels[i], 1, sizeof(tBSPModel), fp);
		
		// Swap the y and z values, and negate the new z so Y is up.
		temp = m_pModels[i].min.v[1];
		m_pModels[i].min.v[1] = m_pModels[i].min.v[2];
		m_pModels[i].min.v[2] = -temp;

		temp = m_pModels[i].max.v[1];
		m_pModels[i].max.v[1] = m_pModels[i].max.v[2];
		m_pModels[i].max.v[2] = -temp;


		temp = m_pModels[i].max.v[2];
		m_pModels[i].max.v[2] = m_pModels[i].min.v[2];
		m_pModels[i].min.v[2] = temp;


	}
	


	// Read in the brushes
	m_numOfBrushes = lumps[kBrushes].length / sizeof (tBSPBrushORIGINAL);
	m_pBrushesORIGINAL = new tBSPBrushORIGINAL[m_numOfBrushes];
	fseek (fp, lumps[kBrushes].offset, SEEK_SET);
	fread (m_pBrushesORIGINAL, m_numOfBrushes, sizeof (tBSPBrushORIGINAL), fp);

	// convert from tBSPBrushORIGINAL to tBSPBrush

	m_pBrushes = new tBSPBrush[m_numOfBrushes];

	for (int b=0;b<m_numOfBrushes;b++)
	{
		m_pBrushes[b].numOfBrushSides = m_pBrushesORIGINAL[b].numOfBrushSides;
		m_pBrushes[b].brushSide = m_pBrushesORIGINAL[b].brushSide;
		m_pBrushes[b].textureID = m_pBrushesORIGINAL[b].textureID;

		m_pBrushes[b].m_pPlanes = NULL;
	}


	SafeDeleteArray(m_pBrushesORIGINAL);

	// Read in the brush sides
	m_numOfBrushSides =	lumps[kBrushSides].length / sizeof (tBSPBrushORIGINALSide);
	m_pBrushSides = new tBSPBrushORIGINALSide[m_numOfBrushSides];
	fseek (fp, lumps[kBrushSides].offset, SEEK_SET);
	fread (m_pBrushSides, m_numOfBrushSides, sizeof (tBSPBrushORIGINALSide), fp);


	m_numOfIndices = lumps[kMeshVerts].length / sizeof(int);

	m_pIndexArray = new int [m_numOfIndices];

	fseek(fp, lumps[kMeshVerts].offset, SEEK_SET);

	fread(m_pIndexArray, m_numOfIndices, sizeof(int), fp);


	// ADDED read entities into m_strEntities

	m_numOfCharsInEntities = lumps[kEntities].length / sizeof(char);
	
	m_strEntities = new char [m_numOfCharsInEntities];

	fseek(fp, lumps[kEntities].offset, SEEK_SET);

	fread(m_strEntities,m_numOfCharsInEntities,sizeof(char), fp);

	///

	// ADDED dump entities into "Entities.txt"

	//g_FileManager.WriteStringToFile(m_strEntities,"ThisMapEntities.txt"); // debug

	//assert(_CrtCheckMemory());


	

	// Get the Entities String from our g_Level
	st_EntitiesParser::Inst()->SetEntitiesString(m_strEntities);
	// Export it to a txt file...
	//st_EntitiesParser::Inst()->ExportEntitiesToFile();
	// Load it at the first time
	st_EntitiesParser::Inst()->Load();


	FillStaticEntities();
	LinkAllStaticEntitiesToLeafs();

	SafeDeleteArray(m_pStartPositions);
	m_iStartPositionsNum = st_EntitiesParser::Inst()->FillPlayerPositions(m_pStartPositions);

	//assert(_CrtCheckMemory());

	//g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->SetPosition(m_pStartPositions[3]);
	
	
	
	//assert(_CrtCheckMemory());
	
	//////////////////////////////////////////////////
	// ADD THIS ADD THIS ADD THIS ADD THIS ADD THIS 
	//g_iPlayerPosNum=1;


	g_iPlayerPosNum=1;

	//////////////////////////////////////////////////


	//g_EntitiesParser.GetPlayerPosition(g_PlayerPosNum,g_PlayerStartPos);

	CVector3f vTemp;

	st_EntitiesParser::Inst()->GetPlayerPosition(g_iPlayerPosNum,vTemp);

	//st_EntitiesParser::Inst()->ExportEntitiesToFile();

	// Position the camera in our new start position

	g_Camera1.SetPosition(vTemp);

	g_Camera1.m_vLookAt = vTemp;



	// Close the file
	if (fp)
	fclose(fp);

	// Here we allocate enough bits to store all the faces for our bitset
	m_FacesDrawn.Resize(m_numOfFaces);


	g_LogFile.OutPutPlainText("Successful.","3","009900",true);

	// convert faces into vertexbuffers and fill m_pVertexBuffer with them,
	// (textures and shaders are also loaded here)

	FillVBArray();



	/*SetMapFileName("maps/q3dm1.bsp");
	st_Renderer::Inst()->InitSkyVBs();		

	FillDynamicEntities();

	g_pPlayerClient1->InitStates();
	g_pPlayerClient2->InitStates();

	g_pPlayerClient1->PutPlayerInStartPos();
				
	g_pPlayerClient2->AdvanceStartPosition();
	g_pPlayerClient2->PutPlayerInStartPos();*/
	
	return true;
}

//////////////////////////// FillVBArray \\\\\\\\\\\\\\\\\\\\\\\\\\\*
///// Fill the VertexBuffer array
//////////////////////////// FillVBArray \\\\\\\\\\\\\\\\\\\\\\\\\\\*
void CQuake3BSP::FillVBArray(void)
{
	// Get the number of faces in our level
	int i = m_numOfFaces;

	m_pVBArray = new CVertexBuffer[m_numOfFaces];

	//UINT uiTempShaderIndex;
	CShader* pTempShader;

	
		

	CShader* pShader = NULL;
	CShaderPass* pShaderPass = NULL;


	int uiShaderPassLoc=0;
	int uiShaderLoc=0;


	g_LogFile.OutPutPlainText("Convertion to Vertex Buffers:","3","000000",false);
	
	// Go through all the faces
	while(i--)
	{	
		

		if (m_pFaces[i].type==Q3FACE_SIMPLE_POLYGON)
			m_pVBArray[i].SetFaceType(VB_REGULAR);
		else
		if (m_pFaces[i].type==Q3FACE_BEZIER_PATCH)
		{
			m_pVBArray[i].SetFaceType(VB_BEZIER_PATCH);
			m_pVBArray[i].SetBezierPatchDimentions(m_pFaces[i].size[0],m_pFaces[i].size[1]);
			
		}
		else
		if (m_pFaces[i].type==Q3FACE_MESH_POLYGON)
			m_pVBArray[i].SetFaceType(VB_MESH);
		else
		if (m_pFaces[i].type==Q3FACE_BILLBOARD)
		{
			// TODO: implement this (in a similar way to autoSprite)
			m_pVBArray[i].SetFaceType(VB_BILLBOARD);
		}
		

		pTempShader = CSingleton<CShaderManager>::Inst()->Load(m_pTextures[m_pFaces[i].textureID].strName);
		
		m_pVBArray[i].SetShaderSP(pTempShader);

		
		if (m_pFaces[i].lightmapID>=0)
			m_pVBArray[i].SetLMTexID(m_sppLightMaps[m_pFaces[i].lightmapID]->GetID());


		if (m_pFaces[i].type==Q3FACE_BEZIER_PATCH)
		{
			// indices are created in real time
		}
		else
		{
			m_pVBArray[i].AllocIndexBuffer(m_pFaces[i].numMeshVerts);
			UINT * pTempIndex;
			pTempIndex = m_pVBArray[i].GetIndices();

			memcpy(pTempIndex,&m_pIndexArray[m_pFaces[i].meshVertIndex],
				m_pFaces[i].numMeshVerts*sizeof(UINT));
		}
				

		DWORD dwTempVBHas = 0;

		dwTempVBHas |= VB_HAS_VERTS;
		dwTempVBHas |= VB_HAS_NORMALS;
		dwTempVBHas |= VB_HAS_TEXCOORDS;


		if (m_pFaces[i].type==Q3FACE_BEZIER_PATCH)
		{
			dwTempVBHas |= VB_HAS_BEZIER_VERTS;
			dwTempVBHas |= VB_HAS_BEZIER_NORMALS;
			dwTempVBHas |= VB_HAS_BEZIER_TEXCOORDS;
			dwTempVBHas |= VB_HAS_BEZIER_LMCOORDS;
		}


		if (m_pFaces[i].lightmapID>=0)
			dwTempVBHas |= VB_HAS_LMCOORDS;
		
		dwTempVBHas |= VB_HAS_COLOR;


		if (m_pFaces[i].type==Q3FACE_BEZIER_PATCH)
		{
			m_pVBArray[i].SetBezierPatchDimentions(m_pFaces[i].size[0],m_pFaces[i].size[1]);
		}


		m_pVBArray[i].Alloc(dwTempVBHas,m_pFaces[i].numOfVerts);


		if (m_pFaces[i].type==Q3FACE_BEZIER_PATCH)
		{
			CVector3f* pTempVec3f;
			pTempVec3f = (CVector3f*) m_pVBArray[i].Get(VB_HAS_BEZIER_VERTS);
			for (int j=0;j<m_pFaces[i].numOfVerts;j++)
				memcpy(&pTempVec3f[j],&m_pVerts[m_pFaces[i].startVertIndex+j].vPosition,
				sizeof(CVector3f));
		}
		else
		if (dwTempVBHas & VB_HAS_VERTS)
		{
			CVector3f* pTempVec3f;
			pTempVec3f = (CVector3f*) m_pVBArray[i].Get(VB_HAS_VERTS);
			for (int j=0;j<m_pFaces[i].numOfVerts;j++)
				memcpy(&pTempVec3f[j],&m_pVerts[m_pFaces[i].startVertIndex+j].vPosition,
				sizeof(CVector3f));
		}
		
		
		
		if (m_pFaces[i].type==Q3FACE_BEZIER_PATCH)
		{
			CVector3f* pTempVec3f;
			pTempVec3f = (CVector3f*) m_pVBArray[i].Get(VB_HAS_BEZIER_NORMALS);
			for (int j=0;j<m_pFaces[i].numOfVerts;j++)
				memcpy(&pTempVec3f[j],&m_pVerts[m_pFaces[i].startVertIndex+j].vNormal,
				sizeof(CVector3f));
		}
		else
		if (dwTempVBHas & VB_HAS_NORMALS)
		{
			CVector3f* pTempVec3f;
			pTempVec3f = (CVector3f*) m_pVBArray[i].Get(VB_HAS_NORMALS);
			for (int j=0;j<m_pFaces[i].numOfVerts;j++)
				memcpy(&pTempVec3f[j],&m_pVerts[m_pFaces[i].startVertIndex+j].vNormal,
				sizeof(CVector3f));
		}

		/*if (m_pFaces[i].type==Q3FACE_BEZIER_PATCH)
		{
		}*/
		if (dwTempVBHas & VB_HAS_COLOR)
		{
			CColor4* pTempColor4;
			pTempColor4 = (CColor4*) m_pVBArray[i].Get(VB_HAS_COLOR);
			
			for (int j=0;j<m_pFaces[i].numOfVerts;j++)
				memcpy(&pTempColor4[j],&m_pVerts[m_pFaces[i].startVertIndex+j].color,
				sizeof(CColor4));
		}



		if (m_pFaces[i].type==Q3FACE_BEZIER_PATCH)
		{
			CVector2f * pTempVec2f;
			pTempVec2f = (CVector2f*) m_pVBArray[i].Get(VB_HAS_BEZIER_TEXCOORDS);

			for (int j=0;j<m_pFaces[i].numOfVerts;j++)
				memcpy(&pTempVec2f[j],&m_pVerts[m_pFaces[i].startVertIndex+j].vTextureCoord,
				sizeof(CVector2f));
		}
		else
		if (dwTempVBHas & VB_HAS_TEXCOORDS)
		{
			CVector2f * pTempVec2f;
			pTempVec2f = (CVector2f*) m_pVBArray[i].Get(VB_HAS_TEXCOORDS);
		
			for (int j=0;j<m_pFaces[i].numOfVerts;j++)
				memcpy(&pTempVec2f[j],&m_pVerts[m_pFaces[i].startVertIndex+j].vTextureCoord,
				sizeof(CVector2f));
		}

		

		if (m_pFaces[i].type==Q3FACE_BEZIER_PATCH)
		{
			CVector2f * pTempVec2f;
			pTempVec2f = (CVector2f*) m_pVBArray[i].Get(VB_HAS_BEZIER_LMCOORDS);

			for (int j=0;j<m_pFaces[i].numOfVerts;j++)
				memcpy(&pTempVec2f[j],&m_pVerts[m_pFaces[i].startVertIndex+j].vLightmapCoord,
				sizeof(CVector2f));
		}
		else
		if (dwTempVBHas & VB_HAS_LMCOORDS)
		{
			CVector2f * pTempVec2f;
			pTempVec2f = (CVector2f*) m_pVBArray[i].Get(VB_HAS_LMCOORDS);

			for (int j=0;j<m_pFaces[i].numOfVerts;j++)
				memcpy(&pTempVec2f[j],&m_pVerts[m_pFaces[i].startVertIndex+j].vLightmapCoord,
				sizeof(CVector2f));
		}

		//if (CSingleton<CShaderManager>::Inst()->GetShader(uiTempShaderIndex)->GetDVObject()->m_bIsAutoSprite == true)
		if ((pTempShader)->GetDVObject()->m_bIsAutoSprite == true)
		{
			// RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE 
			m_pVBArray[i].preTransformBillboard();
		}


		if (m_pFaces[i].type==Q3FACE_BEZIER_PATCH)
		{
			m_pVBArray[i].ReMakeBezierPatch(g_iUserDefinedBezierPatchTessLevel);
		}

		
	}		
	g_LogFile.OutPutPlainText("Successful.","3","009900",true);

	m_FacesDrawn.Resize(m_numOfFaces);
	
}


//////////////////////////// PushAllToRenderer \\\\\\\\\\\\\\\\\\\\\\\\\\\*
///// push all the vertexbuffers to the renderer (used mainly for debugging)
//////////////////////////// PushAllToRenderer \\\\\\\\\\\\\\\\\\\\\\\\\\\*
void CQuake3BSP::PushAllToRenderer(void)
{
	int i = m_numOfFaces;

	while(i--)
	{
		st_Renderer::Inst()->Push(&m_pVBArray[i]);
	}
}

//////////////////////////// PushAllBezierPatchesToRenderer \\\\\\\\\\\\\\\\\\\\\\\\\\\*
///// push ONLY the bezier patches
//////////////////////////// PushAllBezierPatchesToRenderer \\\\\\\\\\\\\\\\\\\\\\\\\\\*
void CQuake3BSP::PushAllBezierPatchesToRenderer(void)
{
	int i = m_numOfFaces;

	while(i--)
	{
		if (m_pVBArray[i].GetFaceType()==VB_BEZIER_PATCH)
			st_Renderer::Inst()->Push(&m_pVBArray[i]);
	}
}

//////////////////////////// PushFaceToRenderer \\\\\\\\\\\\\\\\\\\\\\\\\\\*
///// push a single face to the renderer
//////////////////////////// PushFaceToRenderer \\\\\\\\\\\\\\\\\\\\\\\\\\\*
void CQuake3BSP::PushFaceToRenderer(UINT uiNum)
{
	if (uiNum>=m_numOfFaces) return;
	//if (g_OpenglCameraFrustum.PolygonInFrustum((CVector3f*)m_pVBArray[uiNum].Get(VB_HAS_VERTS),m_pVBArray[uiNum].GetSize()))
		st_Renderer::Inst()->Push(&m_pVBArray[uiNum]);
}

//////////////////////////// FindLeaf \\\\\\\\\\\\\\\\\\\\\\\\\\\*
///// returns the leaf index for a position in space
//////////////////////////// FindLeaf \\\\\\\\\\\\\\\\\\\\\\\\\\\*
int CQuake3BSP::FindLeaf(const CVector3f &vPos)
{
	int i = 0;
	float distance = 0.0f;

	while(i >= 0)
	{
		const tBSPNode&  node = m_pNodes[i];
		//const tBSPPlane& plane = m_pPlanesORIGINAL[node.plane];
		const CPlane& plane = m_pPlanes[node.plane];

        distance =	plane.n.v[0] * vPos.v[0] + 
					plane.n.v[1] * vPos.v[1] + 
					plane.n.v[2] * vPos.v[2] - plane.d;

        if(distance >= 0)	
		{
            i = node.front;
        }
        else		
		{
            i = node.back;
        }
    }

    return ~i; 
}


//////////////////////////// FindLeafs \\\\\\\\\\\\\\\\\\\\\\\\\\\*
///// fills the recieved leaf indices with the leafs that intersect with the given bounding sphere
//////////////////////////// FindLeafs \\\\\\\\\\\\\\\\\\\\\\\\\\\*
void CQuake3BSP::FindLeafs(LEAFS_INDICES_VECTOR& fillMe,const CVector3f &vPos,float fRadius,int iNode)
{
	float distance = 0.0f;


		if (iNode<0)
		{
			fillMe.push_back(~iNode);
			return;
		}


		const tBSPNode&  node = m_pNodes[iNode];
		//const tBSPPlane& plane = m_pPlanesORIGINAL[node.plane];
		const CPlane& plane = m_pPlanes[node.plane];

        distance =	plane.n.v[0] * vPos.v[0] + 
					plane.n.v[1] * vPos.v[1] + 
					plane.n.v[2] * vPos.v[2] - plane.d;
	
        if(distance >= fRadius)	// sphere infront of plane		
		{
            iNode = node.front;
			FindLeafs(fillMe,vPos,fRadius,iNode);
        }
		else
		if(distance <= -fRadius) // camera behind plane
		{
            iNode = node.back;
			FindLeafs(fillMe,vPos,fRadius,iNode);
        }
		else // need to travel both
		{
			iNode = node.front;
			FindLeafs(fillMe,vPos,fRadius,iNode);

			iNode = node.back;
			FindLeafs(fillMe,vPos,fRadius,iNode);
		}
	
}

//////////////////////////// IsClusterVisible \\\\\\\\\\\\\\\\\\\\\\\\\\\*
///// is "test" cluster is potentially visible from "current" cluster?
//////////////////////////// IsClusterVisible \\\\\\\\\\\\\\\\\\\\\\\\\\\*
int CQuake3BSP::IsClusterVisible(int current, int test)
{
	if(!m_clusters.pBitsets || current < 0) return 1;

	// Use binary math to get the 8 bit visibility set for the current cluster
	byte visSet = m_clusters.pBitsets[(current*m_clusters.bytesPerCluster) + (test / 8)];
	
	// Now that we have our vector (bitset), do some bit shifting to find if
	// the "test" cluster is visible from the "current" cluster, according to the bitset.
	int result = visSet & (1 << ((test) & 7));

	// Return the result ( either 1 (visible) or 0 (not visible) )
	return ( result );
}


/////////////////////////// RenderLeaf \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
///// render a leaf
/////////////////////////// RenderLeaf \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void CQuake3BSP::RenderLeaf( int iLeaf, bool CheckFrustum )
{
	// get the leaf we are in
	tBSPLeaf* pLeaf = &m_pLeafs[ iLeaf ];

	// perform PVS as in the previus tutorial
	if(!IsClusterVisible(CurrentCameraCluster, pLeaf->cluster))          // If the current leaf can't be seen from our cluster, go to the next leaf
		return;

	if (CheckFrustum)
		if(!g_OpenglCameraFrustum.BoxInFrustum( pLeaf->min.x, pLeaf->min.y, pLeaf->min.z,
			pLeaf->max.x, pLeaf->max.y, pLeaf->max.z)) return;

	int faceCount = pLeaf->numOfLeafFaces;


	/**/

	// draw static entities

	for (int i=0;i<pLeaf->StaticEntities.size();i++)
		{
			pLeaf->StaticEntities[i]->Draw(false);			
		}

	// draw dynamic entities

	for (int i=0;i<pLeaf->DynamicEntities.size();i++)
		{
			pLeaf->DynamicEntities[i]->Draw(false);			
			//pLeaf->DynamicEntities[i]->DrawBoundingBox();			
		}

	/**/

	// Loop through and render all of the faces in this leaf
	while(faceCount--){
		// Grab the current face index from our leaf faces array
		int faceIndex = m_pLeafFaces[pLeaf->leafface + faceCount];


		// THIS IS IMPORTANT
		if(m_pFaces[faceIndex].type == Q3FACE_BILLBOARD) continue;



		// Since many faces are duplicated in other leafs, we need to
		// make sure this face already hasn't been drawn.
		if(!m_FacesDrawn.On(faceIndex)) 	
		{
			// Set this face as drawn and render it
			m_FacesDrawn.Set(faceIndex);	
			PushFaceToRenderer(faceIndex);
		}
	}			
}

/////////////////////////// RenderTree \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
///// render the BSP tree
/////////////////////////// RenderTree \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
// This function wisely check against the frustum and sort leafs front to back
void CQuake3BSP::RenderTree( int Node, bool CheckFrustum )
{

	// Get a pointer to the node we are now in
	tBSPNode* pNode = &m_pNodes[Node];

	// if the node we came from is COMPLETE_IN then CheckFrustum will hold false and we 
	// won't check this node against the frustum because we know we are COMPLETE_IN the 
	// frustum
	if (CheckFrustum){
		// CheckFrustum = true so our node parent was intersecting the frustum and so
		// he have to continue checking this one
		switch (g_OpenglCameraFrustum.BoxInFrustum( pNode->min.x, pNode->min.y, pNode->min.z, pNode->max.x, pNode->max.y, pNode->max.z)){
			case COMPLETE_OUT: return;				// if this one is complete_out, stop analysing this branch
				// all leaf that come from this node are also complete_out

			case COMPLETE_IN: CheckFrustum = false; // if we are complete_in, Set CheckFrustum to false to not 
				// check frustum on any of my childs, CheckFrustum will be used
				// to call my children

				// if intersecting continue as usual, test all my children against the frustum
		}
	}

	// a node is an structure that hold a partition plane, all leaf are splited in two
	// groups: FRONT and BACK. plane is the partition plane for this node
	//const tBSPPlane* plane = &m_pPlanesORIGINAL[ pNode->plane ];
	const CPlane* plane = &m_pPlanes[ pNode->plane ];

	// We will render leafs in a from to back order 

	// If the camera position is in front the plane
	if ( (plane->n.v[0] * m_CameraPosition.v[0] + plane->n.v[1] * m_CameraPosition.v[1] + plane->n.v[2] * m_CameraPosition.v[2] -  plane->d  > 0) ) 
	{
		if (pNode->front>=0)  RenderTree( pNode->front, CheckFrustum );
		else RenderLeaf( ~pNode->front, CheckFrustum );
		// now that we render or leaf and nodes that are in front of the plane,
		// go with the  BACK ones
		if (pNode->back>=0) RenderTree( pNode->back , CheckFrustum);
		else RenderLeaf( ~pNode->back, CheckFrustum );
	}// else if is CameraPostion is back of the partition plane, draw fack to front
	else 
	{
		if (pNode->back>=0) RenderTree( pNode->back, CheckFrustum );
		else RenderLeaf( ~pNode->back, CheckFrustum );
		if (pNode->front>=0)  RenderTree( pNode->front, CheckFrustum );
		else RenderLeaf( ~pNode->front, CheckFrustum );
	}
}

////////////////////////// RenderLevel \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
///// render the Q3 BSP level
////////////////////////// RenderLevel \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void CQuake3BSP::RenderLevel(const CVector3f &vPos)
{
	// Get the number of faces in our level
	int i = m_numOfFaces;
	
	// Reset our bitset so all the slots are zero. 
	
	m_FacesDrawn.ClearAll();
	

	m_CameraPosition = vPos; 

	int leafIndex = FindLeaf( m_CameraPosition );
	CurrentCameraCluster =   m_pLeafs[leafIndex].cluster;

	st_Engine::Inst()->m_iCameraClusterNum = CurrentCameraCluster;
	st_Engine::Inst()->m_iCameraLeafNum    = leafIndex;

	// Start rendering the tree from the Root (Node 0 ) and start checking the frustum (true)

	RenderTree( 0, true ); 

	//PushAllBezierPatchesToRenderer();


	//BruteDrawDynamicEntities();
}


void CQuake3BSP::DrawMapModelsBBoxes(void)
{
	for (int i=0;i<m_numOfModels;i++)
		st_Renderer::Inst()->DebugDrawBBox(m_pModels[i].min,m_pModels[i].max);

	/*int i=14;
		st_Renderer::Inst()->DebugDrawBBox(m_pModels[i].min,m_pModels[i].max);*/
}

void CQuake3BSP::DrawMapLeafsBBoxes(void)
{
	/*
	CVector3f min,max;
	
	int i=st_Engine::Inst()->m_iCameraLeafNum;

	min.v[0] = m_pLeafs[i].min.x;
	min.v[1] = m_pLeafs[i].min.y;
	min.v[2] = m_pLeafs[i].min.z;

	max.v[0] = m_pLeafs[i].max.x;
	max.v[1] = m_pLeafs[i].max.y;
	max.v[2] = m_pLeafs[i].max.z;

	st_Renderer::Inst()->DebugDrawBBox(min,max);
	*/

	
	CVector3f min,max;
	
	for (int i=0;i<m_numOfLeafs;i++)
	{
		min.v[0] = m_pLeafs[i].min.x;
		min.v[1] = m_pLeafs[i].min.y;
		min.v[2] = m_pLeafs[i].min.z;

		max.v[0] = m_pLeafs[i].max.x;
		max.v[1] = m_pLeafs[i].max.y;
		max.v[2] = m_pLeafs[i].max.z;
	
		st_Renderer::Inst()->DebugDrawBBox(min,max);
	}
	

}

void CQuake3BSP::RenderMapModels(void)
{
	int faceIndex;

	//for (int i=0;i<m_numOfModels;i++)
	for (int i=1;i<m_numOfModels;i++)
	{
		/*int faceIndex;          // The first face index in the model 
		int numOfFaces;         // The number of faces in the model */
		for (int j=0;j<m_pModels[i].numOfFaces;j++)
		{
			faceIndex = m_pModels[i].faceIndex + j;
			// Since many faces are duplicated in other leafs, we need to
			// make sure this face already hasn't been drawn.
			if(!m_FacesDrawn.On(faceIndex)) 	
			{
				// Set this face as drawn and render it
				m_FacesDrawn.Set(faceIndex);	
				PushFaceToRenderer(faceIndex);
			}

		}
	}

}


////////////////////////// SetMapFileName \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
///// set the map file name
////////////////////////// SetMapFileName \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void CQuake3BSP::SetMapFileName(const char* pcSet)
{
	SafeDeleteArray(m_pcMapFileName);
	m_pcMapFileName = new char[strlen(pcSet)+1];
	strcpy(m_pcMapFileName,pcSet);
}


/////////////
// Debug
/////////////

/*void CQuake3BSP::OutPutShadersToFile(void)
{
	std::ofstream t_File;
	t_File.open("ThisMapShaders.txt");
	
	for (int i=0;i<m_numOfTextures;i++)
	{
		t_File << i << ": " << m_pTextures[i].strName << ".\n";
	}

	t_File.close();	
}*/


/////////////////////////// TraceAndResponse \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
////// recuresly slides along the planes
/////////////////////////// TraceAndResponse \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
bool CQuake3BSP::TraceAndResponse(CTraceRequest* input,CTraceResult* output, int iDepth, CEntity* pCausedByEntity)
{
	if (iDepth<20)
		iDepth++;
	else
		return false;



	// DEBUG
	CVector3f vDirOriginal = (input->vRayEnd-input->vRayStart).Normalize();


	
    
	// almost nothing to move
	if (DISTANCE(input->vRayStart,input->vRayEnd)<EPSILON*2)
	{
		output->EndPoint = input->vRayEnd;
		
		if (iDepth==1)
			return true; // we completed the move in the FIRST try
		else
			return false; // we completed the move in a move > first move
	}


	Trace(input,output,true,true,pCausedByEntity); // trying to get there
    
	// didn't collide with anything
	if (output->Fraction>1.f-EPSILON)
	{
		if (iDepth==1)
			return true; // we completed the move in the FIRST try
		else
			return false; // we completed the move in a move > first move
	}


	

	//////// DEBUG DEBUG DEBUG
	

	// we intersected with something, so we need to find the new slide vector

	CVector3f vWhatsLeft = (input->vRayEnd - output->EndPoint); // this is what we still need to travel
	
	vWhatsLeft-= output->CollisionPlane.n * ((vWhatsLeft*output->CollisionPlane.n));


	CVector3f vNormalizedSlideVector = vWhatsLeft;
	vNormalizedSlideVector.Normalize();

	
	// we can't climb such a big slope
	if (vNormalizedSlideVector.v[1] > BIGGEST_CLIMB_SLOPE)
		return false;


	input->vRayStart = output->EndPoint;
	input->vRayEnd   = output->EndPoint + vWhatsLeft;



	TraceAndResponse(input,output,iDepth,pCausedByEntity);
	


	return true;
}


/*
void CQuake3BSP::TraceAgainstModel(CTraceRequest* input,CTraceResult* output,int iModelNum)
{
    m_CurrentTraceInput  = input;
	m_CurrentTraceOutput = output;

	if (input->dwTraceType == 	TRACE_TYPE_LINE)
		m_CurrentTraceInput->fRadius = 0.f;


	{
		m_CurrentTraceOutput->StartOut = true;
		m_CurrentTraceOutput->AllSolid = false;
		m_CurrentTraceOutput->Fraction = 1.f;


		tBSPModel* model = &m_pModels[iModelNum];

		for (int i=0; i < model->numOfBrushes ; i++)
		{
		
			//tBSPBrushORIGINAL* brush = &m_pBrushesORIGINAL[m_pLeafBrushes[model->brushIndex + i]];
			tBSPBrushORIGINAL* brush = &m_pBrushesORIGINAL[model->brushIndex + i];

			//if (brush->numOfBrushSides>0 && m_pTextures[brush->textureID].contents & 1)
			if (brush->numOfBrushSides>0)
				CheckBrush(brush);

		}

				
		if (m_CurrentTraceOutput->Fraction==1)
			m_CurrentTraceOutput->EndPoint = m_CurrentTraceInput->vRayEnd;
		else
		if (m_CurrentTraceOutput->Fraction<EPSILON)
			m_CurrentTraceOutput->EndPoint = m_CurrentTraceInput->vRayStart;
		else
		{
			m_CurrentTraceOutput->EndPoint = m_CurrentTraceInput->vRayStart + 
											(m_CurrentTraceInput->vRayEnd-m_CurrentTraceInput->vRayStart)*m_CurrentTraceOutput->Fraction;
		}
		
	}



}*/

bool CQuake3BSP::TestModelAgainstLeaf(int iModelNum,int iLeafNum)
{
	if (iLeafNum<0 || iLeafNum>=m_numOfLeafs)
		return false;

	/*CVector3f vLeafMin(m_pLeafs[iLeafNum].min.x,m_pLeafs[iLeafNum].min.y,m_pLeafs[iLeafNum].min.z);
	CVector3f vLeafMax(m_pLeafs[iLeafNum].max.x,m_pLeafs[iLeafNum].max.y,m_pLeafs[iLeafNum].max.z);*/

	// DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG

	CVector3f vLeafMin(m_pLeafs[iLeafNum].min.x,m_pLeafs[iLeafNum].min.y,m_pLeafs[iLeafNum].min.z);
	CVector3f vLeafMax(m_pLeafs[iLeafNum].max.x,m_pLeafs[iLeafNum].max.y,m_pLeafs[iLeafNum].max.z);

	return BBoxesIntersect(m_pModels[iModelNum].min,m_pModels[iModelNum].max,vLeafMin,vLeafMax);
		
}



bool CQuake3BSP::TestBBoxAgainstLeaf(CVector3f& vMin,CVector3f& vMax,int iLeafNum)
{
	/*CVector3f vLeafMin(m_pLeafs[iLeafNum].min.x,m_pLeafs[iLeafNum].min.y,m_pLeafs[iLeafNum].min.z);
	CVector3f vLeafMax(m_pLeafs[iLeafNum].max.x,m_pLeafs[iLeafNum].max.y,m_pLeafs[iLeafNum].max.z);*/

	// DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG

	CVector3f vLeafMin(m_pLeafs[iLeafNum].min.x,m_pLeafs[iLeafNum].min.y,m_pLeafs[iLeafNum].min.z);
	CVector3f vLeafMax(m_pLeafs[iLeafNum].max.x,m_pLeafs[iLeafNum].max.y,m_pLeafs[iLeafNum].max.z);

	return BBoxesIntersect(vMin,vMax,vLeafMin,vLeafMax);
		
}


/////////////////////////// Trace \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
////// send a trace down the bsp tree and check against the brushes
/////////////////////////// Trace \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
//void CQuake3BSP::Trace(CTraceRequest* input,CTraceResult* output,bool bActivateEntities, int iAbsolutEntity)
void CQuake3BSP::Trace(CTraceRequest* input,CTraceResult* output,bool bActivateEntities,bool bCheckDynamicEntities, CEntity* pCausedByEntity)
{	
	//
	//m_iTempCurrentTracedEntity = iAbsolutEntity;
	m_pTempCurrentTracedEntity = pCausedByEntity;
	//

    m_CurrentTraceInput  = input;
	m_CurrentTraceOutput = output;

	m_CurrentTraceOutput->HitContents = 0;
	m_CurrentTraceOutput->HitFlags = 0;

	
	// DEBUG DEBUG DEBUG

	//bool bIsntBigEnough=false;

	CVector3f vIsBigEnough = input->vRayEnd - input->vRayStart;

/*	if (  (vIsBigEnough.v[0]<0.03f && vIsBigEnough.v[0]>0) || (vIsBigEnough.v[0]>-0.03f && vIsBigEnough.v[0]<0) ||
		(vIsBigEnough.v[1]<0.03f && vIsBigEnough.v[1]>0) || (vIsBigEnough.v[1]>-0.03f && vIsBigEnough.v[1]<0) ||
		(vIsBigEnough.v[2]<0.03f && vIsBigEnough.v[2]>0) || (vIsBigEnough.v[2]>-0.03f && vIsBigEnough.v[2]<0))
	//if (vIsBigEnough==CVector3f(0.f,0.f,0.f))
	{
		output->Fraction=0.f;
		output->EndPoint = input->vRayStart;
		return;
	}
	else*/
	{
		output->EndPoint+= vIsBigEnough.Normalize()*0.3f;
	}
	

	if (input->dwTraceType == 	TRACE_TYPE_LINE)
		m_CurrentTraceInput->fRadius = 0.f;


	{
		m_CurrentTraceOutput->StartOut = true;
		m_CurrentTraceOutput->AllSolid = false;
		m_CurrentTraceOutput->Fraction = 1.f;

		m_pBrushThatWasLastHit = NULL;
		m_CurrentTraceOutput->HitContents = 0;

		//
		m_CurrentTraceOutput->HitFlags = 0;




		/***/
	
		
		//if (m_iTempCurrentTracedEntity!=-2) // -2 means don't check against dynamic entities (-1 means that this wasn't caused by a dynamic entity)
		// i have to add some boolean that means "don't check against dynamic entities or sth like that,
		// to replace the last line
		if (bCheckDynamicEntities)
		{
			for (int e=0;e<m_iDynamicEntitiesNum;e++)
			{

				//if (m_iTempCurrentTracedEntity>=0)
				if (m_pTempCurrentTracedEntity==m_ppAllDynamicEntities[e]) // we don't wanna check against the same entity ;)
					continue;

				//m_ppAllDynamicEntities[e]->m_uiCDFrame=st_Renderer::Inst()->m_uiFrameCounter;

				if (m_ppAllDynamicEntities[e]->m_bActive)
				if (m_ppAllDynamicEntities[e]->m_dwDynamicEntityType!=DYNAMIC_ENTITY_TYPE_PROJECTILE) // we don't want to aim at projectiles
				if (CheckBrush(m_ppAllDynamicEntities[e]->m_pBrush))
				{
					//if (m_iTempCurrentTracedEntity>=0)
					if (m_pTempCurrentTracedEntity)
					{
						// caused by player
						if (m_pTempCurrentTracedEntity->m_dwDynamicEntityType == DYNAMIC_ENTITY_TYPE_PLAYER)
						{
							// to a player
							if (m_ppAllDynamicEntities[e]->m_dwDynamicEntityType == DYNAMIC_ENTITY_TYPE_PLAYER)
							{
								CVector3f vPlayerPushPlayer= (input->vRayEnd - input->vRayStart).Normalize();
								if (!m_ppAllDynamicEntities[e]->m_pPlayerClient->m_bDying && !m_ppAllDynamicEntities[e]->m_pPlayerClient->m_bStartDie)
									m_ppAllDynamicEntities[e]->m_pPlayerClient->GetPhysicsModel()->m_vOutsideForce+= vPlayerPushPlayer * 10.f;
							}

						}
						else 
						// caused by a projectile
						if (m_pTempCurrentTracedEntity->m_dwDynamicEntityType == DYNAMIC_ENTITY_TYPE_PROJECTILE)
						{
							// to a player
							if (m_ppAllDynamicEntities[e]->m_dwDynamicEntityType == DYNAMIC_ENTITY_TYPE_PLAYER)
							{

								//direct projectile hit damage
								if (  ((CEntityProjectile*)m_pTempCurrentTracedEntity)->m_dwProjectileType == PROJECTILE_TYPE_ROCKET)
								{
									m_ppAllDynamicEntities[e]->m_pPlayerClient->m_iLife-=50*m_ppAllDynamicEntities[e]->m_pPlayerClient->m_fReduceDamageFactor;
									m_ppAllDynamicEntities[e]->m_pPlayerClient->PlayPainSound(50*m_ppAllDynamicEntities[e]->m_pPlayerClient->m_fReduceDamageFactor);
								}
								else
								if (  ((CEntityProjectile*)m_pTempCurrentTracedEntity)->m_dwProjectileType == PROJECTILE_TYPE_PLASMA)																							
								{
									m_ppAllDynamicEntities[e]->m_pPlayerClient->m_iLife-=8*m_ppAllDynamicEntities[e]->m_pPlayerClient->m_fReduceDamageFactor;
									m_ppAllDynamicEntities[e]->m_pPlayerClient->PlayPainSound(8*m_ppAllDynamicEntities[e]->m_pPlayerClient->m_fReduceDamageFactor);
								}


								if (  ((CEntityProjectile*)m_pTempCurrentTracedEntity)->m_dwProjectileType == PROJECTILE_TYPE_ROCKET)																							
								{
									CVector3f vExplosionPushPlayer = (m_ppAllDynamicEntities[m_pBrushThatWasLastHit->m_iAbsoluteEntityIndex]->m_vPos - m_pTempCurrentTracedEntity->m_vPos).Normalize();
									m_ppAllDynamicEntities[e]->m_pPlayerClient->GetPhysicsModel()->m_vOutsideForce+= vExplosionPushPlayer * 200.f;
								}

								((CEntityProjectile*)m_pTempCurrentTracedEntity)->m_bLeaveNoMark = true;
								m_pTempCurrentTracedEntity->OnTouch();
							}


						}
					}	
									
				}
			}
		}
		


		/**/



		// DEBUG DEBUG DEBUG
		//if (!m_pBrushThatWasLastHit)
		CheckMoveByNode(0,0.f,1.f,m_CurrentTraceInput->vRayStart,m_CurrentTraceInput->vRayEnd,bActivateEntities,0);


		


		//BREAKPOINT(m_CurrentTraceOutput->HitContents & CONTENTS_SLIME);

		if (m_pBrushThatWasLastHit)
			{

				if (m_pBrushThatWasLastHit->textureID>=0) // shows we've hit a "regular" map brush
				{
					m_CurrentTraceOutput->HitContents |= m_pTextures[m_pBrushThatWasLastHit->textureID].contents;
					m_CurrentTraceOutput->HitFlags    |= m_pTextures[m_pBrushThatWasLastHit->textureID].flags;


					// if a projectile hit a regular map brush

					if (m_pTempCurrentTracedEntity)
					{
						if (m_pTempCurrentTracedEntity->m_dwDynamicEntityType == DYNAMIC_ENTITY_TYPE_PROJECTILE)
						{
							((CEntityProjectile*)m_pTempCurrentTracedEntity)->m_vNormalAtCollision = m_CurrentTraceOutput->CollisionPlane.n;

							// only leave a mark if the surface hit "allows" it
							if (m_CurrentTraceOutput->HitFlags &SURF_NOMARKS)
								((CEntityProjectile*)m_pTempCurrentTracedEntity)->m_bLeaveNoMark = true;
							else
								((CEntityProjectile*)m_pTempCurrentTracedEntity)->m_bLeaveNoMark = false;
							m_pTempCurrentTracedEntity->OnTouch();
						}
					}
					else // if wasn't caused by an entity, but by a TRACE_CAUSED_BY_SHOOT
					{
						if (m_CurrentTraceInput->dwTraceCausedBy == TRACE_CAUSED_BY_SHOOT)					
						if (m_CurrentTraceInput->iAbsoluteEntityIndexCause>=0)
						if (m_ppAllDynamicEntities[m_CurrentTraceInput->iAbsoluteEntityIndexCause]->m_pPlayerClient->m_bStartAttack)					
						if (m_ppAllDynamicEntities[m_CurrentTraceInput->iAbsoluteEntityIndexCause]->m_pPlayerClient->m_dwCurrentlyHeldWeapon == PLAYER_HOLDING_WEAPON_SHOTGUN)
						{
							/**/ // add the effect of bullet hit something

							CEntity* pEntity;

							GetFreeProjectileEntity(pEntity,PROJECTILE_TYPE_BULLET); // recycle/allocate a projectile
	
							pEntity->m_bActive=false; // cause it to go to explosion immidiatly
							pEntity->m_pPhysicsModel->SetPosition(m_CurrentTraceOutput->EndPoint);
	
							pEntity->m_pPhysicsModel->ChangePositionNow(true);

							pEntity->m_vPos = m_CurrentTraceOutput->EndPoint;
                                		
							((CEntityProjectile*)pEntity)->m_vTryMoveDirection.Set(0.f,0.f,0.f);

							((CEntityProjectile*)pEntity)->m_vNormalAtCollision = m_CurrentTraceOutput->CollisionPlane.n;

							pEntity->OnTouch();

							/**/
						}
								

					}

				}
				else
				{
					// here put the entity that we collided with
					// a cool "trick" might be to put in the "contents" the absolute ID to the dynamic entity we've hit
					// hmm, actually adding another field would be better

					m_CurrentTraceOutput->iAbsoluteEntityIndexHit = m_pBrushThatWasLastHit->m_iAbsoluteEntityIndex;
					
					
					if (m_CurrentTraceInput->dwTraceCausedBy == TRACE_CAUSED_BY_SHOOT)					
					if (m_CurrentTraceInput->iAbsoluteEntityIndexCause>=0)
					if (m_ppAllDynamicEntities[m_CurrentTraceInput->iAbsoluteEntityIndexCause]->m_pPlayerClient->m_bStartAttack)					
					if (m_ppAllDynamicEntities[m_CurrentTraceInput->iAbsoluteEntityIndexCause]->m_pPlayerClient->m_dwCurrentlyHeldWeapon == PLAYER_HOLDING_WEAPON_SHOTGUN)
					{						
						if (m_pBrushThatWasLastHit->m_iAbsoluteEntityIndex>=0)
						{
							if (m_ppAllDynamicEntities[m_pBrushThatWasLastHit->m_iAbsoluteEntityIndex]->m_dwDynamicEntityType == DYNAMIC_ENTITY_TYPE_PLAYER)
							{						
								m_ppAllDynamicEntities[m_pBrushThatWasLastHit->m_iAbsoluteEntityIndex]->m_pPlayerClient->m_iLife-=4*m_ppAllDynamicEntities[m_pBrushThatWasLastHit->m_iAbsoluteEntityIndex]->m_pPlayerClient->m_fReduceDamageFactor;
								m_ppAllDynamicEntities[m_pBrushThatWasLastHit->m_iAbsoluteEntityIndex]->m_pPlayerClient->PlayPainSound(4*m_ppAllDynamicEntities[m_pBrushThatWasLastHit->m_iAbsoluteEntityIndex]->m_pPlayerClient->m_fReduceDamageFactor);

								CVector3f vShotPushPlayer= (input->vRayEnd - input->vRayStart).Normalize();
								m_ppAllDynamicEntities[m_pBrushThatWasLastHit->m_iAbsoluteEntityIndex]->m_pPlayerClient->GetPhysicsModel()->m_vOutsideForce+= vShotPushPlayer * 50.f;
								
							}
						}

						
					}
				}

						

				if (m_pTempCurrentTracedEntity)
				{

					if (m_pTempCurrentTracedEntity->m_dwDynamicEntityType == DYNAMIC_ENTITY_TYPE_PLAYER)
					if (m_pTempCurrentTracedEntity->m_pPlayerClient->GetPhysicsModel())
					{
						m_pTempCurrentTracedEntity->m_pPlayerClient->GetPhysicsModel()->m_iContentsOfTouchedGround |= m_CurrentTraceOutput->HitContents;
						m_pTempCurrentTracedEntity->m_pPlayerClient->GetPhysicsModel()->m_iFlagsOfTouchedGround |= m_CurrentTraceOutput->HitFlags;
					}
				}

			}



		// DEBUG DEBUG DEBUG
		/*if (m_CurrentTraceOutput->StartOut)
		{
			m_CurrentTraceOutput->EndPoint = m_CurrentTraceInput->vRayStart;
		}
		else				*/
		if (m_CurrentTraceOutput->Fraction==1)
			m_CurrentTraceOutput->EndPoint = m_CurrentTraceInput->vRayEnd;
		else
		{
			//m_CurrentTraceOutput->Fraction-=EPSILON;
			m_CurrentTraceOutput->EndPoint = m_CurrentTraceInput->vRayStart + 
											(m_CurrentTraceInput->vRayEnd-m_CurrentTraceInput->vRayStart)*m_CurrentTraceOutput->Fraction;
		}
		
	}


	

}




/////////////////////////// CheckNode \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
////// recurse check the bsp tree by nodes
/////////////////////////// CheckNode \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void CQuake3BSP::CheckMoveByNode(int iNodeIndex, float fStartFraction, float fEndFraction, CVector3f vStart, CVector3f vEnd,bool bActivateEntities,int iRecurseDepth)
{	
	iRecurseDepth++;
	if (iRecurseDepth>20)
		return;

	// this doesn't work for some reason
	/*if (fStartFraction > m_CurrentTraceOutput->Fraction)
	{
		return;
	}*/

	if (iNodeIndex < 0) // a leaf
	{

		int iLeafNum = -(iNodeIndex+1);

		tBSPLeaf* leaf = &m_pLeafs[iLeafNum];


		/*
		// RESTORE RESTORE RESTORE
		if (m_iTempCurrentTracedEntity>=0)
		{
			for (int e=0;e<m_iDynamicEntitiesNum;e++)
			{

				if (m_iTempCurrentTracedEntity>=0)
				if (m_iTempCurrentTracedEntity==e) // we don't wanna check against the same entity ;)
					continue;

				bool bEntityIsInThisLeaf = false;

				for (int l=0;l<m_ppAllDynamicEntities[e]->inLeafs.size();l++)
				{
					if (m_ppAllDynamicEntities[e]->inLeafs[l]==iLeafNum)
					{
						bEntityIsInThisLeaf = true;
						break;
					}

				}

				if (bEntityIsInThisLeaf)
				{
					m_ppAllDynamicEntities[e]->m_uiCDFrame=st_Renderer::Inst()->m_uiFrameCounter;
					CheckBrush(m_ppAllDynamicEntities[e]->m_pBrush);
				}
			}
		}*/


		tBSPBrush* brush;

		int i;




		if (bActivateEntities)
		for (i=0;i<leaf->StaticEntities.size();i++)
		{
									

			int iModelNum = ((CEntityJumpPad*) leaf->StaticEntities[i])->iModelNum; // hmm

			for (int j=0;j<m_pModels[iModelNum].numOfBrushes;j++)
			{

				if (iModelNum>=0)
				{

					brush = &m_pBrushes[m_pModels[iModelNum].brushIndex + j ];
					if (CheckBrush(brush))
					{
	
	/*					if (leaf->StaticEntities[i]->m_fCollidedTime < st_Timer::Inst()->TimerGetTimeFAKE())
							leaf->StaticEntities[i]->m_fCollidedTime =  st_Timer::Inst()->TimerGetTimeFAKE();
						else
							continue;*/

						g_bCollidedWithStaticEntity = true;
						leaf->StaticEntities[i]->OnTouched();

						break;				
					}
				}
				else // there is no brush related to this model (an MD3 model for example)
				{
					leaf->StaticEntities[i]->OnTouched();
					break;				
				}

				// hmm - i need to consider a case that one entity is blocking the other,
				// and to NOT activate her in that case...
				// (i just need to simply store a brush, and only perform the entity activation in the END of this function)

			}

		}

		//if (!g_bCollidedWithStaticEntity)
		{			

			// check collision against "regular" brushes

			for (i=0; i < leaf->numOfLeafBrushes ; i++)
			{
					brush = &m_pBrushes[m_pLeafBrushes[leaf->leafBrush + i]];
	
	
					if (m_CurrentTraceInput->dwTraceCausedBy == TRACE_CAUSED_BY_PLAYER) // it's a player
					{
						
						if (brush->numOfBrushSides>0 &&
							m_pTextures[brush->textureID].contents & (CONTENTS_SOLID | CONTENTS_PLAYERCLIP))
						{
							CheckBrush(brush);
						}
					}
					else
					if (m_CurrentTraceInput->dwTraceCausedBy == TRACE_CAUSED_BY_PROJECTILE) // it's a projectile
					{
						
						if (brush->numOfBrushSides>0 &&
							m_pTextures[brush->textureID].contents & (CONTENTS_SOLID))
						{
							CheckBrush(brush);
						}
					}
					else
					if (m_CurrentTraceInput->dwTraceCausedBy == TRACE_CAUSED_BY_BOT) // it's a bot
					{
						if (brush->numOfBrushSides>0 && 
						(m_pTextures[brush->textureID].contents & CONTENTS_SOLID) || (m_pTextures[brush->textureID].contents & CONTENTS_PLAYERCLIP) )
						{
							CheckBrush(brush);
						}
					}
					else
					if (m_CurrentTraceInput->dwTraceCausedBy == TRACE_CAUSED_BY_SHOOT) // it's a gun shot
					{
						if (brush->numOfBrushSides>0 && m_pTextures[brush->textureID].contents & CONTENTS_SOLID)
						{
							CheckBrush(brush);
						}
					}
					else
					if (m_CurrentTraceInput->dwTraceCausedBy == TRACE_CAUSED_BY_TEST) // it's a test
					{
						if (brush->numOfBrushSides>0)
						{
							CheckBrush(brush);
						}
					}

					
			}

			

			// ToDo: check collision against bezier patches "artifical" brushes

			/*
			for (i=0; i < leaf->numOfBezierLeafBrushes ; i++)
			{
					brush = &m_pBezierBrushes[m_BezierLeafBrushes[leaf->leafBezierBrush + i]];
	
					if (m_CurrentTraceInput->dwTraceCausedBy == TRACE_CAUSED_BY_PLAYER) // it's a player
					{
						if (brush->numOfBezierBrushSides>0 && 
						(m_pTextures[brush->textureID].contents & CONTENTS_SOLID) || (m_pTextures[brush->textureID].contents & CONTENTS_PLAYERCLIP) )
						CheckBrush(brush);
					}
					else
					if (m_CurrentTraceInput->dwTraceCausedBy == TRACE_CAUSED_BY_SHOOT) // it's a gun shot
					{
						if (brush->numOfBezierBrushSides>0 && (m_pTextures[brush->textureID].contents & CONTENTS_SOLID))
						CheckBrush(brush);
					}
			}
			*/


		}

		return; // nothing left to do
	}

	// a node

	tBSPNode  *node  = &m_pNodes[iNodeIndex];
	//tBSPPlane *plane = &m_pPlanesORIGINAL[node->plane];
	CPlane *plane = &m_pPlanes[node->plane];

	// distance from the start and end point to the plane

	float fStartDistance = (plane->n * vStart) - plane->d;
	float fEndDistance   = (plane->n * vEnd)   - plane->d;
    
	
	if (fStartDistance >= m_CurrentTraceInput->fRadius && fEndDistance >= m_CurrentTraceInput->fRadius) // both are infront
		CheckMoveByNode(node->front, fStartFraction, fEndFraction, vStart, vEnd,bActivateEntities,0);
	else
	if (fStartDistance < -m_CurrentTraceInput->fRadius && fEndDistance < -m_CurrentTraceInput->fRadius)  // both are behind
		CheckMoveByNode(node->back,fStartFraction,fEndFraction,vStart,vEnd,bActivateEntities,0);
	else // moving from one side to another
	{
		int side;
		float fraction1,fraction2,middleFraction;
		CVector3f vMiddle;

		if (fStartDistance < fEndDistance)
		{
			side = 1; // back
			float inverseDistance = 1.f / (fStartDistance - fEndDistance);
			fraction1 = (fStartDistance - m_CurrentTraceInput->fRadius - EPSILON) * inverseDistance;
			fraction2 = (fStartDistance + m_CurrentTraceInput->fRadius + EPSILON) * inverseDistance;
		}
		else
		if (fEndDistance < fStartDistance)
		{
			side = 0; // front
			float inverseDistance = 1.f/(fStartDistance - fEndDistance);
			fraction1 = (fStartDistance + m_CurrentTraceInput->fRadius + EPSILON) * inverseDistance;
			fraction2 = (fStartDistance - m_CurrentTraceInput->fRadius - EPSILON) * inverseDistance;
		}
		else
		{
			side = 0; //front
			fraction1 = 1.f;
			fraction2 = 0.f;
		}


		// step 2
		if (fraction1 < 0.f) 
			fraction1 = 0.f;
		else if (fraction1 > 1.f) 
			fraction1 = 1.f;

		if (fraction2 < 0.f) 
			fraction2 = 0.f;
		else if (fraction2 > 1.f) 
			fraction2 = 1.f;


		// step 3
		middleFraction = fStartDistance + (fEndDistance - fStartFraction) * fraction1;
		vMiddle = vStart + ((vEnd-vStart)*fraction1);

		// step 4
		if (side==1) // back
			CheckMoveByNode(node->back,fStartFraction,middleFraction,vStart,vMiddle,bActivateEntities,0);
		else  // front
			CheckMoveByNode(node->front,fStartFraction,middleFraction,vStart,vMiddle,bActivateEntities,0);

		// step 5

		middleFraction = fStartFraction + (fEndFraction - fStartFraction)*fraction2;
		vMiddle = vStart + ((vEnd-vStart)*fraction2);

		// step 6

		if (side==0) // back-->front
			CheckMoveByNode(node->back,middleFraction,fEndFraction,vMiddle,vEnd,bActivateEntities,0);
		else  // front-->back
			CheckMoveByNode(node->front,middleFraction,fEndFraction,vMiddle,vEnd,bActivateEntities,0);
	}

}



///////////////////////////// CheckBrush \\\\\\\\\\\\\\\\\\\\\\\\\*
////// check against the brush
///////////////////////////// CheckBrush \\\\\\\\\\\\\\\\\\\\\\\\\*
bool CQuake3BSP::CheckBrush(tBSPBrush *brush)
{
	bool bDidCollide=false;

	float fStartFraction = -1.f;
	float fEndFraction   = 1.f;
	bool bStartsOut = false;
	bool bEndsOut = false;



	int iSavedBrushSideNum = 0;

	CVector3f vCandidateToHitNormal;

	for (int i=0;i<brush->numOfBrushSides;i++)
	{

		CPlane     *plane;

		if (brush->m_pPlanes) // a dynamic entity brush
		{
			plane     = &(brush->m_pPlanes[i]);
		}
		else // a map brush
		{
			tBSPBrushORIGINALSide *brushSide = &m_pBrushSides[brush->brushSide + i];						
			plane     = &m_pPlanes[brushSide->plane];
		}
		
		float fStartDistance,fEndDistance;

		if (m_CurrentTraceInput->dwTraceType==TRACE_TYPE_LINE)
		{
			fStartDistance = (plane->n*m_CurrentTraceInput->vRayStart) - plane->d;
			fEndDistance   = (plane->n*m_CurrentTraceInput->vRayEnd)   - plane->d;
		}
		else
		if (m_CurrentTraceInput->dwTraceType==TRACE_TYPE_SPHERE)
		{
			fStartDistance = (plane->n*m_CurrentTraceInput->vRayStart) - (plane->d + m_CurrentTraceInput->fRadius);
			fEndDistance   = (plane->n*m_CurrentTraceInput->vRayEnd)   - (plane->d + m_CurrentTraceInput->fRadius);
		}
        

		if (fStartDistance>0.f)
			bStartsOut=true;
		if (fEndDistance>0.f)
			bEndsOut = true;


		if (fStartDistance > 0 && fEndDistance > 0)
			return false;
		if (fStartDistance<=0 && fEndDistance<=0)
			continue;

		if (fStartDistance>fEndDistance) // line is entering the brush
		{
			float fraction = (fStartDistance - EPSILON) / (fStartDistance - fEndDistance);

			if (fraction > fStartFraction)
			{
				fStartFraction = fraction;
				iSavedBrushSideNum = i;
			}

	
		}
		else // line is leaving the brush
		{
			float fraction = (fStartDistance + EPSILON) / (fStartDistance - fEndDistance);
			if (fraction < fEndFraction)
			{
				fEndFraction = fraction;
			}

		}

	}

	if (bStartsOut == false)
	{
		m_CurrentTraceOutput->StartOut = false;
		if (bEndsOut == false)
		m_CurrentTraceOutput->AllSolid = true;
		
		
		// debug debug debug
		//m_CurrentTraceOutput->Fraction = 1.f;
		
		return false; //fishy
	}



	if (fStartFraction < fEndFraction)
	{
		if (fStartFraction > -1 && fStartFraction < m_CurrentTraceOutput->Fraction)
		{
			if (fStartFraction < 0)
				fStartFraction = 0;

			m_CurrentTraceOutput->Fraction = fStartFraction;

			CPlane *POTplane;

			if (brush->m_pPlanes) // a dynamic entity brush
			{
				POTplane     = &(brush->m_pPlanes[iSavedBrushSideNum]);
			}
			else // a map brush
			{
				tBSPBrushORIGINALSide *POTbrushSide = &m_pBrushSides[brush->brushSide + iSavedBrushSideNum];
				POTplane = &m_pPlanes[POTbrushSide->plane];
			}

			

			m_CurrentTraceOutput->CollisionPlane.n = POTplane->n;
			m_CurrentTraceOutput->CollisionPlane.d = POTplane->d;

			m_pBrushThatWasLastHit = brush;
			

			bDidCollide = true;
		}
	}

	return bDidCollide;
}

void CQuake3BSP::FillStaticEntities(void)
{

	char* cpDebug;

	for (int i=0;i<st_EntitiesParser::Inst()->m_iEntitiesNum;i++)
	{
		if (st_EntitiesParser::Inst()->m_pEntities[i].uiEntityType==kTriggerPush) // a jump-pad!
		{
			m_ppAllStaticEntities[m_iStaticEntitiesNum] = new CEntityJumpPad;

			((CEntityJumpPad*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->iModelNum = st_EntitiesParser::Inst()->m_pEntities[i].iModelIndex;

			// find and fill target position location
			for (int j=0;j<st_EntitiesParser::Inst()->m_iEntitiesNum;j++)
			{
				if (st_EntitiesParser::Inst()->m_pEntities[j].uiEntityType==kTargetPosition)
					if (strcmp(st_EntitiesParser::Inst()->m_pEntities[j].caTargetName,st_EntitiesParser::Inst()->m_pEntities[i].caTargetName)==0)
					{
						((CEntityJumpPad*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->vTarget = st_EntitiesParser::Inst()->m_pEntities[j].vOrigin;
						break;
					}												
			}

			// find the model BBox center

			m_ppAllStaticEntities[m_iStaticEntitiesNum]->vModelBBoxCenter = m_pModels[  ((CEntityJumpPad*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->iModelNum].min + 
																			m_pModels[  ((CEntityJumpPad*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->iModelNum].max ;

			m_ppAllStaticEntities[m_iStaticEntitiesNum]->vModelBBoxCenter/=2.f;


			/*i need to try to pre-compute the required velocity, since the problem is that i don't get the same velocity when i touch a jump-pad 
				(and i should! since the BBox center and the target position are constant)*/

			CVector3f vJumpPadVelocity;

			vJumpPadVelocity.v[1] = sqrtf(2* ((-g_vGravity.v[1])*1.f)*(((CEntityJumpPad*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->vTarget.v[1] - ((CEntityJumpPad*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->vModelBBoxCenter.v[1]));
			vJumpPadVelocity.v[0] = (((CEntityJumpPad*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->vTarget.v[0] - ((CEntityJumpPad*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->vModelBBoxCenter.v[0]) * ((-g_vGravity.v[1])*1.f) / vJumpPadVelocity.v[1];
			vJumpPadVelocity.v[2] = (((CEntityJumpPad*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->vTarget.v[2] - ((CEntityJumpPad*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->vModelBBoxCenter.v[2]) * ((-g_vGravity.v[1])*1.f)/ vJumpPadVelocity.v[1];

			((CEntityJumpPad*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->vTarget = vJumpPadVelocity;

			m_iStaticEntitiesNum++;
		}
		else
		if (st_EntitiesParser::Inst()->m_pEntities[i].uiEntityType==kTriggerTeleport) // a teleporter!
		{
			m_ppAllStaticEntities[m_iStaticEntitiesNum] = new CEntityTeleporter;

			((CEntityTeleporter*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->iModelNum = st_EntitiesParser::Inst()->m_pEntities[i].iModelIndex;

			for (int j=0;j<st_EntitiesParser::Inst()->m_iEntitiesNum;j++)
			{
				if (st_EntitiesParser::Inst()->m_pEntities[j].uiEntityType==kTargetPosition)
				{

					if (strcmp(st_EntitiesParser::Inst()->m_pEntities[j].caTargetName,st_EntitiesParser::Inst()->m_pEntities[i].caTargetName)==0)
					{
						((CEntityTeleporter*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->vTarget = st_EntitiesParser::Inst()->m_pEntities[j].vOrigin;
						break;
					}												
				}
			}

			m_iStaticEntitiesNum++;
		}
		else
		if (st_EntitiesParser::Inst()->m_pEntities[i].uiEntityType==kTriggerHurt) // trigger hurt!
		{
			m_ppAllStaticEntities[m_iStaticEntitiesNum] = new CEntityHurt;

			((CEntityHurt*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->iModelNum = st_EntitiesParser::Inst()->m_pEntities[i].iModelIndex;

			m_iStaticEntitiesNum++;
		}
		else			
		if (st_EntitiesParser::Inst()->m_pEntities[i].uiEntityType==kAmmoRockets) // ammo rockets
		{
			m_ppAllStaticEntities[m_iStaticEntitiesNum] = new CEntityCollectable;
			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_vPos = st_EntitiesParser::Inst()->m_pEntities[i].vOrigin;

			((CEntityCollectable*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->m_dwItemType = ENTITY_COLLECTABLE_AMMO_ROCKETS;


			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_pModel = new CMD3;
			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_pModel->Load("models/powerups/ammo/rocketam.md3",NULL,NULL,false);

			/**/
			CMatrix m1,m2;
			m1.MatMakeRot(DEG2RAD(-90.f),DEG2RAD(90.f),0.f);				
			m2.MatMakeMove(st_EntitiesParser::Inst()->m_pEntities[i].vOrigin.v[0],st_EntitiesParser::Inst()->m_pEntities[i].vOrigin.v[1],st_EntitiesParser::Inst()->m_pEntities[i].vOrigin.v[2]);
			m1 = m2 * m1;
			float pfOpenglMatrix[16];
			m1.ConvertToOpenglMatrix(&pfOpenglMatrix[0]);
			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_pModel->Update(true,false,&pfOpenglMatrix[0]);
			/**/

			m_iStaticEntitiesNum++;
		}
		else
		if (st_EntitiesParser::Inst()->m_pEntities[i].uiEntityType==kAmmoBullets) // ammo bullets
		{
			m_ppAllStaticEntities[m_iStaticEntitiesNum] = new CEntityCollectable;
			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_vPos = st_EntitiesParser::Inst()->m_pEntities[i].vOrigin;

			((CEntityCollectable*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->m_dwItemType = ENTITY_COLLECTABLE_AMMO_BULLETS;


			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_pModel = new CMD3;
			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_pModel->Load("models/powerups/ammo/machinegunam.md3",NULL,NULL,false);

			/**/
			CMatrix m1,m2;
			m1.MatMakeRot(DEG2RAD(-90.f),DEG2RAD(90.f),0.f);				
			m2.MatMakeMove(st_EntitiesParser::Inst()->m_pEntities[i].vOrigin.v[0],st_EntitiesParser::Inst()->m_pEntities[i].vOrigin.v[1],st_EntitiesParser::Inst()->m_pEntities[i].vOrigin.v[2]);
			m1 = m2 * m1;
			float pfOpenglMatrix[16];
			m1.ConvertToOpenglMatrix(&pfOpenglMatrix[0]);
			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_pModel->Update(true,false,&pfOpenglMatrix[0]);
			/**/

			m_iStaticEntitiesNum++;
		}
		else
		if (st_EntitiesParser::Inst()->m_pEntities[i].uiEntityType==kAmmoShells) // ammo shells
		{
			m_ppAllStaticEntities[m_iStaticEntitiesNum] = new CEntityCollectable;
			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_vPos = st_EntitiesParser::Inst()->m_pEntities[i].vOrigin;

			((CEntityCollectable*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->m_dwItemType = ENTITY_COLLECTABLE_AMMO_SHELLS;

			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_pModel = new CMD3;
			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_pModel->Load("models/powerups/ammo/plasmaam.md3",NULL,NULL,false);

			/**/
			CMatrix m1,m2;
			m1.MatMakeRot(DEG2RAD(-90.f),DEG2RAD(90.f),0.f);				
			m2.MatMakeMove(st_EntitiesParser::Inst()->m_pEntities[i].vOrigin.v[0],st_EntitiesParser::Inst()->m_pEntities[i].vOrigin.v[1],st_EntitiesParser::Inst()->m_pEntities[i].vOrigin.v[2]);
			m1 = m2 * m1;
			float pfOpenglMatrix[16];
			m1.ConvertToOpenglMatrix(&pfOpenglMatrix[0]);
			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_pModel->Update(true,false,&pfOpenglMatrix[0]);
			/**/

			m_iStaticEntitiesNum++;
		}
		else
		if (st_EntitiesParser::Inst()->m_pEntities[i].uiEntityType==kArmorBody) // armor body
		{
			m_ppAllStaticEntities[m_iStaticEntitiesNum] = new CEntityCollectable;
			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_vPos = st_EntitiesParser::Inst()->m_pEntities[i].vOrigin;

			((CEntityCollectable*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->m_dwItemType = ENTITY_COLLECTABLE_ARMOR_BODY;

			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_pModel = new CMD3;
			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_pModel->Load("models/powerups/armor/armor_red.md3",NULL,NULL,false);

			/**/
			CMatrix m1,m2;
			m1.MatMakeRot(DEG2RAD(-90.f),DEG2RAD(90.f),0.f);				
			m2.MatMakeMove(st_EntitiesParser::Inst()->m_pEntities[i].vOrigin.v[0],st_EntitiesParser::Inst()->m_pEntities[i].vOrigin.v[1],st_EntitiesParser::Inst()->m_pEntities[i].vOrigin.v[2]);
			m1 = m2 * m1;
			float pfOpenglMatrix[16];
			m1.ConvertToOpenglMatrix(&pfOpenglMatrix[0]);
			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_pModel->Update(true,false,&pfOpenglMatrix[0]);
			/**/

			m_iStaticEntitiesNum++;
		}
		else
		if (st_EntitiesParser::Inst()->m_pEntities[i].uiEntityType==kHealth) // item health
		{
			m_ppAllStaticEntities[m_iStaticEntitiesNum] = new CEntityCollectable;
			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_vPos = st_EntitiesParser::Inst()->m_pEntities[i].vOrigin;

			((CEntityCollectable*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->m_dwItemType = ENTITY_COLLECTABLE_HEALTH;

			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_pModel = new CMD3;
			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_pModel->Load("models/powerups/health/medium_cross.md3",NULL,NULL,false);

			/**/
			CMatrix m1,m2;
			m1.MatMakeRot(DEG2RAD(-90.f),DEG2RAD(90.f),0.f);				
			m2.MatMakeMove(st_EntitiesParser::Inst()->m_pEntities[i].vOrigin.v[0],st_EntitiesParser::Inst()->m_pEntities[i].vOrigin.v[1],st_EntitiesParser::Inst()->m_pEntities[i].vOrigin.v[2]);
			m1 = m2 * m1;
			float pfOpenglMatrix[16];
			m1.ConvertToOpenglMatrix(&pfOpenglMatrix[0]);
			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_pModel->Update(true,false,&pfOpenglMatrix[0]);
			/**/
            //st_Timer::Inst()->TimerGetTimeFAKE()

			m_iStaticEntitiesNum++;
		}
		else
		if (st_EntitiesParser::Inst()->m_pEntities[i].uiEntityType==kArmorShard) // item armor shard
		{
			m_ppAllStaticEntities[m_iStaticEntitiesNum] = new CEntityCollectable;
			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_vPos = st_EntitiesParser::Inst()->m_pEntities[i].vOrigin;

			((CEntityCollectable*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->m_dwItemType = ENTITY_COLLECTABLE_ARMOR_SHARD;

			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_pModel = new CMD3;
			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_pModel->Load("models/powerups/armor/shard.md3",NULL,NULL,false);

			/**/
			CMatrix m1,m2;
			m1.MatMakeRot(DEG2RAD(-90.f),DEG2RAD(90.f),0.f);				
			m2.MatMakeMove(st_EntitiesParser::Inst()->m_pEntities[i].vOrigin.v[0],st_EntitiesParser::Inst()->m_pEntities[i].vOrigin.v[1],st_EntitiesParser::Inst()->m_pEntities[i].vOrigin.v[2]);
			m1 = m2 * m1;
			float pfOpenglMatrix[16];
			m1.ConvertToOpenglMatrix(&pfOpenglMatrix[0]);
			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_pModel->Update(true,false,&pfOpenglMatrix[0]);
			/**/
            //st_Timer::Inst()->TimerGetTimeFAKE()

			m_iStaticEntitiesNum++;
		}
		else
		if (st_EntitiesParser::Inst()->m_pEntities[i].uiEntityType==kCombatArmor) // item armor shard
		{
			m_ppAllStaticEntities[m_iStaticEntitiesNum] = new CEntityCollectable;
			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_vPos = st_EntitiesParser::Inst()->m_pEntities[i].vOrigin;

			((CEntityCollectable*) m_ppAllStaticEntities[m_iStaticEntitiesNum])->m_dwItemType = ENTITY_COLLECTABLE_COMBAT_ARMOR;

			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_pModel = new CMD3;
			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_pModel->Load("models/powerups/armor/armor_yel.md3",NULL,NULL,false);

			/**/
			CMatrix m1,m2;
			m1.MatMakeRot(DEG2RAD(-90.f),DEG2RAD(90.f),0.f);				
			m2.MatMakeMove(st_EntitiesParser::Inst()->m_pEntities[i].vOrigin.v[0],st_EntitiesParser::Inst()->m_pEntities[i].vOrigin.v[1],st_EntitiesParser::Inst()->m_pEntities[i].vOrigin.v[2]);
			m1 = m2 * m1;
			float pfOpenglMatrix[16];
			m1.ConvertToOpenglMatrix(&pfOpenglMatrix[0]);
			m_ppAllStaticEntities[m_iStaticEntitiesNum]->m_pModel->Update(true,false,&pfOpenglMatrix[0]);
			/**/
            //st_Timer::Inst()->TimerGetTimeFAKE()

			m_iStaticEntitiesNum++;
		}



		
	}
}

void CQuake3BSP::UpdateAllStaticEntities(void)
{
	for (int i=0;i<m_iStaticEntitiesNum;i++)
	{
		(m_ppAllStaticEntities[i])->Update();
	}

}

void CQuake3BSP::LinkAllStaticEntitiesToLeafs(void)
{
	//assert(_CrtCheckMemory());

	for (int i=0;i<m_iStaticEntitiesNum;i++)
	{
		for (int j=0;j<m_numOfLeafs;j++)
		{			

			if (( m_ppAllStaticEntities[i])->iModelNum>=0)
			{
				if (TestModelAgainstLeaf(( m_ppAllStaticEntities[i])->iModelNum,j))
				{
					m_pLeafs[j].StaticEntities.push_back(m_ppAllStaticEntities[i]);
				}
			}
			else // no model num, so must be a MD3
			{

//				BREAKPOINT(j==603);

				if (!  (m_ppAllStaticEntities[i])->m_pModel)
					continue; // no md3 model exists here

				CVector3f vModelMin,vModelMax;				
				vModelMin.Set((m_ppAllStaticEntities[i])->m_pModel->m_pMD3Shared->m_pBones[0].mins[0],(m_ppAllStaticEntities[i])->m_pModel->m_pMD3Shared->m_pBones[0].mins[2],-(m_ppAllStaticEntities[i])->m_pModel->m_pMD3Shared->m_pBones[0].mins[1]);
				vModelMax.Set((m_ppAllStaticEntities[i])->m_pModel->m_pMD3Shared->m_pBones[0].maxs[0],(m_ppAllStaticEntities[i])->m_pModel->m_pMD3Shared->m_pBones[0].maxs[2],-(m_ppAllStaticEntities[i])->m_pModel->m_pMD3Shared->m_pBones[0].maxs[1]);
				
				vModelMin+=(m_ppAllStaticEntities[i])->m_vPos;
				vModelMax+=(m_ppAllStaticEntities[i])->m_vPos;

				// Here i should put TestBBoxAgainstLeaf()
				if (TestBBoxAgainstLeaf(vModelMin,vModelMax,j))
					m_pLeafs[j].StaticEntities.push_back(m_ppAllStaticEntities[i]);
			}

		}
	}

}

#define PLAYER_BOUNDING_SPHERE_SIZE 23.f

void CQuake3BSP::FillDynamicEntities(CPlayerClient* pPlayer,CPlayerClient* pOponent)
{



	/************************************************/
	// adding players

	m_iDynamicEntitiesNum = 0;

	// player1

	m_ppAllDynamicEntities[m_iDynamicEntitiesNum] = new CEntityPlayer;
	
	m_ppAllDynamicEntities[m_iDynamicEntitiesNum]->m_iAbsoluteEntitiesIndex = m_iDynamicEntitiesNum;
	//((CEntityPlayer*)m_ppAllDynamicEntities[m_iDynamicEntitiesNum])->m_pPlayerClient->m_pPlayerModel->Load("models/players/grunt/");
	((CEntityPlayer*)m_ppAllDynamicEntities[m_iDynamicEntitiesNum])->m_pPlayerClient = pPlayer;	
	
	
    

	(m_ppAllDynamicEntities[m_iDynamicEntitiesNum])->m_pBrush->m_iAbsoluteEntityIndex = m_iDynamicEntitiesNum;
	(m_ppAllDynamicEntities[m_iDynamicEntitiesNum])->m_pPlayerClient->m_iAbsoluteEntityIndex = m_iDynamicEntitiesNum;



	g_pPlayerClient1 = ((CEntityPlayer*)m_ppAllDynamicEntities[m_iDynamicEntitiesNum])->m_pPlayerClient;

	g_pPlayerClient1->GetPhysicsModel()->Init(PLAYER_BOUNDING_SPHERE_SIZE);

	g_pPlayerClient1->InitStates();
	
	
	if (g_pCheckersBoard->m_iCurrentPlayerSide==ARMY_SIDE_PAST) // past is the attacker
	{
		if (g_pCheckersBoard->m_iAdvantagePAST==ADVANTAGE_WEAPON)
		{
			g_pPlayerClient1->m_fShotRateFactor=1.5f;
		}
		else
		if (g_pCheckersBoard->m_iAdvantagePAST==ADVANTAGE_EXTRALIFE)
		{
			g_pPlayerClient1->m_iLife+=50;
			g_pPlayerClient1->m_fShotRateFactor=1.f;
		}
		else
		if (g_pCheckersBoard->m_iAdvantagePAST==ADVANTAGE_AMMO)
		{
			g_pPlayerClient1->m_bInfiniteAmmo=true;
		}

	}
	else // future is the attacker
	{
		if (g_pCheckersBoard->m_iAdvantageFUTURE==ADVANTAGE_WEAPON)
		{
			g_pPlayerClient1->m_fShotRateFactor=1.5f;
		}
		else
		{
			g_pPlayerClient1->m_iLife+=50;
			g_pPlayerClient1->m_fShotRateFactor=1.f;
		}


	}
	
	if (g_Communication.GetAlreadyInited() && g_Communication.GetCommunicationType()==COMMUNICATION_CLIENT)
	{
		g_pPlayerClient1->AdvanceStartPosition();
	}
	else
	{
		g_pPlayerClient1->AdvanceStartPosition();
		g_pPlayerClient1->AdvanceStartPosition();
	}
	g_pPlayerClient1->PutPlayerInStartPos();
	//g_pPlayerClient1->m_PhysicsModel.SetPosition(CVector3f(1000.f,25.f+20.f,-1468.f));


	(m_ppAllDynamicEntities[m_iDynamicEntitiesNum])->m_pBot->SetBotPlayerClient((m_ppAllDynamicEntities[m_iDynamicEntitiesNum])->m_pPlayerClient);

			
	m_iDynamicEntitiesNum++;
	m_iPlayersNum++;



	// player2

	m_ppAllDynamicEntities[m_iDynamicEntitiesNum] = new CEntityPlayer;
	//m_ppAllDynamicEntities[m_iDynamicEntitiesNum] = pOponent;
	
	m_ppAllDynamicEntities[m_iDynamicEntitiesNum]->m_iAbsoluteEntitiesIndex = m_iDynamicEntitiesNum;
	
	//((CEntityPlayer*)m_ppAllDynamicEntities[m_iDynamicEntitiesNum])->m_pPlayerClient->m_pPlayerModel->Load("models/players/major/");
	((CEntityPlayer*)m_ppAllDynamicEntities[m_iDynamicEntitiesNum])->m_pPlayerClient = pOponent;
	
	

	(m_ppAllDynamicEntities[m_iDynamicEntitiesNum])->m_pBrush->m_iAbsoluteEntityIndex = m_iDynamicEntitiesNum;
	(m_ppAllDynamicEntities[m_iDynamicEntitiesNum])->m_pPlayerClient->m_iAbsoluteEntityIndex = m_iDynamicEntitiesNum;
	
	g_pPlayerClient2 = ((CEntityPlayer*)m_ppAllDynamicEntities[m_iDynamicEntitiesNum])->m_pPlayerClient;

	g_pPlayerClient2->GetPhysicsModel()->Init(PLAYER_BOUNDING_SPHERE_SIZE);

	g_pPlayerClient2->InitStates();
							

	if (g_Communication.GetAlreadyInited() && g_Communication.GetCommunicationType()==COMMUNICATION_CLIENT)
	{
		g_pPlayerClient1->AdvanceStartPosition();
		g_pPlayerClient1->AdvanceStartPosition();
	}
	else
	{
		g_pPlayerClient1->AdvanceStartPosition();		
	}
	
	g_pPlayerClient2->PutPlayerInStartPos();
	//g_pPlayerClient2->m_PhysicsModel.SetPosition(CVector3f(212.f,73.f+20.f,-2361.f));

	(m_ppAllDynamicEntities[m_iDynamicEntitiesNum])->m_pBot->SetBotPlayerClient((m_ppAllDynamicEntities[m_iDynamicEntitiesNum])->m_pPlayerClient);

	m_iDynamicEntitiesNum++;
	m_iPlayersNum++;


	g_pCurrentlyProcessedPlayerClient = g_pPlayerClient1;
	g_pHumanPlayerClient = g_pPlayerClient1;
	/*g_pPlayerClient1->SetPlayerWeapon(2);
	g_pPlayerClient2->SetPlayerWeapon(3);*/
	g_pCurrentlyProcessedBot          = g_pCurrentlyProcessedBot1;

	/************************************************/




	/************************************************/
	// adding test projectiles
/*
	m_ppAllDynamicEntities[m_iDynamicEntitiesNum] = new CEntityProjectile;
	m_ppAllDynamicEntities[m_iDynamicEntitiesNum]->m_iAbsoluteEntitiesIndex = -1;
	(m_ppAllDynamicEntities[m_iDynamicEntitiesNum])->m_pBrush->m_iAbsoluteEntityIndex = -1;
	(m_ppAllDynamicEntities[m_iDynamicEntitiesNum])->m_pPhysicsModel->SetPosition(CVector3f(694.f,180.f,-2107.f));
	((CEntityProjectile*)m_ppAllDynamicEntities[m_iDynamicEntitiesNum])->SetTryMove(CVector3f(0.f,-2.f,0.f));


	m_iDynamicEntitiesNum++;*/
	/************************************************/


}


// trying to reuse "dead" projectiles
bool CQuake3BSP::GetFreeProjectileEntity(CEntity*& pEntity,DWORD dwProjectileType)
{

	bool bRecycled = false;

	for (int i=m_iPlayersNum-1;i<m_iDynamicEntitiesNum;i++)
	{
		if (m_ppAllDynamicEntities[i]->m_dwDynamicEntityType == DYNAMIC_ENTITY_TYPE_PROJECTILE) // it's a projectile
		if (!m_ppAllDynamicEntities[i]->m_bActive) // but not active (so we can "revive" it)
		if (((CEntityProjectile*)m_ppAllDynamicEntities[i])->m_fExplosionCountdown<=0.f) // if explosion animation has ended
		if (((CEntityProjectile*)m_ppAllDynamicEntities[i])->m_fMarkCountdown<=0.f) // if mark life has ended
		{
			pEntity = m_ppAllDynamicEntities[i];

			if (pEntity->m_dwDynamicEntityType==DYNAMIC_ENTITY_TYPE_PROJECTILE)
			{

				if (dwProjectileType!=((CEntityProjectile*)pEntity)->m_dwProjectileType)
				{
					((CEntityProjectile*)pEntity)->m_dwProjectileType = dwProjectileType;
					((CEntityProjectile*)pEntity)->SetMD3s();
				}

				


				if (dwProjectileType==PROJECTILE_TYPE_ROCKET)
					((CEntityProjectile*)pEntity)->m_fExplosionCountdown = ROCKET_EXPLOSION_TIME;					
				else
				if (dwProjectileType==PROJECTILE_TYPE_PLASMA)
					((CEntityProjectile*)pEntity)->m_fExplosionCountdown = PLASMA_EXPLOSION_TIME;					
				else
				if (dwProjectileType==PROJECTILE_TYPE_BULLET)
				{
					((CEntityProjectile*)pEntity)->m_bActive=false;
					((CEntityProjectile*)pEntity)->m_fExplosionCountdown = BULLET_EXPLOSION_TIME;					
					// remember that i still need to set the normal
				}



				((CEntityProjectile*)pEntity)->m_fMarkCountdown = MARK_STAY_TIME;


				((CEntityProjectile*)pEntity)->m_pExplosion->DisableSpecialCounter();
			}



			

			return true;
		}
	}

	// if we got here it means there was no projectile entity to "recycle"

	for (int i=m_iPlayersNum-1;i<MAX_DYNAMIC_ENTITIES_NUM;i++)
	{
		if (!m_ppAllDynamicEntities[i])
		{
			m_ppAllDynamicEntities[i] = new CEntityProjectile;
			m_ppAllDynamicEntities[i]->m_iAbsoluteEntitiesIndex = -1;
			(m_ppAllDynamicEntities[i])->m_pBrush->m_iAbsoluteEntityIndex = -1;

			if (m_ppAllDynamicEntities[i]->m_dwDynamicEntityType==DYNAMIC_ENTITY_TYPE_PROJECTILE)
			{
				/*((CEntityProjectile*)m_ppAllDynamicEntities[i])->m_fExplosionCountdown = ROCKET_EXPLOSION_TIME;
				((CEntityProjectile*)m_ppAllDynamicEntities[i])->m_pExplosion->DisableSpecialCounter();*/


				if (dwProjectileType!=((CEntityProjectile*)m_ppAllDynamicEntities[i])->m_dwProjectileType)
				{
					((CEntityProjectile*)m_ppAllDynamicEntities[i])->m_dwProjectileType = dwProjectileType;
					((CEntityProjectile*)m_ppAllDynamicEntities[i])->SetMD3s();
				}

				


				if (dwProjectileType==PROJECTILE_TYPE_ROCKET)
					((CEntityProjectile*)m_ppAllDynamicEntities[i])->m_fExplosionCountdown = ROCKET_EXPLOSION_TIME;					
				else
				if (dwProjectileType==PROJECTILE_TYPE_PLASMA)
					((CEntityProjectile*)m_ppAllDynamicEntities[i])->m_fExplosionCountdown = PLASMA_EXPLOSION_TIME;					
				else
				if (dwProjectileType==PROJECTILE_TYPE_BULLET)
				{
					((CEntityProjectile*)m_ppAllDynamicEntities[i])->m_bActive=false;
					((CEntityProjectile*)m_ppAllDynamicEntities[i])->m_fExplosionCountdown = PLASMA_EXPLOSION_TIME;					
				}


				((CEntityProjectile*)m_ppAllDynamicEntities[i])->m_fMarkCountdown = MARK_STAY_TIME;


				((CEntityProjectile*)m_ppAllDynamicEntities[i])->m_pExplosion->DisableSpecialCounter();
			}


			

			// DEBUG DEBUG DEBUG
			//(m_ppAllDynamicEntities[i])->m_pPhysicsModel->SetPosition(CVector3f(694.f,180.f,-2107.f));

			if (i>m_iDynamicEntitiesNum-1)
			{
				m_iDynamicEntitiesNum++;
			}
			pEntity = m_ppAllDynamicEntities[i];
			return false;
		}

	}


	// if we got here then we ran out of entities
	// ToDO:need to handle this situation
	return true;

}

void CQuake3BSP::UpdateLinkDynamicEntities(void)
{

	/*((CEntityPlayer*) m_ppAllDynamicEntities[0])->FakeAI(m_ppAllDynamicEntities[0]->m_vPos);
	((CEntityPlayer*) m_ppAllDynamicEntities[0])->FakeAI(m_ppAllDynamicEntities[1]->m_vPos);*/

	for (int i=0;i<m_iDynamicEntitiesNum;i++)
	{
		// if the dynamic entities are players
		if (i<m_iPlayersNum)
		{	
			

			if (g_pCheckersBoard->m_iPlayerTypePAST==PLAYER_TYPE_PC && g_pCheckersBoard->m_iPlayerTypeFUTURE==PLAYER_TYPE_PC)
			{
				if (i==0)
					((CEntityPlayer*) m_ppAllDynamicEntities[0])->FakeAI(m_ppAllDynamicEntities[1]->m_vPos);
				else
					((CEntityPlayer*) m_ppAllDynamicEntities[1])->FakeAI(m_ppAllDynamicEntities[0]->m_vPos);	
			}
			else
			// RESTORE RESTORE RESTORE RESTORE 
			// if we're in the range of players (the first N dynamic entities are the arena players)
			// AND if it's not the currently controled player, then we must use AI for him
			if ((g_pCurrentlyProcessedPlayerClient != m_ppAllDynamicEntities[i]->m_pPlayerClient))
			{			
				if (i==0)
				{
					if (g_Communication.GetAlreadyInited())
						((CEntityPlayer*) m_ppAllDynamicEntities[0])->GetInputFromNetwork();
					else
						((CEntityPlayer*) m_ppAllDynamicEntities[0])->FakeAI(m_ppAllDynamicEntities[1]->m_vPos);
				}
				else
				{
					if (g_Communication.GetAlreadyInited())
						((CEntityPlayer*) m_ppAllDynamicEntities[1])->GetInputFromNetwork();
					else
						((CEntityPlayer*) m_ppAllDynamicEntities[1])->FakeAI(m_ppAllDynamicEntities[0]->m_vPos);	
				}
			}
			else
			{
				/*if (g_Communication.GetAlreadyInited())
					g_Communication.SendFPSBattleMessage(m_ppAllDynamicEntities[i]->m_pPlayerClient->GetPhysicsModel()->GetPosition());*/

			}

			
		}
		


		/*if ((i<m_iPlayersNum))
		{
			if ((! (g_Communication.GetAlreadyInited() &&  i==1))) 				
				m_ppAllDynamicEntities[i]->UpdatePhysics(); 
			else
			{				
				if (!(((CEntityPlayer*) m_ppAllDynamicEntities[1])->m_pPlayerClient->m_bDying))
				{
					((CEntityPlayer*) m_ppAllDynamicEntities[i])->UpdateModel();
					((CEntityPlayer*) m_ppAllDynamicEntities[i])->UpdateLinking();
				}
				else
					((CEntityPlayer*) m_ppAllDynamicEntities[i])->m_pPlayerClient->UpdateCharacterAnimation();
				
			}
		}
		else
			m_ppAllDynamicEntities[i]->UpdatePhysics(); 
			*/
		if ((i<m_iPlayersNum))
		{
			if ((((CEntityPlayer*) m_ppAllDynamicEntities[0])->m_pPlayerClient->m_bDying))
			{
				
				((CEntityPlayer*) m_ppAllDynamicEntities[i])->UpdateModel();

				if (!m_bFightOver)
					((CEntityPlayer*) m_ppAllDynamicEntities[i])->UpdateLinking();
			}
			else
				m_ppAllDynamicEntities[i]->UpdatePhysics();
		}
		else
			m_ppAllDynamicEntities[i]->UpdatePhysics(); 


		// networking

		// for now - this doesn't allow you to 'z'
		if (g_Communication.GetAlreadyInited())
		//if	(st_TimerI->GetTimeSinceLastGameLogicHappened() >= GAME_LOGIC_MILISECONDS_INTERVAL)
		if	(st_TimerI->GetTimeSinceLastGameLogicHappened_UDP_SEND() >= GAME_LOGIC_MILISECONDS_INTERVAL_UDP_SEND)
		if (i==0 && !m_bFightOver) 
		{
			/*if ( (!(m_ppAllDynamicEntities[i]->m_pPlayerClient->vLastSentPosition==m_ppAllDynamicEntities[i]->m_pPlayerClient->GetPhysicsModel()->GetPosition())) ||
				(m_ppAllDynamicEntities[i]->m_pPlayerClient->fLastSentAngX!=m_ppAllDynamicEntities[i]->m_pPlayerClient->GetPhysicsModel()->m_fAngX) ||
				(m_ppAllDynamicEntities[i]->m_pPlayerClient->fLastSentAngY!=m_ppAllDynamicEntities[i]->m_pPlayerClient->GetPhysicsModel()->m_fAngY))
			{*/

			DWORD dwPlayerActions = 0;

			if (m_ppAllDynamicEntities[i]->m_pPlayerClient->bShot)
				dwPlayerActions|=FPS_BATTLE_MESSAGE_SHOOT;


				g_Communication.SendFPSBattleMessage(m_ppAllDynamicEntities[i]->m_pPlayerClient->GetPhysicsModel()->GetPosition(),
					m_ppAllDynamicEntities[i]->m_pPlayerClient->GetPhysicsModel()->GetVelocity(),
					m_ppAllDynamicEntities[i]->m_pPlayerClient->GetPhysicsModel()->GetAcceleration(),
					m_ppAllDynamicEntities[i]->m_pPlayerClient->GetPhysicsModel()->GetLastDesiredPosition(),
				m_ppAllDynamicEntities[i]->m_pPlayerClient->GetPhysicsModel()->m_fAngX,
				m_ppAllDynamicEntities[i]->m_pPlayerClient->GetPhysicsModel()->m_fAngY,
				m_ppAllDynamicEntities[i]->m_pPlayerClient->GetPhysicsModel()->m_dwPlayerState,
				0);

				/*m_ppAllDynamicEntities[i]->m_pPlayerClient->vLastSentPosition=m_ppAllDynamicEntities[i]->m_pPlayerClient->GetPhysicsModel()->GetPosition();
				m_ppAllDynamicEntities[i]->m_pPlayerClient->fLastSentAngX=m_ppAllDynamicEntities[i]->m_pPlayerClient->GetPhysicsModel()->m_fAngX;
				m_ppAllDynamicEntities[i]->m_pPlayerClient->fLastSentAngY=m_ppAllDynamicEntities[i]->m_pPlayerClient->GetPhysicsModel()->m_fAngY;
			}*/
		}

		if (st_Engine::Inst()->m_iGameState==0) //if we moved to board game
			break;
		m_ppAllDynamicEntities[i]->LinkToLeafs();
	}

	/*if (st_Engine::Inst()->m_iGameState==0)
	{
		((CEntityPlayer*)m_ppAllDynamicEntities[0])->m_pPlayerClient->InitStates();
		((CEntityPlayer*)m_ppAllDynamicEntities[0])->m_pPlayerClient->m_pPlayerModel->InitAnimationStates();

		((CEntityPlayer*)m_ppAllDynamicEntities[1])->m_pPlayerClient->InitStates();
		((CEntityPlayer*)m_ppAllDynamicEntities[1])->m_pPlayerClient->m_pPlayerModel->InitAnimationStates();

	}*/

	/*
	m_pPlayerModel->InitAnimationStates();*/

}

void CQuake3BSP::BruteDrawDynamicEntities()
{
	/*for (int i=0;i<m_iDynamicEntitiesNum;i++)
	{
		m_ppAllDynamicEntities[i]->Draw();
		m_ppAllDynamicEntities[i]->DrawBoundingBox();
	}*/

}

void CQuake3BSP::KillSelf(void)
{
	g_pPlayerClient1->m_iLife=-1000;
}

void  CQuake3BSP::KillOpponent(void)
{
	g_pPlayerClient2->m_iLife=-1000;
}