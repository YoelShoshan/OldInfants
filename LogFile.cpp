#include "LogFile.h"
#include <fstream>


///////////////////////////
#include "ZedTracker.h"
///////////////////////////

CLogFile::CLogFile()
{
	m_strFileName = "LOG.html";
	m_LogFile.open(m_strFileName.c_str());

	m_LogFile<<"<html><head><title>ZeDuS Engine Log File</title></head><body><font face=""Arial"" size=""5"" color=""#000099""><b><u>ZeDuS Engine Log File</u></b></font>";
	m_LogFile<<"<br>\n";

	OutPutPlainText("CLogFile::Logging process started.","4","000000",true);
}

CLogFile::~CLogFile()
{
	m_LogFile<<"</body></html>";
	m_LogFile.close();
}


void CLogFile::OutPutPlainText(const char* strOutPut,char* strFontSize,char* strFontColor,bool bEndLine)
{
	if (strOutPut==NULL) 
		return;

	m_LogFile<<"<font face=""Arial"" size=""";
	m_LogFile<<strFontSize;
	m_LogFile<<""" color=""#";
	m_LogFile<<strFontColor;
	m_LogFile<<"""><b>";
	m_LogFile<<strOutPut;
	m_LogFile<<"</b></font>";
	if (bEndLine==true)
		m_LogFile<<"<br>\n";

	m_LogFile.flush();
}

