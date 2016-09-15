#ifndef YOEL_LOGFILE_H
#define YOEL_LOGFILE_H

#pragma once

#include <fstream>
#include <string>

class CLogFile
{
public:
	CLogFile();
	~CLogFile();

	// "translates" simple text to html (always adds <br> in the end)
	void OutPutPlainText(const char* strOutPut,char* strFontSize,char* strFontColor,bool bEndLine);

private:
	std::ofstream m_LogFile;
	std::string   m_strFileName;
};






#endif // YOEL_LOGFILE_H