#include "TextMessages.h"

#include <Windows.h>
#include <GL/gl.h>
#include <stdio.h>
#include "GLFonts.h"
extern CFonts      g_Fonts; // fonts global variable (draw 2d text to screen)

#include "Timer.h"
#include "Singleton.h"


#include "LogFile.h"
extern CLogFile g_LogFile;

	

CTextMessages::CTextMessages()
{
	for (int i=0;i<TEXT_MESSAGES_MAX_MESSAGES;i++)
		ZeroMemory(&Messages[i].caMessage[0],MESSAGE_SIZE_MAX);		

	//m_fLastMessageTime = TIMER_GET_TIME;
	m_fLastMessageTime=-1.f;
	m_iActiveMessages=0;
		
	/*sprintf(&Messages[0].caMessage[0],"First Message");
	sprintf(&Messages[1].caMessage[0],"Second Message");
	sprintf(&Messages[2].caMessage[0],"Third Message");
	sprintf(&Messages[3].caMessage[0],"Fourth Message");
	sprintf(&Messages[4].caMessage[0],"Fifth Message");

	m_iActiveMessages=5;*/
}

void CTextMessages::DrawMessages(void)
{
	for (int i=0;i<TEXT_MESSAGES_MAX_MESSAGES;i++)
	if (Messages[i].caMessage[0]!=NULL)
	{

		if (Messages[i].iMessageType==TEXT_MESSAGE_TYPE_SYSTEM)
		{
			glColor4f(1.f,0.f,0.f,1.f);
			glRasterPos2f(70.f,70.f+25.f*((float)i));
			g_Fonts.glPrint("Sys: ");
			
			glColor4f(Messages[i].r,Messages[i].g,Messages[i].b,1.f);
			glRasterPos2f(70.f+60.f,70.f+25.f*((float)i));
		}

		if (Messages[i].iMessageType==TEXT_MESSAGE_TYPE_GAME)
		{
			glColor4f(1.f,0.f,0.f,1.f);
			glRasterPos2f(70.f,70.f+25.f*((float)i));
			g_Fonts.glPrint("Game: ");
			
			glColor4f(Messages[i].r,Messages[i].g,Messages[i].b,1.f);
			glRasterPos2f(70.f+60.f,70.f+25.f*((float)i));
		}


		if (Messages[i].iMessageType==TEXT_MESSAGE_TYPE_CHAT_YOU)
		{
			glColor4f(0.f,1.f,0.f,1.f);
			glRasterPos2f(70.f,70.f+25.f*((float)i));
			g_Fonts.glPrint("You: ");
			
			glColor4f(Messages[i].r,Messages[i].g,Messages[i].b,1.f);
			glRasterPos2f(70.f+60.f,70.f+25.f*((float)i));
		}


		if (Messages[i].iMessageType==TEXT_MESSAGE_TYPE_CHAT_OPONENT)
		{
			glColor4f(0.f,0.f,1.f,1.f);
			glRasterPos2f(70.f,70.f+25.f*((float)i));
			g_Fonts.glPrint("Opponent: ");
			
			glColor4f(Messages[i].r,Messages[i].g,Messages[i].b,1.f);
			glRasterPos2f(70.f+120.f,70.f+25.f*((float)i));
		}


		g_Fonts.glPrint(&Messages[i].caMessage[0]);	// Print GL Text To The Screen	
		
	}
}

void CTextMessages::UpdateMessages(void)
{
	if (m_iActiveMessages<1)
		return;

	if (TIMER_GET_TIME-m_fLastMessageTime>MESSAGE_TIMEOUT)
	{		
		for (int i=0;i<TEXT_MESSAGES_MAX_MESSAGES-1;i++)
		{
			strcpy(&Messages[i].caMessage[0],&Messages[i+1].caMessage[0]);			
			Messages[i].r = Messages[i+1].r;
			Messages[i].g = Messages[i+1].g;
			Messages[i].b = Messages[i+1].b;
			Messages[i].iMessageType = Messages[i+1].iMessageType;
				 
		}

		ZeroMemory(&Messages[TEXT_MESSAGES_MAX_MESSAGES-1].caMessage[0],MESSAGE_SIZE_MAX);
		

		m_fLastMessageTime=TIMER_GET_TIME;
		m_iActiveMessages--;
	}

}

void CTextMessages::AddMessage(char* cpMessage, float r, float g, float b,int iMessageType)
{
	if (!cpMessage)
		return;

	if (strlen(cpMessage)>MESSAGE_SIZE_MAX-1)
		return;

	if (m_iActiveMessages>TEXT_MESSAGES_MAX_MESSAGES-1)
	{
		for (int i=0;i<TEXT_MESSAGES_MAX_MESSAGES-1;i++)
		{
			strcpy(&Messages[i].caMessage[0],&Messages[i+1].caMessage[0]);			
			Messages[i].r = Messages[i+1].r;
			Messages[i].g = Messages[i+1].g;
			Messages[i].b = Messages[i+1].b;
			Messages[i].iMessageType = Messages[i+1].iMessageType;
		}

		

		ZeroMemory(&Messages[TEXT_MESSAGES_MAX_MESSAGES-1].caMessage[0],MESSAGE_SIZE_MAX);		
		

		m_iActiveMessages--;
	}
	

	m_fLastMessageTime=TIMER_GET_TIME;
	Messages[m_iActiveMessages].r = r;
	Messages[m_iActiveMessages].g = g;
	Messages[m_iActiveMessages].b = b;
	Messages[m_iActiveMessages].iMessageType = iMessageType;

	strcpy(&Messages[m_iActiveMessages++].caMessage[0],cpMessage);	


	g_LogFile.OutPutPlainText(cpMessage,"3","000099",true);

}

void CTextMessages::AddMessage(char* cpMessage)
{
	if (!cpMessage)
		return;

	if (strlen(cpMessage)>MESSAGE_SIZE_MAX-1)
		return;

	if (m_iActiveMessages>TEXT_MESSAGES_MAX_MESSAGES-1)
	{
		for (int i=0;i<TEXT_MESSAGES_MAX_MESSAGES-1;i++)
		{
			strcpy(&Messages[i].caMessage[0],&Messages[i+1].caMessage[0]);			
			Messages[i].r = Messages[i+1].r;
			Messages[i].g = Messages[i+1].g;
			Messages[i].b = Messages[i+1].b;
			Messages[i].iMessageType = Messages[i+1].iMessageType;
		}

		ZeroMemory(&Messages[TEXT_MESSAGES_MAX_MESSAGES-1].caMessage[0],MESSAGE_SIZE_MAX);		
		

		m_iActiveMessages--;
	}
	

	m_fLastMessageTime=TIMER_GET_TIME;
	Messages[m_iActiveMessages].r = 0.f;
	Messages[m_iActiveMessages].g = 1.f;
	Messages[m_iActiveMessages].b = 0.f;
	Messages[m_iActiveMessages].iMessageType = TEXT_MESSAGE_TYPE_CHAT_YOU;


	strcpy(&Messages[m_iActiveMessages++].caMessage[0],cpMessage);	

	g_LogFile.OutPutPlainText(cpMessage,"3","000099",true);
	
}