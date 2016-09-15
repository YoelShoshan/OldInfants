#ifndef YOEL_MD3MESH_MANAGER_H
#define YOEL_MD3MESH_MANAGER_H

#pragma once

#include <Windows.h>
#include "SingleTon.h"
#include "MD3.h"

///////////////////////////////////////////
//
// Model Manager
//
///////////////////////////////////////////

#define st_MD3MeshBaseManager CSingleton<CMD3MeshBaseManager>
#define st_MD3MeshBaseManagerI CSingleton<CMD3MeshBaseManager>::Inst()


#define MD3MESH_MAN_MAX_MODELS 256

class CMD3MeshBaseManager
{
public:

	CMD3MeshBaseManager();
	~CMD3MeshBaseManager();
	
/*	CMD3MeshBase* PushModel(CMD3MeshBase* pModel,const char *FileName,int iIndex);*/

	int AlreadyInManager(const char* CMD3MeshBaseName);

	CMD3MeshBase* GetModel(int iAbsoluteIndex);

	CMD3MeshBase* GetModel(const char* ModelName,int iIndex);

	int GetNumOfSubMeshes(const char* ModelName);

	
	// finds the first free tex, allocates it, and returns a pointer to the new allocated CTexture
	CMD3MeshBase* AllocFirstFreeModel(void);
	

	bool Unregister(CMD3MeshBase* pModel);

	int GetModelsNum(void);


private:

	UINT        m_uiLastModelNum;
	CSmartPointer<CMD3MeshBase>* m_spModelsQueue;
};




#endif //YOEL_MD3MESH_MANAGER_H