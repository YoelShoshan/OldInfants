#include "MD3Manager.h"


CMD3SharedManager::CMD3SharedManager()
{
	m_spModelsQueue = new CSmartPointer<CMD3Shared> [MD3_MAN_MAX_MODELS];
	
//	assert(_CrtCheckMemory());
	
	m_uiLastModelNum=0;
}

CMD3SharedManager::~CMD3SharedManager()
{
	SafeDeleteArray(m_spModelsQueue);
}


int CMD3SharedManager::GetModelsNum(void)
{
	int iModelsNum=0;
	for (int i=0;i<m_uiLastModelNum+1;i++)
	{
		if (((CMD3Shared*)m_spModelsQueue[i])!=NULL)
			iModelsNum++;
	}

	return iModelsNum;

}



int CMD3SharedManager::AlreadyInManager(const char* ModelName)
{
	char* t_ModelName;
	for (int i=0; i<m_uiLastModelNum+1;i++)
	{
		if (m_spModelsQueue[i])
		{	
		t_ModelName = m_spModelsQueue[i]->GetName();
		
		if (t_ModelName!=NULL)
			if (strcmp(ModelName,t_ModelName)==0) // we don't care about the index, because it's enough that we found one sub-mesh of the object, it already means it was loaded
			{
				return i;
			}
		}
	}

	return -1; // not in manager
}

/*

int CMD3MeshBaseManager::GetNumOfSubMeshes(const char* ModelName)
{
	int iNumOfSubMeshes = 0;

	char* t_ModelName;
	for (int i=0; i<m_uiLastModelNum+1;i++)
	{
		if (m_spModelsQueue[i])
		{	
		t_ModelName = m_spModelsQueue[i]->GetName();
		
		if (t_ModelName!=NULL)
			if (strcmp(ModelName,t_ModelName)==0) // we don't care about the index, because it's enough that we found one sub-mesh of the object, it already means it was loaded
			{
				iNumOfSubMeshes++;
			}
		}
		// else break;
	}

	return iNumOfSubMeshes; // not in manager

}
*/

bool CMD3SharedManager::Unregister(CMD3Shared* pModel)
{
	for (int i=0;i<m_uiLastModelNum+1;i++)
	{
		if ((CMD3Shared*)m_spModelsQueue[i]==pModel)
		{
			m_spModelsQueue[i]=NULL;
			return true;
		}
	}
	return false;
}


CMD3Shared* CMD3SharedManager::AllocFirstFreeModel(void)
{
	//assert(_CrtCheckMemory());
	for (int i=0; i<MD3_MAN_MAX_MODELS;i++)
	{		
		if ((CMD3Shared*)m_spModelsQueue[i]==NULL)
		{			
			if (i>m_uiLastModelNum)
				m_uiLastModelNum=i;
			m_spModelsQueue[i] = new CMD3Shared;

			//////////////////////////////////////
			//gex_iDebugSPNewTEXTURECalls++;
			//////////////////////////////////////

			return ((CMD3Shared*)m_spModelsQueue[i]);			
		}
	}

	/// something not good happened...
	return NULL;
}

CMD3Shared* CMD3SharedManager::GetModel(int iAbsoluteIndex)
{
	return (CMD3Shared*) m_spModelsQueue[iAbsoluteIndex];
}


CMD3Shared* CMD3SharedManager::GetModel(const char* ModelName)
{
	char* t_ModelName;
	for (int i=0; i<m_uiLastModelNum+1;i++)
	{
		if (m_spModelsQueue[i])
		{	
		t_ModelName = m_spModelsQueue[i]->GetName();
		
		if (t_ModelName!=NULL)
			if (strcmp(ModelName,t_ModelName)==0)  // same file name
			{
					CMD3Shared* pReturnModel = GetModel(i);
					return pReturnModel;
			}
		}
	}

	return NULL; // not in manager
}




/*

// if we get here, it means we already found out that this model doesn't exist in the manager,
// so we are pushing
CMD3MeshBase* CMD3MeshBaseManager::PushModel(CMD3MeshBase* pModel,const char *FileName,int iIndex)
{	
//	assert(_CrtCheckMemory());
	int iAlreadyInMan= AlreadyInManager(FileName);
	if (iAlreadyInMan<0) // not already in manager (that's ok, it's supposed to be like this)
	{

		CTexture* pTempTex;
		pTempTex = AllocFirstFreeTex();


		if (pTempTex->LoadTexture(FileName,bMipMap,bClamp))  // load was successfull
			return pTempTex;
		else  // load FAILED
		{

			int iErrorImageInMan = AlreadyInManager("ceImages/Error.jpg");
			if (iErrorImageInMan<0)			
			{
				(pTempTex->LoadTexture("ceImages/Error.jpg",bMipMap,bClamp));
				return pTempTex;
			}
			else
			{
				delete pTempTex;
				/////////////////////////
				//gex_iDebugSPDeleteCalls++;
				/////////////////////////
				return ((CTexture*)m_spTexturesQueue[iErrorImageInMan]);
			}
		}
	}
	else // if we made a mistake and somehow we got here when we already have this mesh in the queue
	{
		return ((CMD3MeshBase*)m_spModelsQueue[iAlreadyInMan]);
	}
}
*/