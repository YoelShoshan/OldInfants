#ifndef YOEL_TEXTURE_H
#define YOEL_TEXTURE_H

#pragma once

#include "ReferenceCount.h"
#include "SmartPointer.h"
//#include "Defines.h"
#include "jpeglib.h"

///////////////////////////
//#include "ZedTracker.h"
///////////////////////////

// Largest supported anistropic texture ratio extension address
#ifndef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
	#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#endif

	// Anistropic texturing extension address
#ifndef GL_TEXTURE_MAX_ANISOTROPY_EXT
	#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#endif


#define TGA_RGB		 2	
#define TGA_A		 3	
#define TGA_RLE		10	

void FindTextureExtension(char *strFileName);


struct tImage
{
	int channels;			// how many channels? (3 is just rgb, 4 has also alpha - rgba)
	int sizeX;			
	int sizeY;			
	unsigned char *data;	// the actual pixels data
};

class CTexture: public CReferenceCount
{ 
public: 
     CTexture(); 
     ~CTexture(); 
 
     //bool LoadRaw(byte* pImageBits,int iWidth, int iHeight); 
     //void CreateNormalizationCubeMap(int iSize);  --> ask dTag about this

	 void ChangeGamma(byte *pImage, int size, float factor);
     
	 void LoadLightmapTexture(byte *pImageBits, int width, int height);
	 bool LoadTexture(const char* strFileName,bool bMipMap,bool bClamp, bool bCompress);

	 //bool LoadSkyBoxTextures(const char* strFileName);

     void Bind(); 

     inline char* GetName() const { return m_TexName; }; 

     inline int GetWidth() const { return m_iWidth; }; 
     inline int GetHeight() const  { return m_iHeight; }; 
 
	 inline void SetID(UINT uiID) { m_uiTextureID = uiID;};
     inline UINT GetID() const { return m_uiTextureID; }; 

	 inline bool GetIsLoaded() const { return m_bIsLoaded;};
	 inline void SetIsLoaded(bool bIsLoaded) { m_bIsLoaded = bIsLoaded;}; 
 
protected: 
     //void GetCubeVector(int i, int iCubeSize, int x, int y, CVector3& v); 

 
     tImage* LoadBMP(const char *pchFileName); 
     tImage* LoadTGA(const char *pchFileName); 
     tImage* LoadJPG(const char *pchFileName); 
     void DecodeJPG(jpeg_decompress_struct* cinfo, tImage *pImageData); 

	 bool m_bIsLoaded;
 
     UINT m_uiTextureID; 
     //LPSTR sName; 
	 char* m_TexName;

 
     int m_iWidth; 
     int m_iHeight; 
}; 

///////////////////////////////////////////
//
// Texture Manager
//
///////////////////////////////////////////

#define st_TextureManager CSingleton<CTextureManager>


#define TEX_MAN_MAX_TEXTURES 2048

class CTextureManager
{
public:

	CTextureManager();
	~CTextureManager();
	
	CTexture* LoadTexAndPush(const char *FileName,bool bMipMap,bool bClamp,bool bCompress);

	CTexture* LoadLightMapAndPush(byte *pImageBits, int width, int height);

	//CSmartPointer<CTexture> LoadSkyBoxTexturesAndPush(const char* TextureName);
	
	int AlreadyInManager(const char* TextureName);

	// finds the first free tex, allocates it, and returns a pointer to the new allocated CTexture
	CTexture* AllocFirstFreeTex(void);

	bool CTextureManager::Unregister(CTexture* pTex);

	int GetTexturesNum(void);


private:

	UINT        m_uiLastTexNum;
	//CTexture**   m_ppTexturesQueue;
	CSmartPointer<CTexture>* m_spTexturesQueue;
};


#endif