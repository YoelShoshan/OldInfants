#ifndef YOEL_SKY_H
#define YOEL_SKY_H

#pragma once

#include <windows.h>

#include "Math3D.h"
#include "Texture.h"



#define SPHERE_RADIUS 6.0f

#define SKYBOX_TESS	18       // (maybe I should make this depand on the current pc speed? )
#define SKYBOX_SIZE 2048.0f

#define MAX_NUM_SKYVERTS	 600
#define MAX_NUM_CLIPSKYVERTS 32

class CSky
{
public:

	CSky();
	~CSky();

void GenerateSphereTexCoords(float height,CVector3f *vertices, int num_vertices, CVector2f *texcoords);
void GenerateCubeMapTexCoords(int tess,int texturesize,CVector2f *texcoords);

void FlipCubeMapTexCoords(int tess,int texturesize,CVector2f *texcoords,bool bFlipX,bool bFlipY);

void GenerateSkyBoxVerts(CVector3f forward,CVector3f up,CVector3f right, int tess, float skyboxsize, CVector3f *vertices);

void  GenerateSkyIndices(UINT* indices);

void CreateGridIndices(UINT* IndexArray,int xdim,int ydim,int startindex,int startvert);

void SetSideTexture(int iSideNum, CTexture* pTex);

void FreeSideTextures(void);

void GenerateSkyBox(void);


	enum 
	{
		SKY_MINS = 0,
		SKY_MAXS = 1,
		SKY_MINT = 2,
		SKY_MAXT  = 3
	};

	enum
	{
		SKY_RIGHT	=	0,	//positive x
		SKY_TOP		=	1,	//positive y
		SKY_BACK	=	2,	//positive z

		SKY_LEFT	=	3,  //negative x
		SKY_BOTTOM	=	4,  //negative y
		SKY_FRONT	=	5   //negative z
	};

CVector3f* m_pSkyBoxVerts;
CVector2f* m_pSkyBoxSPHERETexCoords;

CVector2f* m_pSkyBoxCUBETexCoords;

UINT*       m_ipSkyBoxIndices;  

int m_iVertsPerSide;
int m_iIndicesPerSide;

char* m_strSkyBoxShaderName;

bool m_bDrawSide[6];
CVector3f m_pSkySidesNormals[6];

int m_CubeSkyBoxTextureIndex;

//UINT m_puiTexturesIndex[6];
CSmartPointer<CTexture> m_spTexturesIndex[6];


};



#endif YOEL_SKY_H