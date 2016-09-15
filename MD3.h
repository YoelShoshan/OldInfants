#ifndef YOEL_STATIC_MD3_H
#define YOEL_STATIC_MD3_H

#pragma once
#include <windows.h>

#include <fstream>
#include "Math3d.h"
#include "VertexBuffer.h"
#include "ReferenceCount.h"
#include "SmartPointer.h"
#include <vector>

// the MD3 Header
struct tMd3Header
{ 
	char	fileID[4];					// This stores the file ID - Must be "IDP3"
	int		version;					// This stores the file version - Must be 15
	char	strFile[68];				// This stores the name of the file
	int		numFrames;					// This stores the number of animation frames
	int		numTags;					// This stores the tag count
	int		numMeshes;					// This stores the number of sub-objects in the mesh
	int		numMaxSkins;				// This stores the number of skins for the mesh
	int		headerSize;					// This stores the mesh header size
	int		tagStart;					// This stores the offset into the file for tags
	int		tagEnd;						// This stores the end offset into the file for tags
	int		fileSize;					// This stores the file size
};


// the bone information
struct tMd3Bone
{
	float	mins[3];					// This is the min (x, y, z) value for the bone
	float	maxs[3];					// This is the max (x, y, z) value for the bone
	float	position[3];				// This supposedly stores the bone position???
	float	scale;						// This stores the scale of the bone
	char	creator[16];				// The modeler used to create the model (I.E. "3DS Max")
};


// The tag structure for the .MD3 file format.  These are used to link other
// models to and to rotate and transate the child models of that model.
struct tMd3Tag
{
	char		strName[64];			// This stores the name of the tag (I.E. "tag_torso")
	CVector3f	vPosition;				// This stores the translation that should be performed
	//float		rotation[3][3];			// This stores the 3x3 rotation matrix for this frame
	float		rotation[9];			// This stores the 3x3 rotation matrix for this frame
};


// This structure is used to read in the mesh data for the .md3 models
struct tMd3MeshInfo
{
	char	meshID[4];					// This stores the mesh ID (We don't care)
	char	strName[68];				// This stores the mesh name (We do care)
	int		numMeshFrames;				// This stores the mesh aniamtion frame count
	int		numSkins;					// This stores the mesh skin count
	int     numVertices;				// This stores the mesh vertex count
	int		numTriangles;				// This stores the mesh face count
	int		triStart;					// This stores the starting offset for the triangles
	int		headerSize;					// This stores the header size for the mesh
	int     uvStart;					// This stores the starting offset for the UV coordinates
	int		vertexStart;				// This stores the starting offset for the vertex indices
	int		meshSize;					// This stores the total mesh size
};


// This stores the normals and vertex indices 
struct tMd3Vert
{
   signed short	 position[3];			// The vertex for this face (scale down by 64.0f)
   //unsigned char normal[2];				// This stores that ANGLES of the rotated (normalized) vectors...
   short normal;
};


// This stores the indices into the vertex and texture coordinate arrays
struct tMd3Face
{
   int vertexIndices[3];				
};


// This stores UV coordinates
struct tMd3TexCoord
{
   float textureCoord[2];
};


// This stores a skin name (We don't use this, just the name of the model to get the texture)
struct tMd3Skin 
{
	char strName[68];
};


class CMD3MeshBase : public CReferenceCount
{
public:

	CMD3MeshBase();
	~CMD3MeshBase();

	int                      m_iSkinsNum;
	tMd3Skin				*m_pMD3Skins;			// The skin name data (not used)
	int                      m_iTexCoordNum;
	tMd3TexCoord			*m_pMD3TexCoords;		// The texture coordinates
	int                      m_iFacesNum;
	tMd3Face				*m_pMD3Faces;		// Face/Triangle data
	int                      m_iVerticesNum;
	tMd3Vert		    	*m_pMD3Verts;		// Vertex/UV indices

	int                     m_iMeshFramesNum;

	


	CVector3f         *m_pOriginalVerts; // data
	CVector3f         *m_pNormals;
	CVector2f         *m_pTexCoords;

	CVector3f         *m_pTangents;
	CVector3f         *m_pBinormals;

	unsigned int      *m_uiIndices;


	char* m_pcPlayerSkin; 
	//char* m_pcPlayerSkinNormalMap; 

	char* GetName(void) { return m_pcMD3Name;};
	int   GetMeshNum(void) {return m_iMD3MeshNum;};

	char* m_pcMD3Name;
	int   m_iMD3MeshNum;

};





class CMD3Mesh
{
public:

	CMD3Mesh();
	~CMD3Mesh();

	void        ComputeInterpolatedVerts(float fT,bool bChangingToAnotherAnimation);

	CColor4*           m_pColors;
	
	/*CMD3MeshBase* m_pSharedMesh;*/
	CSmartPointer<CMD3MeshBase> m_pSharedMesh;

	CVector3f         *m_pCurrentInterpolatedVerts; // data
	CVector3f         *m_pBackupVerts; // data


	CVector3f         *m_pCurrentFrameVerts; // pointer
	CVector3f         *m_pNextFrameVerts; // pointer

			    
	CVertexBuffer m_VB;    // the vertexbuffers array
	

};



// an animation sequence 
class CAnimationSequence
{
public:
	CAnimationSequence() { strName[0]=NULL;startFrame=0;endFrame=1;loopingFrames=0;framesPerSecond=1;iAnimationID=-1;bShouldFreezeAtEnd=false;};
	~CAnimationSequence() {};

	char strName[255];			// This stores the name of the animation (I.E. "TORSO_STAND")
	int startFrame;				// This stores the first frame number for this animation
	int endFrame;				// This stores the last frame number for this animation
	int loopingFrames;			// This stores the looping frames for this animation (not used)
	int framesPerSecond;		// This stores the frames per second that this animation runs

	bool bIsRepetetive;
	bool bShouldFreezeAtEnd;

	int iAnimationID;
};


#define MAX_MD3_MODEL_ANIMATION_SEQUENCES 30

class CMD3;

class CMD3Shared : public CReferenceCount
{
public:

	CMD3Shared();
	~CMD3Shared();

	int m_iNumOfAnimations;				// The number of animations in this model 
	CAnimationSequence animations[MAX_MD3_MODEL_ANIMATION_SEQUENCES]; // The list of animations

	char* GetName(void) { return m_pcMD3Name;};

	char* m_pcMD3Name;

	int          m_iMeshesNum;

	// links - ("bones" structure)

	int m_iNumOfTags; // number of tags in the model
	tMd3Tag     *m_pTags;

	
	
	char* m_cpFileName;

	tMd3Bone	*m_pBones;			// This stores the bone data

};

class CMD3
{
public:

	CMD3();
	~CMD3();

	bool Load(const char* cpFileName,const char* skin, const char* texture,bool bPPL); // if no skin, then input skin==NULL

	bool ExistInManager(const char* cpFileName);

	bool LoadSkin(const char* cpFileName,bool bPPL);

	void ConvertToVertexBuffers(int iInManager,bool bPPL);
	
	void PushToRenderer(bool bPPLused);

	void PushToRenderer(char *cpShaderName);

/*	void PushToRendererForceShader(char *cpShaderName);*/

	void SetSpecialCounter(void);

	void SetSubMeshesColorArrays(byte r, byte g, byte b, byte a);

	void DisableSpecialCounter(void);

	void SetMeshesCurrentFramePointers(int iFrameNum);
	void SetMeshesNextFramePointers(int iFrameNum);

	void ComputeMeshesInterpolatedVertices(float fT);

	void SetAlwaysDraw(bool bSet);
		
	void ForceAnimByGeneralAnimationName(int iNum);

	void ForceAnimByLocalArrayIndex(int iNum);

	void SetIgnoreCameraMatrix(bool bSet);

	CMD3Mesh* GetMeshes(void){ return m_pMeshes;};

	void Update(bool bSpecialWorld,bool bIgnoreCameraMatrix,float* pfWorldMatrix);

	void UpdateAnimation(void);

	int GetAnimationByID(int iNum);

	bool GetAnimationEndedNow(void) { return m_bAnimationEndedInThisFrame;};


	// data

	CSmartPointer<CMD3Shared> m_pMD3Shared;
	//CMD3Shared* m_pMD3Shared;

	CMD3 ** m_pLinks; // an array of pointers to pointers to MD3 objects

	
	CMD3Mesh    *m_pMeshes;
	

	int m_iCurrentAnim;					// The current index into pAnimations list 
	int m_iCurrentFrame;					// The current frame of the current animation 
	int m_iNextFrame;						// The next frame of animation to interpolate too
	float m_fT;							// The ratio of 0.0f to 1.0f between each key frame
	float m_fLastTime;						// This stores the last time that was stored
	
	

	int                m_iCurrentDrawnFrame;

	// when moving from one animation to another, there should be a smooth move from the current vertices state,
	// to the first frame of the animation we're switching TOO
	bool  m_bChangingAni;
	float m_fChangeAniT;


	
	bool m_bAnimationEndedInThisFrame;
	
	char * m_cpFileName;

private:
	

	// The file pointer
	FILE *m_pFile;
	

	
	

	


};











#endif //YOEL_STATIC_MD3_H