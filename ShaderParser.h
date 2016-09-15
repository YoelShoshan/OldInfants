#ifndef YOEL_SHADER_PARSER_H
#define YOEL_SHADER_PARSER_H

#pragma once

#include "Shader.h"

#include "File.h"

#include <Vector>
#include <String>
#include <Map>

//using namespace std;

////////////////////////////
//
// USE STL STRINGS!!!!!!!!!
//
////////////////////////////

struct SCacheObject
{
	int iFileNum;
	int iPos;

	SCacheObject():iFileNum(-1),iPos(-1){};
	SCacheObject(int FileNum,int Pos):iFileNum(FileNum),iPos(iPos){};
};

#define st_ShaderParser CSingleton<CShaderParser>

class CShaderParser
{
public:
	CShaderParser();
	~CShaderParser();

	void BuildFileList(char* Path, char* What); // for now only search one path
	void DebugDrawFileList(void);

	
	void CacheFile(const char* FileName,int filenum);
	void BuildCacheMap(void);
	bool FindInCache(const char* find);
	void DebugDrawCacheMap(void);
	/*void DebugOutPutCacheMapToFile(void);*/

	CShader* ParseShader(char* ShaderName);


private:

	FILE_VECTOR         m_ShaderCacheFileVector;

	typedef std::map<std::string,SCacheObject> SHADER_CACHE_MAP;
	typedef SHADER_CACHE_MAP::iterator SHADER_CACHE_MAP_ITERATOR;

	SHADER_CACHE_MAP          m_ShaderCacheMap;
	SHADER_CACHE_MAP_ITERATOR m_ShaderCacheMapIT;

};



#endif // YOEL_SHADER_PARSER_H