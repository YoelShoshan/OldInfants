
#include "Sky.h"

///////////////////////////
#include "ZedTracker.h"
///////////////////////////

CSky::CSky()
{
	m_pSkyBoxVerts = NULL;
	m_pSkyBoxSPHERETexCoords = NULL;
	m_pSkyBoxCUBETexCoords = NULL;
		m_ipSkyBoxIndices = NULL;  

	m_strSkyBoxShaderName=NULL;

	

	m_bDrawSide[0] = true; // right plane
	m_bDrawSide[1] = true; // up plane
	m_bDrawSide[2] = true; // back plane
	m_bDrawSide[3] = true; // left plane
	m_bDrawSide[4] = false; // buttom plane
	m_bDrawSide[5] = true; //front plane


	m_pSkySidesNormals[0].Set(-1,0,0);	//right
	m_pSkySidesNormals[1].Set(0,-1,0);	//top
	m_pSkySidesNormals[2].Set(0,0,-1);	// back
	m_pSkySidesNormals[3].Set(1,0,0);	//left
	m_pSkySidesNormals[4].Set(0,1,0);	//bottom
	m_pSkySidesNormals[5].Set(0,0,1);	//front

}

CSky::~CSky()
{
	SafeDeleteArray(m_pSkyBoxVerts);
	SafeDeleteArray(m_pSkyBoxSPHERETexCoords);
	SafeDeleteArray(m_pSkyBoxCUBETexCoords);
	SafeDeleteArray(m_ipSkyBoxIndices);
	SafeDeleteArray(m_strSkyBoxShaderName);
}



void CSky::GenerateSphereTexCoords(float height,CVector3f *vertices, int num_vertices, CVector2f *texcoords)
{
	float temp=height*height-SPHERE_RADIUS*SPHERE_RADIUS;
	for (int v=0;v<num_vertices;++v)
	{
		CVector3f dir;

		
		dir.v[0] = vertices[v].v[0];
		dir.v[1] = vertices[v].v[1];
		dir.v[2] = vertices[v].v[2];

		
		float  a = Dot(dir,dir);
		float b=height*dir.v[1]/a;
		float c=temp/a;
		float t=sqrtf(b*b-c)-b;

		assert(t>0.0f);

		float h=(dir.v[1]*t+height)/SPHERE_RADIUS;
		h=fabsf(h);
		if (h>1.0f) h=1.0f;	//unfortunately this happens rather often
		float lat=(0.5f*PI-asinf(h))/(0.5f*PI);

		t=dir.v[0]*dir.v[0]+dir.v[2]*dir.v[2];
		if (t>0.000001f)
		{
			t=1.0f/sqrtf(t);
		}
		texcoords[v].v[0]=0.5f+dir.v[0]*lat*t*2.0f; //  is repeating the sky 4 times in both direction
		texcoords[v].v[1]=1.0f-(0.5f+dir.v[2]*lat*t*2.0f);

	}
}

void CSky::GenerateCubeMapTexCoords(int tess,int texturesize,CVector2f *texcoords)
{
	float texstep=1.0f/(float)(tess-1);
	for (int y=0;y<tess;++y)
	{
		for (int x=0;x<tess;++x)
		{
			*(texcoords++)=CVector2f((float)x*texstep,(float)y*texstep);
		}
	}
}

/*void CSky::DetermineMinMax(CVector3f *verts, int num_verts)
{
	// which component of the vector make up the s and t axis, the third component is used for normalizing
	// -value is encoded as ~value
	static const int staxis[6][3]={{2,1,0},{~2,0,1},{~0,1,2},{~2,1,0},{~2,~0,1},{0,1,2}}; 

	CVector3f absfacedir;
	CVector3f facedir=*verts;
	int side,skyside;

	for (int v=1;v<num_verts;++v) // only allow triangles
	{
		facedir+=verts[v];
	}

	absfacedir.v[0]=fabsf(facedir.v[0]);
	absfacedir.v[1]=fabsf(facedir.v[1]);
	absfacedir.v[2]=fabsf(facedir.v[2]);
	if ((absfacedir.v[1]>absfacedir.v[0])&&(absfacedir.v[1]>absfacedir.v[2]))
	{
		side=SKY_TOP;
	}
	else if((absfacedir.v[0]>absfacedir.v[1])&&(absfacedir.v[0]>absfacedir.v[2]))
	{
		side=SKY_RIGHT;
	}
	else side=SKY_BACK;

	skyside= facedir[side]>=0.0f ? side : side+3;
	usedskysides[skyside]=true;


	for (v=0;v<num_verts;++v)
	{
		CVector3f corner=verts[v];

		float scale=fabsf(corner[staxis[skyside][2]]);
		if (scale>0.0f) scale=1.0f/scale;

		side=staxis[skyside][0];
		float s=side>=0.0f?corner[side]:-corner[~side];
		s*=scale;
		side=staxis[skyside][1];
		float t=side>=0.0f?corner[side]:-corner[~side];
		t*=scale;

		if (s<skyminmax[skyside][SKY_MINS]) skyminmax[skyside][SKY_MINS]=s;
		if (s>skyminmax[skyside][SKY_MAXS]) skyminmax[skyside][SKY_MAXS]=s;
		if (t<skyminmax[skyside][SKY_MINT]) skyminmax[skyside][SKY_MINT]=t;
		if (t>skyminmax[skyside][SKY_MAXT]) skyminmax[skyside][SKY_MAXT]=t;
	}

}*/

void  CSky::GenerateSkyIndices(UINT* indices)
{
	
	int xy = 6*(SKYBOX_TESS-1)*(SKYBOX_TESS-1);
	
	CreateGridIndices(m_ipSkyBoxIndices,SKYBOX_TESS,SKYBOX_TESS,0,0);
	CreateGridIndices(m_ipSkyBoxIndices,SKYBOX_TESS,SKYBOX_TESS,xy, m_iVertsPerSide);
	CreateGridIndices(m_ipSkyBoxIndices,SKYBOX_TESS,SKYBOX_TESS,2*xy,2*m_iVertsPerSide);
	CreateGridIndices(m_ipSkyBoxIndices,SKYBOX_TESS,SKYBOX_TESS,3*xy,3*m_iVertsPerSide);
	CreateGridIndices(m_ipSkyBoxIndices,SKYBOX_TESS,SKYBOX_TESS,4*xy,4*m_iVertsPerSide);
	CreateGridIndices(m_ipSkyBoxIndices,SKYBOX_TESS,SKYBOX_TESS,5*xy,5*m_iVertsPerSide);

}

void CSky::CreateGridIndices(UINT* IndexArray,int xdim,int ydim,int startindex,int startvert)
{
	int indices_per_row=6*(xdim-1);

	UINT index=0;
	UINT vertex=xdim;	

	for (int y=1;y<ydim;y++)
	{
		for (int x=0;x<xdim-1;x++)
		{
			IndexArray[startindex+index++]=vertex + startvert;
			IndexArray[startindex+index++]=vertex+1 + startvert;
			IndexArray[startindex+index++]=vertex-xdim + startvert;

			IndexArray[startindex+index++]=vertex-xdim + startvert;
			IndexArray[startindex+index++]=vertex+1 + startvert;
			IndexArray[startindex+index++]=vertex+1-xdim + startvert;

			vertex++;
		}
		vertex++;
	}
}

void CSky::GenerateSkyBoxVerts(CVector3f forward,CVector3f up,CVector3f right, int tess, float skyboxsize, CVector3f *vertices)
{
	float dist= skyboxsize/(float)(tess-1);
	CVector3f origin=(forward-up-right)*(skyboxsize*0.5f);
	up*=dist;
	right*=dist;

	// we start in the lower left corner of the skyside
	for (int y=0;y<tess;++y)
	{
		CVector3f vert=origin;
		for (int x=0;x<tess;++x)
		{
			*(vertices++)=(CVector3f)vert;
			vert+=right;
		}
		origin+=up;
	}
}

void CSky::GenerateSkyBox(void)
{
	static const CVector3f baseaxis[18]=
	{
		CVector3f(1,0,0), CVector3f(0,0,1),CVector3f(0,1,0),			//right
			CVector3f(0,1,0), CVector3f(0,0,-1),CVector3f(1,0,0), 		//top
			CVector3f(0,0,1), CVector3f(-1,0,0),CVector3f(0,1,0), 		// back
			CVector3f(-1,0,0), CVector3f(0,0,-1),CVector3f(0,1,0), 		//left
			CVector3f(0,-1,0),  CVector3f(0,0,-1),CVector3f(-1,0,0),		//bottom
			CVector3f(0,0,-1), CVector3f(1,0,0)	,CVector3f(0,1,0) 	//front
	};


	SafeDeleteArray(m_pSkyBoxVerts);
	SafeDeleteArray(m_pSkyBoxSPHERETexCoords);
	SafeDeleteArray(m_pSkyBoxCUBETexCoords);
	SafeDeleteArray(m_ipSkyBoxIndices);


	m_iVertsPerSide=SKYBOX_TESS*SKYBOX_TESS;
	m_iIndicesPerSide=6*(SKYBOX_TESS-1)*(SKYBOX_TESS-1);


	m_pSkyBoxVerts = new CVector3f[6*m_iVertsPerSide];
	m_pSkyBoxSPHERETexCoords = new CVector2f[6*m_iVertsPerSide];
	m_pSkyBoxCUBETexCoords = new CVector2f[6*m_iVertsPerSide];
	m_ipSkyBoxIndices = new UINT[6*m_iIndicesPerSide];  

	for (int s=0;s<6;++s)
	{
		GenerateSkyBoxVerts(baseaxis[s*3],baseaxis[s*3+2],baseaxis[s*3+1],SKYBOX_TESS,SKYBOX_SIZE,m_pSkyBoxVerts+s*m_iVertsPerSide);		
		GenerateCubeMapTexCoords(SKYBOX_TESS,256,m_pSkyBoxCUBETexCoords+s*m_iVertsPerSide);		
	}

	GenerateSphereTexCoords(5,m_pSkyBoxVerts,6*m_iVertsPerSide,m_pSkyBoxSPHERETexCoords);

	GenerateSkyIndices(m_ipSkyBoxIndices);

}

void CSky::SetSideTexture(int iSideNum, CTexture* pTex)
{
	//m_puiTexturesIndex[iSideNum] = newIndex;
	m_spTexturesIndex[iSideNum] = pTex;
}

void CSky::FreeSideTextures(void)
{
	for (int i=0;i<6;i++)
		m_spTexturesIndex[i]=NULL;

	SafeDeleteArray(m_strSkyBoxShaderName);
	m_strSkyBoxShaderName = NULL;
}
