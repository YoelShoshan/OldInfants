#ifndef YOEL_MD3_MANAGER_H
#define YOEL_MD3_MANAGER_H

#pragma once

#include <Windows.h>
#include "SingleTon.h"
#include "MD3.h"

///////////////////////////////////////////
//
// Model Manager
//
///////////////////////////////////////////

#define st_MD3BaseManager CSingleton<CMD3SharedManager>
#define st_MD3BaseManagerI CSingleton<CMD3SharedManager>::Inst()


#define MD3_MAN_MAX_MODELS 256

class CMD3SharedManager
{
public:

	CMD3SharedManager();
	~CMD3SharedManager();
	
/*	CMD3MeshBase* PushModel(CMD3MeshBase* pModel,const char *FileName,int iIndex);*/

	int AlreadyInManager(const char* CMD3SharedName);

	CMD3Shared* GetModel(int iAbsoluteIndex);

	CMD3Shared* GetModel(const char* ModelName);

	/*int GetNumOfSubMeshes(const char* ModelName);*/

	
	// finds the first free tex, allocates it, and returns a pointer to the new allocated CTexture
	CMD3Shared* AllocFirstFreeModel(void);
	

	bool Unregister(CMD3Shared* pModel);

	int GetModelsNum(void);


private:

	UINT        m_uiLastModelNum;
	CSmartPointer<CMD3Shared>* m_spModelsQueue;
};




#endif //YOEL_MD3_MANAGER_H