#include "GUI.h"

#include "Renderer.h"

#include "VertexBuffer.h"

#include <gl/gl.h>
extern CVertexBuffer* g_pvbCharacterFrame;

#include "VertexBuffer.h"
CVertexBuffer *g_pTempVB;

#include "Input.h"
#include "Engine.h"

#include <assert.h>
#include <crtdbg.h>

#include "Timer.h"

#include "CheckersBoard.h"
extern CCheckersBoard* g_pCheckersBoard;

#include "GLFonts.h"
extern CFonts      g_Fonts; // fonts global variable (draw 2d text to screen)

extern char defaultIP[16];

#include <assert.h>
#include <crtdbg.h>

extern char chat[500];

extern int g_iCardTexUnits; // num of tex units on current card

#include "Communication.h"
extern CCommunication g_Communication;

// sound manager
#include "SoundManager.h"
extern CSoundManager g_SoundManager;

bool CGUIBase::IsMouseOn(void)
{
	int m_iAbsoluteLeftX,m_iAbsoluteRightX,m_iAbsoluteUpY,m_iAbsoluteDownY;

	if (m_piFatherX && m_piFatherY)
	{
		m_iAbsoluteLeftX  = (*m_piFatherX) + m_iPosX;
		m_iAbsoluteRightX = (*m_piFatherX) + m_iPosX + m_iWidth;
		m_iAbsoluteUpY   = (*m_piFatherY) + m_iPosY;
		m_iAbsoluteDownY = (*m_piFatherY) + m_iPosY + m_iHeight;
	}
	else
	{
		m_iAbsoluteLeftX  = m_iPosX;
		m_iAbsoluteRightX = m_iPosX + m_iWidth;
		m_iAbsoluteUpY   =  m_iPosY;
		m_iAbsoluteDownY = m_iPosY + m_iHeight;

	}

	if ( st_Input::Inst()->GetAbsoluteX()>=m_iAbsoluteLeftX && st_Input::Inst()->GetAbsoluteX()<=m_iAbsoluteRightX
		&& st_Input::Inst()->GetAbsoluteY()>=m_iAbsoluteUpY && st_Input::Inst()->GetAbsoluteY()<=m_iAbsoluteDownY)
		return true;

	return false;	
}

////////////////////////////// IMAGE IMAGE IMAGE IMAGE IMAGE ////////////////////////


void CGUIImage::Render(void)
{
	if (!m_bActive)
		return;

	// render the image 
	/**/
	st_Renderer::Inst()->InitTexUnits();
	/**/

	g_pTempVB->SetShaderSP(m_spShader);	
	st_Renderer::Inst()->RenderVB(g_pTempVB,false,true); 
	glColor4f(1.f,1.f,1.f,1.f);
	st_Renderer::Inst()->DebugDraw2DQuad(NULL,
		(*m_piFatherX)+m_iPosX,(*m_piFatherX)+m_iPosX+m_iWidth,
		(*m_piFatherY)+m_iPosY,(*m_piFatherY)+m_iPosY+m_iHeight,false);

}




////////////////////////////// BUTTON BUTTON BUTTON BUTTON BUTTON ////////////////////////

/*CGUIButton::~CGUIButton()
{

}*/

void CGUIButton::SetShaderUp(CShader* pSet)
{
	m_spShaderUp = pSet;
}

void CGUIButton::SetShaderDown(CShader* pSet)
{
	m_spShaderDown = pSet;
}

void CGUIButton::SetShaderHover(CShader* pSet)
{
	m_spShaderHover = pSet;

}

void CGUIButton::Render(void)
{

	// render the image 
	/**/
	st_Renderer::Inst()->InitTexUnits();
	/**/

	// render the button 

	if (m_iState==BUTTON_STATE_UP)
		g_pTempVB->SetShaderSP(m_spShaderUp);	
	else
	if (m_iState==BUTTON_STATE_DOWN)
		g_pTempVB->SetShaderSP(m_spShaderDown);	
	else
	if (m_iState==BUTTON_STATE_HOVER)
		g_pTempVB->SetShaderSP(m_spShaderHover);	

	st_Renderer::Inst()->RenderVB(g_pTempVB,false,true); 
	
	
	if (m_bActive)
	{
		glColor4f(1.f,1.f,1.f,1.f);
	st_Renderer::Inst()->DebugDraw2DQuad(NULL,
		(*m_piFatherX)+m_iPosX,(*m_piFatherX)+m_iPosX+m_iWidth,
		(*m_piFatherY)+m_iPosY,(*m_piFatherY)+m_iPosY+m_iHeight,false);
	}
	/*else
		glColor4f(0.5f,0.5f,0.5f,0.5f);*/
	
	

}

void CGUIButton::OnMouse(void)
{
	// first check if it's in the zone
	
	if (IsMouseOn())
	{
		if (st_Input::Inst()->MouseButtonDownFirstFrame(DIMOUSE_LEFTBUTTON))
		{
			m_iState=BUTTON_STATE_DOWN;

			if (m_iMessagesReciever==MESSAGES_RECIEVER_ENGINE)
				st_EngineI->AddEngineMessage(m_dwPotentialMessage);
			else
			if (m_iMessagesReciever==MESSAGES_RECIEVER_GUI)
				st_EngineI->AddGUIMessage(m_dwPotentialMessage);

			
		}
		else
			m_iState=BUTTON_STATE_HOVER;
	}
	else
		m_iState=BUTTON_STATE_UP;

}


void CGUIEditBox::Render(void)
{
	//glColor4f(1.f,0.f,0.f,1.f);
	glColor4f(0.f,1.f,0.f,1.f);
	glRasterPos2f((*m_piFatherX)+m_iPosX,(*m_piFatherY)+m_iPosY);

	int iLength = strlen(&caText[0]);
	// if no need to scroll
	if (iLength<=m_iBoxLength)
	{			
		g_Fonts.glPrint(&caText[0]);
		return;
	}
	
	// scroll
	iLength-=m_iBoxLength;
	g_Fonts.glPrint(&caText[iLength]);			

}


void CGUIEditBox::OnMouse(void)
{

}

void CGUIEditBox::OnKeyboard(void)
{
	if (!m_bWritable)
		return;

	if (m_bIgnoreWriteMode)
		m_bWriteMode=true;

	if (m_bClearNextTime)
	{
		m_iPositionInText=0;
		caText[0] = '_';			
		caText[1] = NULL;	
		m_bClearNextTime=false;
	}


	if (!m_bWriteMode)
	{
		bool bWriteMode=false;
		if(st_Input::Inst()->KeyPressed(DIK_RETURN))
		{
				if (st_Input::Inst()->m_WPKeyBuffer[DIK_RETURN]==false)
				{
					st_Input::Inst()->m_WPKeyBuffer[DIK_RETURN]=true;									
					
					bWriteMode=true;
				}		
		}
		else
			st_Input::Inst()->m_WPKeyBuffer[DIK_RETURN]=false;


			if(st_Input::Inst()->KeyPressed(DIK_NUMPADENTER))
		{
				if (st_Input::Inst()->m_WPKeyBuffer[DIK_NUMPADENTER]==false)
				{
					st_Input::Inst()->m_WPKeyBuffer[DIK_NUMPADENTER]=true;									
					
					bWriteMode=true;
				}		
		}
		else
			st_Input::Inst()->m_WPKeyBuffer[DIK_NUMPADENTER]=false;

		m_bWriteMode = bWriteMode;

		if (!m_bWriteMode)
			return;
	}

	if (m_iMessagesReciever>=0)
	if(st_Input::Inst()->KeyPressed(DIK_RETURN))
		{
				if (st_Input::Inst()->m_WPKeyBuffer[DIK_RETURN]==false)
				{
					st_Input::Inst()->m_WPKeyBuffer[DIK_RETURN]=true;									
					
					if (m_iPositionInText>0)
					{
						if (m_iMessagesReciever==MESSAGES_RECIEVER_ENGINE)
							st_EngineI->AddEngineMessage(m_dwPotentialMessage);
						else
						if (m_iMessagesReciever==MESSAGES_RECIEVER_GUI)
							st_EngineI->AddGUIMessage(m_dwPotentialMessage);

						if (m_iEditBoxType==GUI_EDIT_BOX_TYPE_CHAT)
							m_bClearNextTime=true;
					strcpy(&caTextBACKUP[0],&caText[0]);
					}
					m_bWriteMode=false;
				}		
		}
		else
			st_Input::Inst()->m_WPKeyBuffer[DIK_RETURN]=false;

	if (m_iMessagesReciever>=0)
	if(st_Input::Inst()->KeyPressed(DIK_NUMPADENTER))
		{
				if (st_Input::Inst()->m_WPKeyBuffer[DIK_NUMPADENTER]==false)
				{
					st_Input::Inst()->m_WPKeyBuffer[DIK_NUMPADENTER]=true;									
				
					if (m_iPositionInText>0)
					{
						if (m_iMessagesReciever==MESSAGES_RECIEVER_ENGINE)
							st_EngineI->AddEngineMessage(m_dwPotentialMessage);
						else
						if (m_iMessagesReciever==MESSAGES_RECIEVER_GUI)
							st_EngineI->AddGUIMessage(m_dwPotentialMessage);
	
						if (m_iEditBoxType==GUI_EDIT_BOX_TYPE_CHAT)
							m_bClearNextTime=true;						
					strcpy(&caTextBACKUP[0],&caText[0]);
					}
					m_bWriteMode=false;
				}		
		}
		else
			st_Input::Inst()->m_WPKeyBuffer[DIK_NUMPADENTER]=false;



	
	if(st_Input::Inst()->KeyPressed(DIK_UP))
		{
				if (st_Input::Inst()->m_WPKeyBuffer[DIK_UP]==false)
				{
					st_Input::Inst()->m_WPKeyBuffer[DIK_UP]=true;									
					
					if (caTextBACKUP[0]!=NULL && caTextBACKUP[1]!=NULL)
					{
						//char cTemp[400];
						//strcpy(&cTemp[0],&caTextBACKUP[0]);
						strcpy(&caText[0],&caTextBACKUP[0]);
						//strcpy(&caTextBACKUP[0],&cTemp[0]);

						m_iPositionInText=strlen(&caText[0])-1;
					}
				}		
		}
		else
			st_Input::Inst()->m_WPKeyBuffer[DIK_UP]=false;




	//assert(_CrtCheckMemory());

	//assert(_CrtCheckMemory());

	if (m_iPositionInText>0)
	{
		if(st_Input::Inst()->KeyPressed(DIK_BACKSPACE))
		{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_BACKSPACE_DELAY)
			{				
				m_fLastTimeButtonPressed = TIMER_GET_TIME;
				m_iPositionInText-=2;
				caText[m_iPositionInText+1] = '_';				
				caText[m_iPositionInText+2] = NULL;									
				m_iPositionInText++;		
			}
		}
		
	}

	//assert(_CrtCheckMemory());

	if (updateMe)
	{	
		if (caText[strlen(caText)-1]=='_')
			caText[strlen(caText)-1]=NULL;

		strcpy(updateMe,&caText[0]);

		if (caText[strlen(caText)]==NULL && caText[strlen(caText)+1]==NULL)
			caText[strlen(caText)]='_';
	}

	//assert(_CrtCheckMemory());

	//assert(_CrtCheckMemory());

	if (m_iPositionInText>m_iMaximumAcceptableChars)
		return;
		

	//assert(_CrtCheckMemory());

	if(st_Input::Inst()->KeyPressed(DIK_PERIOD))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_PERIOD )
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;									
				m_iLastButtonPressed=DIK_PERIOD;

				if (m_iEditBoxType!=GUI_EDIT_BOX_TYPE_IP_ADRESS || caText[m_iPositionInText-1] != '.') // 2 periods one after another don't make sense
				{
					caText[m_iPositionInText] = '.';
					caText[m_iPositionInText+1] = '_';
					caText[m_iPositionInText+2] = NULL;
					m_iPositionInText++;
				}
			}	

			return;
	}
	

	//assert(_CrtCheckMemory());


	

	if(st_Input::Inst()->KeyPressed(DIK_0))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY  || m_iLastButtonPressed!=DIK_0 )
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_0;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = '0';
				else
					caText[m_iPositionInText] = ')';

				
				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}
	

	

	if(st_Input::Inst()->KeyPressed(DIK_1))
	{

			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY  || m_iLastButtonPressed!=DIK_1 )
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;
				m_iLastButtonPressed=DIK_1;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = '1';
				else
					caText[m_iPositionInText] = '!';

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}
	


	if(st_Input::Inst()->KeyPressed(DIK_2))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY   || m_iLastButtonPressed!=DIK_2 )
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;
				m_iLastButtonPressed=DIK_2;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = '2';
				else
					caText[m_iPositionInText] = '@';
				
				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}




	if(st_Input::Inst()->KeyPressed(DIK_3))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY   || m_iLastButtonPressed!=DIK_3 )
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_3;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = '3';
				else
					caText[m_iPositionInText] = '#';

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

		return;
	}
	


	if(st_Input::Inst()->KeyPressed(DIK_4))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY  || m_iLastButtonPressed!=DIK_4 )
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;
				m_iLastButtonPressed=DIK_4;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = '4';
				else
					caText[m_iPositionInText] = '$';

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}



	if(st_Input::Inst()->KeyPressed(DIK_5))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY   || m_iLastButtonPressed!=DIK_5 )
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_5;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = '5';
				else
					caText[m_iPositionInText] = '%';

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}



	if(st_Input::Inst()->KeyPressed(DIK_6))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY   || m_iLastButtonPressed!=DIK_6 )
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;
				m_iLastButtonPressed=DIK_6;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = '6';
				else
					caText[m_iPositionInText] = '^';

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}
	


	if(st_Input::Inst()->KeyPressed(DIK_7))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY   || m_iLastButtonPressed!=DIK_7 )
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_7;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = '7';
				else
					caText[m_iPositionInText] = '&';

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}
//assert(_CrtCheckMemory());


	if(st_Input::Inst()->KeyPressed(DIK_8))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY   || m_iLastButtonPressed!=DIK_8 )
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;
				m_iLastButtonPressed=DIK_8;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = '8';
				else
					caText[m_iPositionInText] = '*';

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}
	



	if(st_Input::Inst()->KeyPressed(DIK_9))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY   || m_iLastButtonPressed!=DIK_9 )
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;
				m_iLastButtonPressed=DIK_9;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = '9';
				else
					caText[m_iPositionInText] = '(';

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}


	//assert(_CrtCheckMemory());
	
	if (m_iEditBoxType!=GUI_EDIT_BOX_TYPE_CHAT)
		return;



	

	
	if(st_Input::Inst()->KeyPressed(DIK_APOSTROPHE))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY   || m_iLastButtonPressed!=DIK_APOSTROPHE)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_APOSTROPHE;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = '\'';				
				else
					caText[m_iPositionInText] = '""';				

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}



	if(st_Input::Inst()->KeyPressed(DIK_NUMPADEQUALS))
	{
		if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY   || m_iLastButtonPressed!=DIK_NUMPADEQUALS)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;		
				m_iLastButtonPressed=DIK_NUMPADEQUALS;

				caText[m_iPositionInText] = '=';				
				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}



	if(st_Input::Inst()->KeyPressed(DIK_MINUS))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY    || m_iLastButtonPressed!=DIK_MINUS)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;
				m_iLastButtonPressed=DIK_MINUS;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = '-';				
				else
					caText[m_iPositionInText] = '_';				

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}


	

		if(st_Input::Inst()->KeyPressed(DIK_SEMICOLON))
		{
				if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY    || m_iLastButtonPressed!=DIK_SEMICOLON)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;
				m_iLastButtonPressed=DIK_SEMICOLON;
					
					if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
						caText[m_iPositionInText] = ';';				
					else
						caText[m_iPositionInText] = ':';	

					caText[m_iPositionInText+1] = '_';
					caText[m_iPositionInText+2] = NULL;
					m_iPositionInText++;
				}		
			return;
		}

	

	if(st_Input::Inst()->KeyPressed(DIK_A))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY    || m_iLastButtonPressed!=DIK_A)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;		
				m_iLastButtonPressed=DIK_A;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 'a';	
				else
					caText[m_iPositionInText] = 'A';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}



	if(st_Input::Inst()->KeyPressed(DIK_B))
	{
		if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_B)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_B;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 'b';	
				else
					caText[m_iPositionInText] = 'B';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}



	if(st_Input::Inst()->KeyPressed(DIK_C))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_C)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;
				m_iLastButtonPressed=DIK_C;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 'c';	
				else
					caText[m_iPositionInText] = 'C';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}



	if(st_Input::Inst()->KeyPressed(DIK_D))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_D)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;
				m_iLastButtonPressed=DIK_D;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 'd';	
				else
					caText[m_iPositionInText] = 'D';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}



	if(st_Input::Inst()->KeyPressed(DIK_E))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_E)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_E;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 'e';	
				else
					caText[m_iPositionInText] = 'E';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}




	if(st_Input::Inst()->KeyPressed(DIK_F))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_F)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_F;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 'f';	
				else
					caText[m_iPositionInText] = 'F';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}




	if(st_Input::Inst()->KeyPressed(DIK_G))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_G)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_G;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 'g';	
				else
					caText[m_iPositionInText] = 'G';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}


	if(st_Input::Inst()->KeyPressed(DIK_H))
	{
		if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_H)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_H;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 'h';	
				else
					caText[m_iPositionInText] = 'H';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}
//assert(_CrtCheckMemory());


	if(st_Input::Inst()->KeyPressed(DIK_I))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_I)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_I;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 'i';	
				else
					caText[m_iPositionInText] = 'I';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}




	if(st_Input::Inst()->KeyPressed(DIK_J))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY  || m_iLastButtonPressed!=DIK_J)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_J;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 'j';	
				else
					caText[m_iPositionInText] = 'J';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}




	if(st_Input::Inst()->KeyPressed(DIK_K))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_K)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_K;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 'k';	
				else
					caText[m_iPositionInText] = 'K';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}
	




	if(st_Input::Inst()->KeyPressed(DIK_L))
	{
		if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_L)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_L;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 'l';	
				else
					caText[m_iPositionInText] = 'L';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}



	if(st_Input::Inst()->KeyPressed(DIK_M))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_M)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;								
				m_iLastButtonPressed=DIK_M;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 'm';	
				else
					caText[m_iPositionInText] = 'M';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}
	



	if(st_Input::Inst()->KeyPressed(DIK_N))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_N)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;			
				m_iLastButtonPressed=DIK_N;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 'n';	
				else
					caText[m_iPositionInText] = 'N';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}



	if(st_Input::Inst()->KeyPressed(DIK_O))
	{
		if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_O)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;								
				m_iLastButtonPressed=DIK_O;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 'o';	
				else
					caText[m_iPositionInText] = 'O';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}




	if(st_Input::Inst()->KeyPressed(DIK_P))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_P)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_P;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 'p';	
				else
					caText[m_iPositionInText] = 'P';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}

//assert(_CrtCheckMemory());

	if(st_Input::Inst()->KeyPressed(DIK_Q))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_Q)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;
				m_iLastButtonPressed=DIK_Q;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 'q';	
				else
					caText[m_iPositionInText] = 'Q';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}
	



	if(st_Input::Inst()->KeyPressed(DIK_R))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_R)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_R;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 'r';	
				else
					caText[m_iPositionInText] = 'R';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}
	



	if(st_Input::Inst()->KeyPressed(DIK_S))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_S)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_S;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 's';	
				else
					caText[m_iPositionInText] = 'S';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}
	


	if(st_Input::Inst()->KeyPressed(DIK_T))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_T)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_T;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 't';	
				else
					caText[m_iPositionInText] = 'T';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}
	


	if(st_Input::Inst()->KeyPressed(DIK_U))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_U)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;
				m_iLastButtonPressed=DIK_U;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 'u';	
				else
					caText[m_iPositionInText] = 'U';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}




	if(st_Input::Inst()->KeyPressed(DIK_V))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_V)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;
				m_iLastButtonPressed=DIK_V;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 'v';	
				else
					caText[m_iPositionInText] = 'V';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}



	
	if(st_Input::Inst()->KeyPressed(DIK_W))
	{
		if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_W)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_W;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 'w';	
				else
					caText[m_iPositionInText] = 'W';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}
	


	if(st_Input::Inst()->KeyPressed(DIK_X))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_X)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_X;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 'x';	
				else
					caText[m_iPositionInText] = 'X';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}



	if(st_Input::Inst()->KeyPressed(DIK_Y))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_Y)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;									
				m_iLastButtonPressed=DIK_Y;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 'y';	
				else
					caText[m_iPositionInText] = 'Y';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}
	


	if(st_Input::Inst()->KeyPressed(DIK_Z))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_Z)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_Z;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = 'z';	
				else
					caText[m_iPositionInText] = 'Z';	

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}
	



	if(st_Input::Inst()->KeyPressed(DIK_SPACE))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_SPACE)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_SPACE;

				caText[m_iPositionInText] = ' ';
				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}




	if(st_Input::Inst()->KeyPressed(DIK_ADD))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_ADD)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_ADD;

				caText[m_iPositionInText] = '+';
				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}



	if(st_Input::Inst()->KeyPressed(DIK_SUBTRACT))
	{
		if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_SUBTRACT)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;
				m_iLastButtonPressed=DIK_SUBTRACT;


				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = '-';
				else
					caText[m_iPositionInText] = '_';

				
				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}



	if(st_Input::Inst()->KeyPressed(DIK_UNDERLINE))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_UNDERLINE)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_UNDERLINE;

				caText[m_iPositionInText] = '_';
				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}
	




	if(st_Input::Inst()->KeyPressed(DIK_LBRACKET))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_LBRACKET)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_LBRACKET;

				if (!st_Input::Inst()->m_WPKeyBuffer[DIK_LSHIFT]==false && !st_Input::Inst()->m_WPKeyBuffer[DIK_LSHIFT]==false)
					caText[m_iPositionInText] = '[';
				else
					caText[m_iPositionInText] = '{';

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}
	


	if(st_Input::Inst()->KeyPressed(DIK_RBRACKET))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_RBRACKET)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_RBRACKET;

				if (!st_Input::Inst()->m_WPKeyBuffer[DIK_LSHIFT]==false && !st_Input::Inst()->m_WPKeyBuffer[DIK_LSHIFT]==false)
					caText[m_iPositionInText] = ']';
				else
					caText[m_iPositionInText] = '}';

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}



	if(st_Input::Inst()->KeyPressed(DIK_NUMPADSTAR))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_NUMPADSTAR)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_NUMPADSTAR;

				caText[m_iPositionInText] = '*';

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}


	
	


	if(st_Input::Inst()->KeyPressed(DIK_SLASH))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_SLASH)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;
				m_iLastButtonPressed=DIK_SLASH;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = '/';
				else
					caText[m_iPositionInText] = '?';

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}


	

	     

	if(st_Input::Inst()->KeyPressed(DIK_COMMA))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_COMMA)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_COMMA;

				caText[m_iPositionInText] = ',';
				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}


	if(st_Input::Inst()->KeyPressed(DIK_EQUALS))
	{
		if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY || m_iLastButtonPressed!=DIK_EQUALS)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;	
				m_iLastButtonPressed=DIK_EQUALS;

				if(!st_Input::Inst()->KeyPressed(DIK_LSHIFT) && !st_Input::Inst()->KeyPressed(DIK_RSHIFT))
					caText[m_iPositionInText] = '=';
				else
					caText[m_iPositionInText] = '+';

				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}


	if(st_Input::Inst()->KeyPressed(DIK_BACKSLASH))
	{
			if (TIMER_GET_TIME>m_fLastTimeButtonPressed+GUI_EDIT_BOX_PRESS_DELAY  || m_iLastButtonPressed!=DIK_BACKSLASH)
			{
				m_fLastTimeButtonPressed = TIMER_GET_TIME;								
				m_iLastButtonPressed=DIK_BACKSLASH;

				caText[m_iPositionInText] = '\\';
				caText[m_iPositionInText+1] = '_';
				caText[m_iPositionInText+2] = NULL;
				m_iPositionInText++;
			}		

			return;
	}



	//assert(_CrtCheckMemory());
	
}


////////////////////////////// 2 STATES BUTTON //////////////////////////////////////////

////////////////////////////// BUTTON BUTTON BUTTON BUTTON BUTTON ////////////////////////

/*CGUIButton::~CGUIButton()
{

}*/

void CGUI2StatesButton::SetShaderState1(CShader* pSet)
{
	m_spShaderState1 = pSet;
}

void CGUI2StatesButton::SetShaderState2(CShader* pSet)
{
	m_spShaderState2 = pSet;
}

void CGUI2StatesButton::Render(void)
{
	// render the image 
	/**/
	st_Renderer::Inst()->InitTexUnits();
	/**/
	// render the button 

	if (m_iState==TWO_SIDES_BUTTON_STATE_1)
		g_pTempVB->SetShaderSP(m_spShaderState1);	
	else
	if (m_iState==TWO_SIDES_BUTTON_STATE_2)
		g_pTempVB->SetShaderSP(m_spShaderState2);	
	
	st_Renderer::Inst()->RenderVB(g_pTempVB,false,true); 

	if (m_bActive)
		glColor4f(1.f,1.f,1.f,1.f);
	else
		glColor4f(0.5f,0.5f,0.5f,0.5f);

	st_Renderer::Inst()->DebugDraw2DQuad(NULL,
		(*m_piFatherX)+m_iPosX,(*m_piFatherX)+m_iPosX+m_iWidth,
		(*m_piFatherY)+m_iPosY,(*m_piFatherY)+m_iPosY+m_iHeight,false);
	

}

void CGUI2StatesButton::OnMouse(void)
{
	if (!m_bActive)
		return;
	// first check if it's in the zone
	
	if (IsMouseOn())
	{
		if (st_Input::Inst()->MouseButtonDownFirstFrame(DIMOUSE_LEFTBUTTON))
		{

			if (m_iMessagesReciever==MESSAGES_RECIEVER_ENGINE)
				st_EngineI->AddEngineMessage(m_dwPotentialMessage);
			else
			if (m_iMessagesReciever==MESSAGES_RECIEVER_GUI)
				st_EngineI->AddGUIMessage(m_dwPotentialMessage);

			/*if (m_iState==TWO_SIDES_BUTTON_STATE_1)*/
				m_iState=TWO_SIDES_BUTTON_STATE_2;
			/*else
				m_iState=TWO_SIDES_BUTTON_STATE_1;*/
			
		}
		/*else
			m_iState=BUTTON_STATE_HOVER;*/
	}
	/*else
		m_iState=BUTTON_STATE_UP;*/

}


////////////////////////////// WINDOW WINDOW WINDOW WINDOW WINDOW  ////////////////////////

void CGUIWindow::Init(int iElementsNum)
{
	m_iTotalElementsNum = iElementsNum;
	m_ppElements = new CGUIBase*[m_iTotalElementsNum];

	for (int i=0;i<m_iTotalElementsNum;i++)
		m_ppElements[i] = NULL;

	
}

// currently just adds buttons
CGUIBase* CGUIWindow::AddElement(int iType)
{
	m_iCurrentElementNum++;
	
	

	if (iType==GUI_TYPE_BUTTON_SIMPLE)
	{
		m_ppElements[m_iCurrentElementNum-1] = new CGUIButton;
		m_ppElements[m_iCurrentElementNum-1]->m_iType = iType;

		m_ppElements[m_iCurrentElementNum-1]->m_piFatherX = &m_iPosX;
		m_ppElements[m_iCurrentElementNum-1]->m_piFatherY = &m_iPosY;

		return m_ppElements[m_iCurrentElementNum-1];
	}
	else
	if (iType==GUI_TYPE_IMAGE)
	{
		m_ppElements[m_iCurrentElementNum-1] = new CGUIImage;
		m_ppElements[m_iCurrentElementNum-1]->m_iType = iType;

		m_ppElements[m_iCurrentElementNum-1]->m_piFatherX = &m_iPosX;
		m_ppElements[m_iCurrentElementNum-1]->m_piFatherY = &m_iPosY;

		return m_ppElements[m_iCurrentElementNum-1];
	}
	else
	if (iType==GUI_TYPE_BUTTON_2_STATES)
	{
		m_ppElements[m_iCurrentElementNum-1] = new CGUI2StatesButton;
		m_ppElements[m_iCurrentElementNum-1]->m_iType = iType;

		m_ppElements[m_iCurrentElementNum-1]->m_piFatherX = &m_iPosX;
		m_ppElements[m_iCurrentElementNum-1]->m_piFatherY = &m_iPosY;

		return m_ppElements[m_iCurrentElementNum-1];
	}		
	else
	if (iType==GUI_TYPE_TEXT_EDIT_BOX)
	{
		m_ppElements[m_iCurrentElementNum-1] = new CGUIEditBox;
		m_ppElements[m_iCurrentElementNum-1]->m_iType = iType;

		m_ppElements[m_iCurrentElementNum-1]->m_piFatherX = &m_iPosX;
		m_ppElements[m_iCurrentElementNum-1]->m_piFatherY = &m_iPosY;

		return m_ppElements[m_iCurrentElementNum-1];
	}		
	else
	return NULL;

}

CGUIBase* CGUIWindow::GetElementNum(int iNum)
{
	if (iNum<m_iTotalElementsNum && iNum>=0)
		return m_ppElements[iNum];
}

void CGUIWindow::OnKeyboard(void)
{
	if (!m_bActive)
		return;

	for (int i=0;i<m_iCurrentElementNum;i++)
		if (m_ppElements[i])
			m_ppElements[i]->OnKeyboard();
}

void CGUIWindow::OnMouse(void)
{
	if (!m_bActive)
		return;

	int iTemp;

	// if animated then animate it

	if (IsMouseOn() && m_bAnimatedPop)
	{
		if (m_iAnimatedFinalY!=m_iPosY)
		{
			iTemp= ((int)   ((float) abs(m_iAnimatedFinalY-m_iPosY)) * st_Timer::Inst()->GetDeltaTimeFAKE()*0.01f);


			if (m_iAnimatedFinalY>m_iAnimatedStartY)
			{
				if (iTemp!=0)
					m_iPosY+=iTemp;
				else
					m_iPosY+=1;
			}
			else
			{
				if (iTemp!=0)
					m_iPosY-=iTemp;
				else
					m_iPosY-=1;
				
			}

			if (m_iAnimatedFinalY>m_iAnimatedStartY)
			{
				if (m_iPosY>m_iAnimatedFinalY)
					m_iPosY=m_iAnimatedFinalY;	
			}
			else
			{
				if (m_iPosY<m_iAnimatedFinalY)
					m_iPosY=m_iAnimatedFinalY;	
			}
		}       




		if (m_iAnimatedFinalX!=m_iPosX)
		{
			iTemp= ((int)   ((float) abs(m_iAnimatedFinalX-m_iPosX)) * st_Timer::Inst()->GetDeltaTimeFAKE()*0.01f);


			if (m_iAnimatedFinalX>m_iAnimatedStartX)
			{
				if (iTemp!=0)
					m_iPosX+=iTemp;
				else
					m_iPosX+=1;
			}
			else
			{
				if (iTemp!=0)
					m_iPosX-=iTemp;
				else
					m_iPosX-=1;
				
			}

			if (m_iAnimatedFinalX>m_iAnimatedStartX)
			{
				if (m_iPosX>m_iAnimatedFinalX)
					m_iPosX=m_iAnimatedFinalX;	
			}
			else
			{
				if (m_iPosX<m_iAnimatedFinalX)
					m_iPosX=m_iAnimatedFinalX;	
			}
		}       
	}
	else // animated but mouse isn't on
	if (m_bAnimatedPop)
	{
		if (m_iAnimatedStartY!=m_iPosY)
		{
			iTemp= ((int)   ((float) abs(m_iAnimatedStartY-m_iPosY)) * st_Timer::Inst()->GetDeltaTimeFAKE()*0.01f);

			if (m_iAnimatedStartY>m_iAnimatedFinalY)
			{
				if (iTemp!=0)
					m_iPosY+=iTemp;
				else
					m_iPosY+=1;
			}
			else
			{
				if (iTemp!=0)
					m_iPosY-=iTemp;
				else
					m_iPosY-=1;
			}

			if (m_iAnimatedStartY>m_iAnimatedFinalY)
			{
				if (m_iPosY>m_iAnimatedStartY)
					m_iPosY=m_iAnimatedStartY;	
			}
			else
			{
				if (m_iPosY<m_iAnimatedStartY)
					m_iPosY=m_iAnimatedStartY;	
			}

		}  



		if (m_iAnimatedStartX!=m_iPosX)
		{
			iTemp= ((int)   ((float) abs(m_iAnimatedStartX-m_iPosX)) * st_Timer::Inst()->GetDeltaTimeFAKE()*0.01f);

			if (m_iAnimatedStartX>m_iAnimatedFinalX)
			{
				if (iTemp!=0)
					m_iPosX+=iTemp;
				else
					m_iPosX+=1;
			}
			else
			{
				if (iTemp!=0)
					m_iPosX-=iTemp;
				else
					m_iPosX-=1;
			}

			if (m_iAnimatedStartX>m_iAnimatedFinalX)
			{
				if (m_iPosX>m_iAnimatedStartX)
					m_iPosX=m_iAnimatedStartX;	
			}
			else
			{
				if (m_iPosX<m_iAnimatedStartX)
					m_iPosX=m_iAnimatedStartX;	
			}

		}  
	
	}
	

	for (int i=0;i<m_iCurrentElementNum;i++)
		if (m_ppElements[i])
			m_ppElements[i]->OnMouse();
}

void CGUIWindow::Render(void)
{
	if (!m_bRender)
		return;
	// first draw the menu itself

	g_pTempVB->SetShaderSP(m_spShader);	


	/**/
	st_Renderer::Inst()->InitTexUnits();
	/**/

	st_Renderer::Inst()->RenderVB(g_pTempVB,false,true); 
	glColor4f(1.f,1.f,1.f,1.f);
	st_Renderer::Inst()->DebugDraw2DQuad(NULL,m_iPosX,m_iPosX+m_iWidth,m_iPosY,m_iPosY+m_iHeight,false);

	// now render the elements

	for (int i=0;i<m_iCurrentElementNum;i++)
		if (m_ppElements[i])
			if (m_ppElements[i]->m_iType!=GUI_TYPE_TEXT_EDIT_BOX)
			m_ppElements[i]->Render();
}

void CGUIWindow::RenderText(void)
{
	if (!m_bRender)
		return;

	for (int i=0;i<m_iCurrentElementNum;i++)
		if (m_ppElements[i])
			if (m_ppElements[i]->m_iType==GUI_TYPE_TEXT_EDIT_BOX)
			m_ppElements[i]->Render();
}


CGUIManager::CGUIManager()
{
	Init(TRUE);
}

void CGUIManager::Init(bool bFirst)
{
	assert(_CrtCheckMemory());
	
	m_bChoseNetwork=false;

	m_dwGUIMessages=0;

	if (bFirst)
		g_pTempVB = new CVertexBuffer;

	


	if (bFirst)
		m_pBoardCharacterMenu = new CGUIWindow;

	/*// left up corner
	m_pBoardCharacterMenu->m_iPosX = 0;
	m_pBoardCharacterMenu->m_iPosY = 0;

	// height/width
	m_pBoardCharacterMenu->m_iWidth=400;
	m_pBoardCharacterMenu->m_iHeight=100;*/

	int iRight, iDown;

	/*st_RendererI->GetAnchoredValues((ANCHOR_X_MAX|ANCHOR_Y_MAX),300,300,0,0,
		m_pBoardCharacterMenu->m_iPosX,iRight,iDown,m_pBoardCharacterMenu->m_iPosY);*/

	st_RendererI->GetAnchoredValues((ANCHOR_X_MAX),300,300,0,70,
		m_pBoardCharacterMenu->m_iPosX,iRight,iDown,m_pBoardCharacterMenu->m_iPosY);


	m_pBoardCharacterMenu->m_iPosX+=150;
	iRight+=150;

	m_pBoardCharacterMenu->m_iWidth = iRight - m_pBoardCharacterMenu->m_iPosX;
	m_pBoardCharacterMenu->m_iHeight = iDown - m_pBoardCharacterMenu->m_iPosY;

	m_pBoardCharacterMenu->m_iAnimatedStartX = m_pBoardCharacterMenu->m_iPosX;
	m_pBoardCharacterMenu->m_iAnimatedStartY = m_pBoardCharacterMenu->m_iPosY;

	m_pBoardCharacterMenu->m_iAnimatedFinalX = m_pBoardCharacterMenu->m_iPosX-150;
	m_pBoardCharacterMenu->m_iAnimatedFinalY = m_pBoardCharacterMenu->m_iPosY;

	m_pBoardCharacterMenu->m_bAnimatedPop=true;


	char Name[200];
	CGUIButton* pButton;
	
	if (bFirst)
	{
		
		strcpy(Name,"menuCharaterFrame");
		CShader* pShader = st_ShaderManager::Inst()->Load(&Name[0]);
		
		m_pBoardCharacterMenu->SetShader(pShader);

		
		//g_pTempVB->SetShaderSP(pShader);		
	}

	
	//assert(_CrtCheckMemory());

	if (bFirst)
	{
		// init the character frame menu
		m_pBoardCharacterMenu->Init(10);		


		// create the character empty image

		CGUIImage* pImage = (CGUIImage*) m_pBoardCharacterMenu->AddElement(GUI_TYPE_IMAGE);
		pImage->m_iPosX = 20;
		pImage->m_iPosY = 20;
		pImage->m_iWidth  = 150;
		pImage->m_iHeight = 220;

		strcpy(Name,"menuCharaterEmpty");		
		pImage->SetShader(st_ShaderManager::Inst()->Load(&Name[0]));


		// create the character test1 image

		pImage = (CGUIImage*) m_pBoardCharacterMenu->AddElement(GUI_TYPE_IMAGE);
		pImage->m_iPosX = 20;
		pImage->m_iPosY = 20;
		pImage->m_iWidth  = 150;
		pImage->m_iHeight = 220;

		strcpy(Name,"menuCharaterTest1");		
		pImage->SetShader(st_ShaderManager::Inst()->Load(&Name[0]));
		pImage->Deactivate();

		pImage = (CGUIImage*) m_pBoardCharacterMenu->AddElement(GUI_TYPE_IMAGE);
		pImage->m_iPosX = 20;
		pImage->m_iPosY = 20;
		pImage->m_iWidth  = 150;
		pImage->m_iHeight = 220;

		strcpy(Name,"menuCharaterTest2");		
		pImage->SetShader(st_ShaderManager::Inst()->Load(&Name[0]));
		pImage->Deactivate();



		
		

		// create the "cast_a_spell" button

		pButton = (CGUIButton*) m_pBoardCharacterMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetPotentialMessage(ENGINE_MESSAGE_CAST_A_SPELL);

		pButton->m_iPosX = 163;
		pButton->m_iPosY = 250;
		pButton->m_iWidth  = 114;
		pButton->m_iHeight = 22;

		//assert(_CrtCheckMemory());

		strcpy(Name,"ceGui/MenusAndButtons/CastASpellUp.tga");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));
		strcpy(Name,"ceGui/MenusAndButtons/CastASpellDown.tga");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));
		strcpy(Name,"ceGui/MenusAndButtons/CastASpellHover.tga");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));



		//assert(_CrtCheckMemory());

		//CGUIButton* pButton = (CGUIButton*) m_pBoardCharacterMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);
	}

	//pButton


	
	assert(_CrtCheckMemory());



	// TOP MENU

	if (bFirst)
		m_pTopMenu = new CGUIWindow;
	
	st_RendererI->GetAnchoredValues((ANCHOR_X_MIDDLE),300,146,0,0,
		m_pTopMenu->m_iPosX,iRight,iDown,m_pTopMenu->m_iPosY);

	m_pTopMenu->m_iWidth = iRight - m_pTopMenu->m_iPosX;
	m_pTopMenu->m_iHeight = iDown - m_pTopMenu->m_iPosY;


	m_pTopMenu->m_iPosY-=90;

	m_pTopMenu->m_iAnimatedStartX=m_pTopMenu->m_iPosX;
	m_pTopMenu->m_iAnimatedStartY=m_pTopMenu->m_iPosY;

	m_pTopMenu->m_iAnimatedFinalX=m_pTopMenu->m_iPosX;
	m_pTopMenu->m_iAnimatedFinalY=m_pTopMenu->m_iPosY+90;

	m_pTopMenu->m_bAnimatedPop=true;


	if (bFirst)
	{

		// TOP MENU BUTTONS
		
		strcpy(Name,"menuTopBoard");
		CShader* pShader = st_ShaderManager::Inst()->Load(&Name[0]);
		
		m_pTopMenu->SetShader(pShader);

		// init the character frame menu
		m_pTopMenu->Init(10);	

		////////


		// New Game BUTTON

		pButton = (CGUIButton*) m_pTopMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		//pButton->SetPotentialMessage(ENGINE_MESSAGE_NEW_GAME);
		pButton->SetMessagesReciever(MESSAGES_RECIEVER_GUI);
		pButton->SetPotentialMessage(GUI_MESSAGE_SHOW_NEW_GAME_MENU);

		//pButton->m_iPosX = 17;
		pButton->m_iPosX = 14;
		pButton->m_iPosY = 41;
		pButton->m_iWidth  = 116;
		pButton->m_iHeight = 40;


		strcpy(Name,"menuTopBoardNewGameUP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"menuTopBoardNewGameDOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"menuTopBoardNewGameHOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));


		// End Game BUTTON

		pButton = (CGUIButton*) m_pTopMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		//pButton->SetPotentialMessage(ENGINE_MESSAGE_NEW_GAME);
		pButton->SetMessagesReciever(MESSAGES_RECIEVER_GUI);
		pButton->SetPotentialMessage(GUI_MESSAGE_SHOW_NEW_GAME_MENU);

		//pButton->m_iPosX = 17;
		pButton->m_iPosX = 14;
		pButton->m_iPosY = 41;
		pButton->m_iWidth  = 116;
		pButton->m_iHeight = 40;


		strcpy(Name,"menuTopBoardEndGameUP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"menuTopBoardEndGameDOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"menuTopBoardEndGameHOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));

		pButton->Deactivate();


		// Save Game BUTTON

		pButton = (CGUIButton*) m_pTopMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetMessagesReciever(MESSAGES_RECIEVER_GUI);
		pButton->SetPotentialMessage(GUI_MESSAGE_SHOW_SAVE_GAME_MENU);

		pButton->m_iPosX = 14+116;
		pButton->m_iPosY = 41;
		pButton->m_iWidth  = 116;
		pButton->m_iHeight = 40;


		strcpy(Name,"menuTopBoardSaveGameUP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"menuTopBoardSaveGameDOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"menuTopBoardSaveGameHOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));
		
		// Load Game BUTTON

		pButton = (CGUIButton*) m_pTopMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetMessagesReciever(MESSAGES_RECIEVER_GUI);
		pButton->SetPotentialMessage(GUI_MESSAGE_SHOW_LOAD_GAME_MENU);

		pButton->m_iPosX = 14+116*2;
		pButton->m_iPosY = 41;
		pButton->m_iWidth  = 116;
		pButton->m_iHeight = 40;


		strcpy(Name,"menuTopBoardLoadGameUP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"menuTopBoardLoadGameDOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"menuTopBoardLoadGameHOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));


		// Options BUTTON

		pButton = (CGUIButton*) m_pTopMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetMessagesReciever(MESSAGES_RECIEVER_GUI);
		pButton->SetPotentialMessage(GUI_MESSAGE_SHOW_OPTIONS_MENU);

		pButton->m_iPosX = 14+116*3;
		pButton->m_iPosY = 41;
		pButton->m_iWidth  = 116;
		pButton->m_iHeight = 40;


		strcpy(Name,"menuTopBoardOptionsUP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"menuTopBoardOptionsDOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"menuTopBoardOptionsHOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));


		// Quit BUTTON

		pButton = (CGUIButton*) m_pTopMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetMessagesReciever(MESSAGES_RECIEVER_GUI);
		pButton->SetPotentialMessage(GUI_MESSAGE_SHOW_QUIT_MENU);

		pButton->m_iPosX = 14+116*4;
		pButton->m_iPosY = 41;
		pButton->m_iWidth  = 116;
		pButton->m_iHeight = 40;


		strcpy(Name,"menuTopBoardQuitUP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"menuTopBoardQuitDOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"menuTopBoardQuitHOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));

	}
	assert(_CrtCheckMemory());


	//YesNoQUIT

	if (bFirst)
		m_pAreYouSureMenuQUIT = new CGUIWindow;
	
	st_RendererI->GetAnchoredValues((ANCHOR_X_MIDDLE|ANCHOR_Y_MIDDLE),250,150,0,0,
		m_pAreYouSureMenuQUIT->m_iPosX,iRight,iDown,m_pAreYouSureMenuQUIT->m_iPosY);

	m_pAreYouSureMenuQUIT->m_iWidth = iRight - m_pAreYouSureMenuQUIT->m_iPosX;
	m_pAreYouSureMenuQUIT->m_iHeight = iDown - m_pAreYouSureMenuQUIT->m_iPosY;

	

	if (bFirst)
	{

		// TOP MENU BUTTONS
		
		strcpy(Name,"menuAreYouSure");
		CShader* pShader = st_ShaderManager::Inst()->Load(&Name[0]);
		
		m_pAreYouSureMenuQUIT->SetShader(pShader);

		// init the character frame menu
		m_pAreYouSureMenuQUIT->Init(10);	

		m_pAreYouSureMenuQUIT->m_bRender = false;
		m_pAreYouSureMenuQUIT->m_bActive = false;



		// Yes button

		pButton = (CGUIButton*) m_pAreYouSureMenuQUIT->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetPotentialMessage(ENGINE_MESSAGE_QUIT);

		pButton->m_iPosX = 40;
		pButton->m_iPosY = 140;
		pButton->m_iWidth  = 200;
		pButton->m_iHeight = 84;


		strcpy(Name,"ceGui/MenusAndButtons/yesUP.jpg");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"ceGui/MenusAndButtons/yesUP.jpg");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"ceGui/MenusAndButtons/yesHOVER.jpg");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));


		// No button

		pButton = (CGUIButton*) m_pAreYouSureMenuQUIT->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetMessagesReciever(MESSAGES_RECIEVER_GUI);
		pButton->SetPotentialMessage(GUI_MESSAGE_GET_TO_DEFAULT_MENUS);

		pButton->m_iPosX = 241;
		pButton->m_iPosY = 140;
		pButton->m_iWidth  = 200;
		pButton->m_iHeight = 84;


		strcpy(Name,"ceGui/MenusAndButtons/noUP.jpg");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"ceGui/MenusAndButtons/noUP.jpg");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"ceGui/MenusAndButtons/noHOVER.jpg");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));



	}

	assert(_CrtCheckMemory());


	//YesNoQUIT

	if (bFirst)
		m_pAreYouSureMenuNEWGAME = new CGUIWindow;
	
	st_RendererI->GetAnchoredValues((ANCHOR_X_MIDDLE|ANCHOR_Y_MIDDLE),250,150,0,0,
	m_pAreYouSureMenuNEWGAME->m_iPosX,iRight,iDown,m_pAreYouSureMenuNEWGAME->m_iPosY);

	m_pAreYouSureMenuNEWGAME->m_iWidth = iRight - m_pAreYouSureMenuNEWGAME->m_iPosX;
	m_pAreYouSureMenuNEWGAME->m_iHeight = iDown - m_pAreYouSureMenuNEWGAME->m_iPosY;

	

	if (bFirst)
	{

		// TOP MENU BUTTONS
		
		strcpy(Name,"menuAreYouSure");
		CShader* pShader = st_ShaderManager::Inst()->Load(&Name[0]);
		
		m_pAreYouSureMenuNEWGAME->SetShader(pShader);

		// init the character frame menu
		m_pAreYouSureMenuNEWGAME->Init(10);	

		m_pAreYouSureMenuNEWGAME->m_bRender = false;
		m_pAreYouSureMenuNEWGAME->m_bActive = false;



		// Yes button

		pButton = (CGUIButton*) m_pAreYouSureMenuNEWGAME->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetPotentialMessage(ENGINE_MESSAGE_NEW_GAME);

		pButton->m_iPosX = 40;
		pButton->m_iPosY = 140;
		pButton->m_iWidth  = 200;
		pButton->m_iHeight = 84;


		strcpy(Name,"ceGui/MenusAndButtons/yesUP.jpg");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"ceGui/MenusAndButtons/yesUP.jpg");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"ceGui/MenusAndButtons/yesHOVER.jpg");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));


		// No button

		pButton = (CGUIButton*)m_pAreYouSureMenuNEWGAME->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetMessagesReciever(MESSAGES_RECIEVER_GUI);
		pButton->SetPotentialMessage(GUI_MESSAGE_GET_TO_DEFAULT_MENUS);

		pButton->m_iPosX = 241;
		pButton->m_iPosY = 140;
		pButton->m_iWidth  = 200;
		pButton->m_iHeight = 84;


		strcpy(Name,"ceGui/MenusAndButtons/noUP.jpg");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"ceGui/MenusAndButtons/noUP.jpg");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"ceGui/MenusAndButtons/noHOVER.jpg");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));



	}


	

	/////// Save Game Menu ///////

	
	if (bFirst)
		m_pSaveGameMenu = new CGUIWindow;
	
	st_RendererI->GetAnchoredValues((ANCHOR_X_MIDDLE|ANCHOR_Y_MIDDLE),114,212,0,0,
		m_pSaveGameMenu->m_iPosX,iRight,iDown,m_pSaveGameMenu->m_iPosY);

	m_pSaveGameMenu->m_iWidth = iRight - m_pSaveGameMenu->m_iPosX;
	m_pSaveGameMenu->m_iHeight = iDown - m_pSaveGameMenu->m_iPosY;


	assert(_CrtCheckMemory());

	if (bFirst)
	{

		// TOP MENU BUTTONS
		
		strcpy(Name,"loadsaveGames");
		CShader* pShader = st_ShaderManager::Inst()->Load(&Name[0]);
		
		m_pSaveGameMenu->SetShader(pShader);

		// init the character frame menu
		m_pSaveGameMenu->Init(10);	

		m_pSaveGameMenu->m_bRender = false;
		m_pSaveGameMenu->m_bActive = false;



		// Save 00

		pButton = (CGUIButton*) m_pSaveGameMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetPotentialMessage(ENGINE_MESSAGE_SAVED_GAME_00);

		pButton->m_iPosX = 0;
		pButton->m_iPosY = 40;
		pButton->m_iWidth  = 226;
		pButton->m_iHeight = 55;


		strcpy(Name,"game00_UP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"game00_DOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"game00_HOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));

		// Save 02

		pButton = (CGUIButton*) m_pSaveGameMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetPotentialMessage(ENGINE_MESSAGE_SAVED_GAME_02);

		pButton->m_iPosX = 0;
		pButton->m_iPosY = 40+55*2;
		pButton->m_iWidth  = 226;
		pButton->m_iHeight = 55;


		strcpy(Name,"game02_UP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"game02_DOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"game02_HOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));

		// Save 04

		pButton = (CGUIButton*) m_pSaveGameMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetPotentialMessage(ENGINE_MESSAGE_SAVED_GAME_04);

		pButton->m_iPosX = 0;
		pButton->m_iPosY = 40+55*4;
		pButton->m_iWidth  = 226;
		pButton->m_iHeight = 55;


		strcpy(Name,"game04_UP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"game04_DOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"game04_HOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));



		// Save 01

		pButton = (CGUIButton*) m_pSaveGameMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetPotentialMessage(ENGINE_MESSAGE_SAVED_GAME_01);

		pButton->m_iPosX = 0;
		pButton->m_iPosY = 40+55*1;
		pButton->m_iWidth  = 226;
		pButton->m_iHeight = 55;


		strcpy(Name,"game01_UP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"game01_DOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"game01_HOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));


		// Save 03

		pButton = (CGUIButton*) m_pSaveGameMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetPotentialMessage(ENGINE_MESSAGE_SAVED_GAME_03);

		pButton->m_iPosX = 0;
		pButton->m_iPosY = 40+55*3;
		pButton->m_iWidth  = 226;
		pButton->m_iHeight = 55;


		strcpy(Name,"game03_UP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"game03_DOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"game03_HOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));

		// Save 05

		pButton = (CGUIButton*) m_pSaveGameMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetPotentialMessage(ENGINE_MESSAGE_SAVED_GAME_05);

		pButton->m_iPosX = 0;
		pButton->m_iPosY = 40+55*5;
		pButton->m_iWidth  = 226;
		pButton->m_iHeight = 55;


		strcpy(Name,"game05_UP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"game05_DOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"game05_HOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));







		// Cancel button

		pButton = (CGUIButton*)m_pSaveGameMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetMessagesReciever(MESSAGES_RECIEVER_GUI);
		pButton->SetPotentialMessage(GUI_MESSAGE_GET_TO_DEFAULT_MENUS);

		pButton->m_iPosX = -30;
		pButton->m_iPosY = 490;
		pButton->m_iWidth  = 290;
		pButton->m_iHeight = 71;


		strcpy(Name,"cancelUP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"cancelDOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"cancelHOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));



	}

	assert(_CrtCheckMemory());

	/////// Load Game Menu ///////

	
	if (bFirst)
		m_pLoadGameMenu = new CGUIWindow;
	
	st_RendererI->GetAnchoredValues((ANCHOR_X_MIDDLE|ANCHOR_Y_MIDDLE),114,212,0,0,
	m_pLoadGameMenu->m_iPosX,iRight,iDown,m_pLoadGameMenu->m_iPosY);

	m_pLoadGameMenu->m_iWidth = iRight - m_pLoadGameMenu->m_iPosX;
	m_pLoadGameMenu->m_iHeight = iDown - m_pLoadGameMenu->m_iPosY;


	if (bFirst)
	{

		// TOP MENU BUTTONS
		
		strcpy(Name,"loadsaveGames");
		CShader* pShader = st_ShaderManager::Inst()->Load(&Name[0]);
		
		m_pLoadGameMenu->SetShader(pShader);

		// init the character frame menu
		m_pLoadGameMenu->Init(10);	

		m_pLoadGameMenu->m_bRender = false;
		m_pLoadGameMenu->m_bActive = false;



		// load 00

		pButton = (CGUIButton*) m_pLoadGameMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetPotentialMessage(ENGINE_MESSAGE_LOAD_GAME_00);

		pButton->m_iPosX = 0;
		pButton->m_iPosY = 40;
		pButton->m_iWidth  = 226;
		pButton->m_iHeight = 55;


		strcpy(Name,"game00_UP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"game00_DOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"game00_HOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));

		// load 02

		pButton = (CGUIButton*) m_pLoadGameMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetPotentialMessage(ENGINE_MESSAGE_LOAD_GAME_02);

		pButton->m_iPosX = 0;
		pButton->m_iPosY = 40+55*2;
		pButton->m_iWidth  = 226;
		pButton->m_iHeight = 55;


		strcpy(Name,"game02_UP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"game02_DOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"game02_HOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));
		// load 04

		pButton = (CGUIButton*) m_pLoadGameMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetPotentialMessage(ENGINE_MESSAGE_LOAD_GAME_04);

		pButton->m_iPosX = 0;
		pButton->m_iPosY = 40+55*4;
		pButton->m_iWidth  = 226;
		pButton->m_iHeight = 55;


		strcpy(Name,"game04_UP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"game04_DOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"game04_HOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));



		// lave 01

		pButton = (CGUIButton*) m_pLoadGameMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetPotentialMessage(ENGINE_MESSAGE_LOAD_GAME_01);

		pButton->m_iPosX = 0;
		pButton->m_iPosY = 40+55;
		pButton->m_iWidth  = 226;
		pButton->m_iHeight = 55;


		strcpy(Name,"game01_UP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"game01_DOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"game01_HOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));


		// load 03

		pButton = (CGUIButton*) m_pLoadGameMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetPotentialMessage(ENGINE_MESSAGE_LOAD_GAME_03);

		pButton->m_iPosX = 0;
		pButton->m_iPosY = 40+55*3;
		pButton->m_iWidth  = 226;
		pButton->m_iHeight = 55;


		strcpy(Name,"game03_UP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"game03_DOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"game03_HOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));

		// load 05

		pButton = (CGUIButton*) m_pLoadGameMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetPotentialMessage(ENGINE_MESSAGE_LOAD_GAME_05);

		pButton->m_iPosX = 0;
		pButton->m_iPosY = 40+55*5;
		pButton->m_iWidth  = 226;
		pButton->m_iHeight = 55;


		strcpy(Name,"game05_UP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"game05_DOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"game05_HOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));







		// Cancel button

		pButton = (CGUIButton*)m_pLoadGameMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetMessagesReciever(MESSAGES_RECIEVER_GUI);
		pButton->SetPotentialMessage(GUI_MESSAGE_GET_TO_DEFAULT_MENUS);

		pButton->m_iPosX = -30;
		pButton->m_iPosY = 490;
		pButton->m_iWidth  = 290;
		pButton->m_iHeight = 71;


		strcpy(Name,"cancelUP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"cancelDOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"cancelHOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));



	}

	assert(_CrtCheckMemory());

	// chat menu

	if (bFirst)
		m_pChatMenu = new CGUIWindow;

	st_RendererI->GetAnchoredValues(0,610,50,70,230,
	m_pChatMenu->m_iPosX,iRight,iDown,m_pChatMenu->m_iPosY);

	m_pChatMenu->m_iWidth = iRight - m_pChatMenu->m_iPosX;
	m_pChatMenu->m_iHeight = iDown - m_pChatMenu->m_iPosY;

	
	if (bFirst)
	{
		// menu itself

		// TOP MENU BUTTONS
		
		//strcpy(Name,"optionsMenu");
		strcpy(Name,"menuChat");		
		CShader* pShader = st_ShaderManager::Inst()->Load(&Name[0]);
		
		m_pChatMenu->SetShader(pShader);

		// init the character frame menu
		m_pChatMenu->Init(4);	

		m_pChatMenu->m_bRender = true;
		m_pChatMenu->m_bActive = true; // ???



		/// chat edit box

		CGUIEditBox* pEditBox = (CGUIEditBox*) m_pChatMenu->AddElement(GUI_TYPE_TEXT_EDIT_BOX);
		//CGUIImage* pImage = (CGUIImage*) m_pChooseIP->AddElement(GUI_TYPE_IMAGE);
		
		
		pEditBox->m_iPosX = 10;
		pEditBox->m_iPosY = 30;
		pEditBox->m_iWidth  = 400;
		pEditBox->m_iHeight = 40;

		
		pEditBox->updateMe = &chat[0];

		/*char Temp[30];
		sprintf(&Temp[0],"%s_",&defaultIP[0]);
		strcpy(&pEditBox->caText[0],&Temp[0]);
		pEditBox->m_iPositionInText = strlen(&Temp[0])-1;*/

		pEditBox->m_iMessagesReciever = MESSAGES_RECIEVER_ENGINE;
		pEditBox->m_dwPotentialMessage = ENGINE_MESSAGE_SEND_CHAT_MESSAGE;
		
		

		pEditBox->m_iEditBoxType = GUI_EDIT_BOX_TYPE_CHAT;
		
		pEditBox->m_bWritable = true;
		//pEditBox->m_iBoxLength = 50;
		pEditBox->m_iBoxLength = 40;
		pEditBox->m_iMaximumAcceptableChars = 300;

	}
	


	/////// choose ip menu ////////

	if (bFirst)
		m_pChooseIP = new CGUIWindow;

	st_RendererI->GetAnchoredValues((ANCHOR_X_MIDDLE|ANCHOR_Y_MIDDLE),250,212,0,0,
	m_pChooseIP->m_iPosX,iRight,iDown,m_pChooseIP->m_iPosY);

	m_pChooseIP->m_iWidth = iRight - m_pChooseIP->m_iPosX;
	m_pChooseIP->m_iHeight = iDown - m_pChooseIP->m_iPosY;

	assert(_CrtCheckMemory());
	
	if (bFirst)
	{
		// menu itself

		// TOP MENU BUTTONS
		
		//strcpy(Name,"optionsMenu");
		//strcpy(Name,"menuCharaterFrame");		
		strcpy(Name,"menuChat");				
		//strcpy(Name,"textures/sfx/flame1");				
		CShader* pShader = st_ShaderManager::Inst()->Load(&Name[0]);
		
		m_pChooseIP->SetShader(pShader);

		// init the character frame menu
		m_pChooseIP->Init(5);	

		m_pChooseIP->m_bRender = false;
		m_pChooseIP->m_bActive = false;


		assert(_CrtCheckMemory());

		// choose oponent IP image

		// 1st makred / unmarked
		CGUIImage*pImage = (CGUIImage*) m_pChooseIP->AddElement(GUI_TYPE_IMAGE);
		
		//pImage->m_iPosX = 145;
		pImage->m_iPosX = 100;
		pImage->m_iPosY = 40+80;
		pImage->m_iWidth  = 300;
		pImage->m_iHeight = 50;
		
		strcpy(Name,"pleaseTypeServerIP");		
		pImage->SetShader(st_ShaderManager::Inst()->Load(&Name[0]));
		pImage->m_bActive=true;

		assert(_CrtCheckMemory());

		// Network button - join a game

		pButton = (CGUIButton*) m_pChooseIP->AddElement(GUI_TYPE_BUTTON_SIMPLE);	
		
		pButton->SetPotentialMessage(ENGINE_MESSAGE_START_NETWORK_CLIENT);
		

		pButton->m_iPosX = 330;
		pButton->m_iPosY = 220;
		pButton->m_iWidth  = 109;
		pButton->m_iHeight = 50;


		strcpy(Name,"ConnectUP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"ConnectUP");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));	

		strcpy(Name,"ConnectHOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));		


		assert(_CrtCheckMemory());

		// IP edit box

		// 1st makred / unmarked
		
		CGUIEditBox* pEditBox = (CGUIEditBox*) m_pChooseIP->AddElement(GUI_TYPE_TEXT_EDIT_BOX);
		//CGUIImage* pImage = (CGUIImage*) m_pChooseIP->AddElement(GUI_TYPE_IMAGE);
		
		
		pEditBox->m_iPosX = 90;
		pEditBox->m_iPosY = 250;
		pEditBox->m_iWidth  = 275;
		pEditBox->m_iHeight = 72;

		/*pEditBox->updateMe = &defaultIP[0];
		char Temp[30];
		sprintf(&Temp[0],"%s_",&defaultIP[0]);
		strcpy(&pEditBox->caText[0],&Temp[0]);
		pEditBox->m_iPositionInText = strlen(&Temp[0])-1;*/

		pEditBox->m_iMessagesReciever = MESSAGES_RECIEVER_ENGINE;
		pEditBox->m_dwPotentialMessage = ENGINE_MESSAGE_START_NETWORK_CLIENT;

		pEditBox->m_iEditBoxType = GUI_EDIT_BOX_TYPE_IP_ADRESS;
		
		pEditBox->m_bWritable = true;
		pEditBox->m_iBoxLength = 16;
		pEditBox->m_iMaximumAcceptableChars = 14;

		pEditBox->m_bIgnoreWriteMode = true;
		
		assert(_CrtCheckMemory());

		/////////////////// editbox image

		pImage = (CGUIImage*) m_pChooseIP->AddElement(GUI_TYPE_IMAGE);
		
		//pImage->m_iPosX = 145;
		pImage->m_iPosX = 90-10;
		pImage->m_iPosY = 250-25;
		pImage->m_iWidth  = 245;
		pImage->m_iHeight = 40;
		
		strcpy(Name,"editBox");		
		pImage->SetShader(st_ShaderManager::Inst()->Load(&Name[0]));
		pImage->m_bActive=true;



		assert(_CrtCheckMemory());
		
		
		// Cancel
		
		pButton = (CGUIButton*)m_pChooseIP->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetMessagesReciever(MESSAGES_RECIEVER_GUI);
		pButton->SetPotentialMessage(GUI_MESSAGE_SHOW_OPTIONS_MENU);

		pButton->m_iPosX = 160;
		pButton->m_iPosY = 490;
		pButton->m_iWidth  = 290;
		pButton->m_iHeight = 71;


		strcpy(Name,"cancelUP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"cancelDOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"cancelHOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));

		assert(_CrtCheckMemory()); // this

	}


	/////// Options Menu ///////

	
	if (bFirst)
		m_pOptionsMenu = new CGUIWindow;
	
	st_RendererI->GetAnchoredValues((ANCHOR_X_MIDDLE|ANCHOR_Y_MIDDLE),250,212,0,0,
	m_pOptionsMenu->m_iPosX,iRight,iDown,m_pOptionsMenu->m_iPosY);

	m_pOptionsMenu->m_iWidth = iRight - m_pOptionsMenu->m_iPosX;
	m_pOptionsMenu->m_iHeight = iDown - m_pOptionsMenu->m_iPosY;


	CGUI2StatesButton* p2SidedButton;
	
	assert(_CrtCheckMemory());

	if (bFirst)
	{

		// TOP MENU BUTTONS
		
		strcpy(Name,"optionsMenu");
		CShader* pShader = st_ShaderManager::Inst()->Load(&Name[0]);
		
		m_pOptionsMenu->SetShader(pShader);

		// init the character frame menu
		m_pOptionsMenu->Init(10);	

		m_pOptionsMenu->m_bRender = false;
		m_pOptionsMenu->m_bActive = false;


		

		// 2 sides button - set past army human

		p2SidedButton = (CGUI2StatesButton*) m_pOptionsMenu->AddElement(GUI_TYPE_BUTTON_2_STATES);

		p2SidedButton->SetMessagesReciever(MESSAGES_RECIEVER_GUI);
		p2SidedButton->SetPotentialMessage(GUI_MESSAGE_SET_PAST_ARMY_HUMAN);

		p2SidedButton->m_iPosX = 190;
		p2SidedButton->m_iPosY = 54;
		p2SidedButton->m_iWidth  = 137;
		p2SidedButton->m_iHeight = 71;


		strcpy(Name,"humanOFF");				
		p2SidedButton->SetShaderState1(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"humanON");		
		p2SidedButton->SetShaderState2(st_ShaderManager::Inst()->Load(&Name[0]));	


		// 2 sides button - set past army pc

		p2SidedButton = (CGUI2StatesButton*) m_pOptionsMenu->AddElement(GUI_TYPE_BUTTON_2_STATES);

		p2SidedButton->SetMessagesReciever(MESSAGES_RECIEVER_GUI);
		p2SidedButton->SetPotentialMessage(GUI_MESSAGE_SET_PAST_ARMY_PC);

		p2SidedButton->m_iPosX = 320;
		p2SidedButton->m_iPosY = 54;
		p2SidedButton->m_iWidth  = 184;
		p2SidedButton->m_iHeight = 72;


		strcpy(Name,"pcOFF");		
		p2SidedButton->SetShaderState1(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"pcON");		
		p2SidedButton->SetShaderState2(st_ShaderManager::Inst()->Load(&Name[0]));	



		// 2 sides button - set future army human

		p2SidedButton = (CGUI2StatesButton*) m_pOptionsMenu->AddElement(GUI_TYPE_BUTTON_2_STATES);

		p2SidedButton->SetMessagesReciever(MESSAGES_RECIEVER_GUI);
		p2SidedButton->SetPotentialMessage(GUI_MESSAGE_SET_FUTURE_ARMY_HUMAN);

		p2SidedButton->m_iPosX = 190;
		p2SidedButton->m_iPosY = 124;
		p2SidedButton->m_iWidth  = 137;
		p2SidedButton->m_iHeight = 71;


		strcpy(Name,"humanOFF");				
		p2SidedButton->SetShaderState1(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"humanON");		
		p2SidedButton->SetShaderState2(st_ShaderManager::Inst()->Load(&Name[0]));	


		// 2 sides button - set future army pc

		p2SidedButton = (CGUI2StatesButton*) m_pOptionsMenu->AddElement(GUI_TYPE_BUTTON_2_STATES);

		p2SidedButton->SetMessagesReciever(MESSAGES_RECIEVER_GUI);
		p2SidedButton->SetPotentialMessage(GUI_MESSAGE_SET_FUTURE_ARMY_PC);

		p2SidedButton->m_iPosX = 320;
		p2SidedButton->m_iPosY = 124;
		p2SidedButton->m_iWidth  = 184;
		p2SidedButton->m_iHeight = 72;


		strcpy(Name,"pcOFF");		
		p2SidedButton->SetShaderState1(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"pcON");		
		p2SidedButton->SetShaderState2(st_ShaderManager::Inst()->Load(&Name[0]));	



		// Network button - host a game

		pButton = (CGUIButton*) m_pOptionsMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);
		pButton->SetPotentialMessage(ENGINE_MESSAGE_START_NETWORK_SERVER);

		pButton->m_iPosX = 110-150;
		pButton->m_iPosY = 200;
		pButton->m_iWidth  = 278;
		pButton->m_iHeight = 72;


		strcpy(Name,"HostGameON");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"HostGameON");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));	

		strcpy(Name,"HostGameOFF");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));	


		// Network button - join a game

		pButton = (CGUIButton*) m_pOptionsMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetMessagesReciever(MESSAGES_RECIEVER_GUI);
		pButton->SetPotentialMessage(GUI_MESSAGE_SHOW_CHOOSE_IP_MENU);
		

		pButton->m_iPosX = 110-150;
		pButton->m_iPosY = 280;
		pButton->m_iWidth  = 275;
		pButton->m_iHeight = 72;


		strcpy(Name,"JoinGameON");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"JoinGameON");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));	

		strcpy(Name,"JoinGameOFF");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));	

				

		// Done button

		// remember to change this into "close"

		pButton = (CGUIButton*)m_pOptionsMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);

		pButton->SetMessagesReciever(MESSAGES_RECIEVER_GUI);
		pButton->SetPotentialMessage(GUI_MESSAGE_GET_TO_DEFAULT_MENUS);

		pButton->m_iPosX = 160;
		pButton->m_iPosY = 490;
		pButton->m_iWidth  = 150;
		pButton->m_iHeight = 89;


		strcpy(Name,"doneUP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"doneDOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"doneHOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));


		// Cancel button

		pButton = (CGUIButton*)m_pOptionsMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);
		pButton->SetPotentialMessage(ENGINE_MESSAGE_STOP_CONNECTION);
		pButton->m_iPosX = 395-150;
		pButton->m_iPosY = 200;
		pButton->m_iWidth  = 290;
		pButton->m_iHeight = 71;

		strcpy(Name,"cancelUP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"cancelDOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"cancelHOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));


		// Cancel button

		pButton = (CGUIButton*)m_pOptionsMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);
		pButton->SetPotentialMessage(ENGINE_MESSAGE_STOP_CONNECTION);
		pButton->m_iPosX = 395-150;
		pButton->m_iPosY = 200+80;
		pButton->m_iWidth  = 290;
		pButton->m_iHeight = 71;

		strcpy(Name,"cancelUP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"cancelDOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"cancelHOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));


	}


	assert(_CrtCheckMemory());


	//// pre battle attacker choose advantage menu
	
		
	if (bFirst)
		m_pAdvantageMenu = new CGUIWindow;
	
	st_RendererI->GetAnchoredValues(0,228,424,0,0,
		m_pAdvantageMenu->m_iPosX,iRight,iDown,m_pAdvantageMenu->m_iPosY);

	m_pAdvantageMenu->m_iWidth = 228;
	m_pAdvantageMenu->m_iHeight = 424;

	
	m_pAdvantageMenu->m_iPosX-=145;
	m_pAdvantageMenu->m_iPosY+=20;

	m_pAdvantageMenu->m_iAnimatedStartX=m_pAdvantageMenu->m_iPosX;
	m_pAdvantageMenu->m_iAnimatedStartY=m_pAdvantageMenu->m_iPosY;

	m_pAdvantageMenu->m_iAnimatedFinalX=m_pAdvantageMenu->m_iPosX+145;
	m_pAdvantageMenu->m_iAnimatedFinalY=m_pAdvantageMenu->m_iPosY;

	m_pAdvantageMenu->m_bAnimatedPop=true;

	if (bFirst)
	{

		// TOP MENU BUTTONS
		
		strcpy(Name,"loadsaveGames");
		CShader* pShader = st_ShaderManager::Inst()->Load(&Name[0]);
		
		m_pAdvantageMenu->SetShader(pShader);

		// init the character frame menu
		m_pAdvantageMenu->Init(10);	

		m_pAdvantageMenu->m_bRender = true;
		m_pAdvantageMenu->m_bActive = true;


		// weapon

		pButton = (CGUIButton*) m_pAdvantageMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);
		//pButton->SetPotentialMessage(ENGINE_MESSAGE_ADVANTAGE_WEAPON);
		pButton->SetMessagesReciever(MESSAGES_RECIEVER_GUI);
		pButton->SetPotentialMessage(GUI_MESSAGE_SET_ADVANTAGE_WEAPON);
		

		pButton->m_iPosX = 0;
		pButton->m_iPosY = 40+80;
		pButton->m_iWidth  = 126;
		pButton->m_iHeight = 55;


		strcpy(Name,"preBattleMenuWeaponUP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"preBattleMenuWeaponDOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"preBattleMenuWeaponHOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));


		// extra life

		pButton = (CGUIButton*) m_pAdvantageMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);
		//pButton->SetPotentialMessage(ENGINE_MESSAGE_ADVANTAGE_EXTRALIFE);
		pButton->SetMessagesReciever(MESSAGES_RECIEVER_GUI);
		pButton->SetPotentialMessage(GUI_MESSAGE_SET_ADVANTAGE_EXTRALIFE);

		pButton->m_iPosX = 0;
		pButton->m_iPosY = 100+80;
		pButton->m_iWidth  = 138;
		pButton->m_iHeight = 55;


		strcpy(Name,"preBattleMenuExtraLifeUP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"preBattleMenuExtraLifeDOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"preBattleMenuExtraLifeHOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));


		//infinite ammo

		pButton = (CGUIButton*) m_pAdvantageMenu->AddElement(GUI_TYPE_BUTTON_SIMPLE);
		//pButton->SetPotentialMessage(ENGINE_MESSAGE_ADVANTAGE_EXTRALIFE);
		pButton->SetMessagesReciever(MESSAGES_RECIEVER_GUI);
		pButton->SetPotentialMessage(GUI_MESSAGE_SET_ADVANTAGE_AMMO);

		pButton->m_iPosX = 0;
		pButton->m_iPosY = 160+80;
		pButton->m_iWidth  = 138;
		pButton->m_iHeight = 55;


		strcpy(Name,"preBattleMenuAmmoUP");		
		pButton->SetShaderUp(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"preBattleMenuAmmoDOWN");		
		pButton->SetShaderDown(st_ShaderManager::Inst()->Load(&Name[0]));

		strcpy(Name,"preBattleMenuAmmoHOVER");		
		pButton->SetShaderHover(st_ShaderManager::Inst()->Load(&Name[0]));



		

		// 1st makred / unmarked
		CGUIImage*pImage = (CGUIImage*) m_pAdvantageMenu->AddElement(GUI_TYPE_IMAGE);
		
		pImage->m_iPosX = 145;
		pImage->m_iPosY = 40+80;
		pImage->m_iWidth  = 69;
		pImage->m_iHeight = 55;
		
		strcpy(Name,"marked");		
		pImage->SetShader(st_ShaderManager::Inst()->Load(&Name[0]));
		pImage->m_bActive=true;

		pImage = (CGUIImage*) m_pAdvantageMenu->AddElement(GUI_TYPE_IMAGE);
		
		pImage->m_iPosX = 145;
		pImage->m_iPosY = 40+80;
		pImage->m_iWidth  = 69;
		pImage->m_iHeight = 55;
		
		strcpy(Name,"unmarked");		
		pImage->SetShader(st_ShaderManager::Inst()->Load(&Name[0]));
		pImage->m_bActive=false;


		// 2nd makred / unmarked
		pImage = (CGUIImage*) m_pAdvantageMenu->AddElement(GUI_TYPE_IMAGE);

		pImage->m_iPosX = 145;
		pImage->m_iPosY = 100+80;
		pImage->m_iWidth  = 69;
		pImage->m_iHeight = 55;
		
		strcpy(Name,"marked");		
		pImage->SetShader(st_ShaderManager::Inst()->Load(&Name[0]));
		pImage->m_bActive=false;

		pImage = (CGUIImage*) m_pAdvantageMenu->AddElement(GUI_TYPE_IMAGE);

		pImage->m_iPosX = 145;
		pImage->m_iPosY = 100+80;
		pImage->m_iWidth  = 69;
		pImage->m_iHeight = 55;
		
		strcpy(Name,"unmarked");		
		pImage->SetShader(st_ShaderManager::Inst()->Load(&Name[0]));
		pImage->m_bActive=true;




		// 3nd makred / unmarked
		pImage = (CGUIImage*) m_pAdvantageMenu->AddElement(GUI_TYPE_IMAGE);

		pImage->m_iPosX = 145;
		pImage->m_iPosY = 160+80;
		pImage->m_iWidth  = 69;
		pImage->m_iHeight = 55;
		
		strcpy(Name,"marked");		
		pImage->SetShader(st_ShaderManager::Inst()->Load(&Name[0]));
		pImage->m_bActive=false;

		pImage = (CGUIImage*) m_pAdvantageMenu->AddElement(GUI_TYPE_IMAGE);

		pImage->m_iPosX = 145;
		pImage->m_iPosY = 160+80;
		pImage->m_iWidth  = 69;
		pImage->m_iHeight = 55;
		
		strcpy(Name,"unmarked");		
		pImage->SetShader(st_ShaderManager::Inst()->Load(&Name[0]));
		pImage->m_bActive=true;

	}

	assert(_CrtCheckMemory());

}

void CGUIManager::Render(void)
{
	m_pBoardCharacterMenu->Render();
	m_pTopMenu->Render();
	m_pAreYouSureMenuQUIT->Render();
	m_pAreYouSureMenuNEWGAME->Render();
	m_pSaveGameMenu->Render();
	m_pLoadGameMenu->Render();
	m_pOptionsMenu->Render();
	m_pAdvantageMenu->Render();
	m_pChooseIP->Render();	

	CGUIEditBox* pEditBox = (CGUIEditBox*) m_pChatMenu->GetElementNum(0);
	if (pEditBox->m_bWriteMode)	
		m_pChatMenu->Render();
}

void CGUIManager::RenderText(void)
{
	m_pChooseIP->RenderText();

	CGUIEditBox* pEditBox = (CGUIEditBox*) m_pChatMenu->GetElementNum(0);
	if (pEditBox->caText[1]!=NULL)
		m_pChatMenu->RenderText();
}

bool CGUIManager::UpdateMouseAndKeyboard(void)
{
	st_Input::Inst()->GetAbsoluteX();
    st_Input::Inst()->GetAbsoluteY();

	bool bReturn = false;

	if (m_pBoardCharacterMenu->m_bActive)
	{
		if (m_pBoardCharacterMenu->IsMouseOn())
			bReturn=true;
		m_pBoardCharacterMenu->OnMouse();
	}


	if (m_pTopMenu->m_bActive)
	{
		if (m_pTopMenu->IsMouseOn())
			bReturn=true;
		m_pTopMenu->OnMouse();
	}

	if (m_pChatMenu->m_bActive)
	{
		m_pChatMenu->OnKeyboard();
		if (m_pChatMenu->IsMouseOn())
			bReturn=true;
		m_pChatMenu->OnMouse();

	}

	if (m_pAreYouSureMenuQUIT->m_bActive)
	{
		if (m_pAreYouSureMenuQUIT->IsMouseOn())
			bReturn=true;
		m_pAreYouSureMenuQUIT->OnMouse();
	}


	if (m_pAreYouSureMenuNEWGAME->m_bActive)
	{
		if (m_pAreYouSureMenuNEWGAME->IsMouseOn())
			bReturn=true;
		m_pAreYouSureMenuNEWGAME->OnMouse();
	}

	if (m_pSaveGameMenu->m_bActive)
	{
		if (m_pSaveGameMenu->IsMouseOn())
			bReturn=true;
		m_pSaveGameMenu->OnMouse();
	}

	if (m_pChooseIP->m_bActive)
	{
		m_pChooseIP->OnKeyboard();
		if (m_pChooseIP->IsMouseOn())
			bReturn=true;
		m_pChooseIP->OnMouse();
	}

	

	if (m_pLoadGameMenu->m_bActive)
	{
		if (m_pLoadGameMenu->IsMouseOn())
			bReturn=true;
		m_pLoadGameMenu->OnMouse();
	}

	if (m_pAdvantageMenu->m_bActive)
	{
		if (m_pAdvantageMenu->IsMouseOn())
			bReturn=true;
		m_pAdvantageMenu->OnMouse();
	}

	

	if (m_pOptionsMenu->m_bActive)
	{
		if (m_pOptionsMenu->IsMouseOn())
			bReturn=true;
		m_pOptionsMenu->OnMouse();
	}

	
	



	return bReturn;
}

void CGUIManager::HandleGUIMessages(void)
{

	static char Name[200];

	CGUIButton * pButton;

	///// change top menu left button according to context

	// perhaps fake load both options, so they wouldn't get loaded per frame???

	if (g_pCheckersBoard->m_iBoardGameState==GAME_BOARD_STATE_MENU)
	{
		((CGUIButton*)m_pTopMenu->GetElementNum(0))->Activate();
		((CGUIButton*)m_pTopMenu->GetElementNum(1))->Deactivate();		
	}
	else
	{
		if (g_Communication.GetAlreadyInited())
		{
			((CGUIButton*)m_pTopMenu->GetElementNum(0))->Activate();
			((CGUIButton*)m_pTopMenu->GetElementNum(1))->Deactivate();		
		}
		else
		{
			((CGUIButton*)m_pTopMenu->GetElementNum(1))->Activate();
			((CGUIButton*)m_pTopMenu->GetElementNum(0))->Deactivate();		
		}
	}



	static bool bLastTimeEscape;
	
	
	if (m_dwGUIMessages & GUI_MESSAGE_ESCAPE)
	{
		if (IsDefaultMenusAndNotQuitMenu())
		{		
			if (!bLastTimeEscape)
					m_dwGUIMessages|=GUI_MESSAGE_SHOW_QUIT_MENU;		
		}
		else
			m_dwGUIMessages|=GUI_MESSAGE_GET_TO_DEFAULT_MENUS;

		bLastTimeEscape=true;
	}
	else
		bLastTimeEscape=false;




	/////



	if (!g_Communication.GetAlreadyInited()) // doesn't make sense while networking...
		if (m_dwGUIMessages & GUI_MESSAGE_SHOW_SAVE_GAME_MENU)
		{

			if (g_pCheckersBoard->AreAllBoardAnimationsDone())
			{
				ResetBoardGameMenus();
				DeActivateAllMenus();

				m_pSaveGameMenu->m_bRender = true;
				m_pSaveGameMenu->m_bActive = true;		
			}
		}

	if (!g_Communication.GetAlreadyInited()) // doesn't make sense while networking...
		if (m_dwGUIMessages & GUI_MESSAGE_SHOW_LOAD_GAME_MENU)
		{
			
			if (g_pCheckersBoard->AreAllBoardAnimationsDone())
			{
				ResetBoardGameMenus();
				DeActivateAllMenus();

				m_pLoadGameMenu->m_bRender = true;
				m_pLoadGameMenu->m_bActive = true;		
			}
			
		}


		if (m_dwGUIMessages & GUI_MESSAGE_SHOW_ADVANTAGE_MENU)
		{
			//if (g_pCheckersBoard->AreAllBoardAnimationsDone())
			{
				ResetBoardGameMenus();
				DeActivateAllMenus();

				m_pAdvantageMenu->m_bRender = true;
				m_pAdvantageMenu->m_bActive = true;		
			}
			
		}

		
		
	
		if (m_dwGUIMessages & GUI_MESSAGE_SHOW_QUIT_MENU)
		{

			ResetBoardGameMenus();
			DeActivateAllMenus();

			m_pAreYouSureMenuQUIT->m_bRender = true;
			m_pAreYouSureMenuQUIT->m_bActive = true;		
		}


		if (g_pCheckersBoard->AreAllBoardAnimationsDone())
		if (m_dwGUIMessages & GUI_MESSAGE_SHOW_NEW_GAME_MENU)
		{			
			if (g_pCheckersBoard->m_iBoardGameState==GAME_BOARD_STATE_MENU)
			{
				g_pCheckersBoard->NewGame();
				g_pGUIManager->ResetBoardGameMenus();		
				m_dwGUIMessages|=GUI_MESSAGE_GET_TO_DEFAULT_MENUS; // sends a message to the next if...
				m_pOptionsMenu->m_bActive=true;

				if (g_pCheckersBoard->m_bPlayingWinSong)
				{
					g_pCheckersBoard->StopBackgroundMusic();
					g_SoundManager.PlayStream(g_pCheckersBoard->m_ppBackgroundStreams[CB_SOUND_STREAM_INTRO]);
				}
			}
			else
			{
				//g_pCheckersBoard->m_iBoardGameState=GAME_BOARD_STATE_MENU;
				ResetBoardGameMenus();
				DeActivateAllMenus();

				m_pAreYouSureMenuNEWGAME->m_bRender = true;
				m_pAreYouSureMenuNEWGAME->m_bActive = true;		
			}

		}


		if (m_dwGUIMessages & GUI_MESSAGE_SHOW_CHOOSE_IP_MENU)
		{
			// disable writing text to console
			CGUIEditBox* pEditBox = (CGUIEditBox*) m_pChatMenu->GetElementNum(0);
			pEditBox->m_bWritable=false;
		

			pEditBox = (CGUIEditBox*) m_pChooseIP->GetElementNum(2);
			pEditBox->updateMe = &defaultIP[0];
			char Temp[30];
			if (defaultIP[strlen(defaultIP)-1]=='_')
			{
				// do nothing
			}
			else
			if (defaultIP[1]!=NULL)
			{
				sprintf(&Temp[0],"%s_",&defaultIP[0]);
				strcpy(&pEditBox->caText[0],&Temp[0]);
				pEditBox->m_iPositionInText = strlen(&Temp[0])-1;
			}
		


			g_pGUIManager->ResetBoardGameMenus();		
			//m_dwGUIMessages|=GUI_MESSAGE_GET_TO_DEFAULT_MENUS; // sends a message to the next if...
			m_pChooseIP->m_bActive=true;
			m_pChooseIP->m_bRender=true;
			
		}

		

		if (m_dwGUIMessages & GUI_MESSAGE_GET_TO_DEFAULT_MENUS)
		{
			//enable writing text to the "console"
			CGUIEditBox* pEditBox = (CGUIEditBox*) m_pChatMenu->GetElementNum(0);
			pEditBox->m_bWritable=true;

			//if (g_pCheckersBoard->m_iPlayerTypePAST!=PLAYER_TYPE_NETWORK && g_pCheckersBoard->m_iPlayerTypeFUTURE!=PLAYER_TYPE_NETWORK)
			if (m_pOptionsMenu->m_bActive) // if message was sent by the options dialog
			if (g_pCheckersBoard->m_bPlayersChanged)
			{
				//m_pOptionsMenu->m_bActive=false;
				// use the values the user set

				if (((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(0))->GetState()==TWO_SIDES_BUTTON_STATE_1)
				{
					//if (!m_bChoseNetwork)
					if (!g_Communication.GetAlreadyInited())
					{
						g_pCheckersBoard->m_iPlayerTypePAST = PLAYER_TYPE_PC;
						g_pCheckersBoard->m_iPlayerTypeFUTURE = PLAYER_TYPE_HUMAN;
					}
					/*else
					{
						g_pCheckersBoard->m_iPlayerTypePAST = PLAYER_TYPE_NETWORK;
						g_pCheckersBoard->m_iPlayerTypeFUTURE = PLAYER_TYPE_HUMAN;
					}*/
					
				}
				else
				{
					//if (!m_bChoseNetwork)
					if (!g_Communication.GetAlreadyInited())
					{
						g_pCheckersBoard->m_iPlayerTypePAST = PLAYER_TYPE_HUMAN;
						g_pCheckersBoard->m_iPlayerTypeFUTURE = PLAYER_TYPE_PC;
					}
					/*else
					{
						g_pCheckersBoard->m_iPlayerTypePAST = PLAYER_TYPE_HUMAN;
						g_pCheckersBoard->m_iPlayerTypeFUTURE = PLAYER_TYPE_NETWORK;
					}*/

				}				
			}

			ResetBoardGameMenus();

		}

		


		if (m_dwGUIMessages & GUI_MESSAGE_SHOW_OPTIONS_MENU)
		{			
			g_pCheckersBoard->m_bPlayersChanged=true;

			if (g_pCheckersBoard->m_iBoardGameState==GAME_BOARD_STATE_MENU)
			{
				((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(0))->Activate();
				((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(1))->Activate();
				((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(2))->Activate();
				((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(3))->Activate();
			}
			else
			{
				// ONLY server can decide what sides next game will be
				if (g_Communication.GetAlreadyInited() && g_Communication.GetCommunicationType()==COMMUNICATION_SERVER)
				{
					((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(0))->Activate();
					((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(1))->Activate();
					((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(2))->Activate();
					((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(3))->Activate();
				}
				else
				{
					((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(0))->Deactivate();
					((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(1))->Deactivate();
					((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(2))->Deactivate();
					((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(3))->Deactivate();
				}
			}



			g_Communication.UpdateOptionsMenu();



			

			ResetBoardGameMenus();
			DeActivateAllMenus();

			m_pOptionsMenu->m_bRender = true;
			m_pOptionsMenu->m_bActive = true;		

			// fill in the current values
			if (!g_Communication.GetAlreadyInited())
			{
				if (g_pCheckersBoard->m_iPlayerTypePAST==PLAYER_TYPE_HUMAN)
				{
					((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(0))->SetState2();     
					((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(1))->SetState1();     
				}
				else // past army is pc
				{
					((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(0))->SetState1();     
					((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(1))->SetState2();     
				}



				if (g_pCheckersBoard->m_iPlayerTypeFUTURE==PLAYER_TYPE_HUMAN)
				{
					((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(2))->SetState2();     
					((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(3))->SetState1();     
				}
				else // past army is pc
				{
					((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(2))->SetState1();     
					((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(3))->SetState2();     
				}
			}


			
		}

		if (m_dwGUIMessages & GUI_MESSAGE_SET_PAST_ARMY_PC)
		{
			((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(0))->SetState1();     
			((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(2))->SetState2();     
			((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(3))->SetState1();     


			//g_pCheckersBoard->m_iPlayerTypePAST = PLAYER_TYPE_PC;
			//g_pCheckersBoard->m_iPlayerTypeFUTURE = PLAYER_TYPE_HUMAN;
		}

		if (m_dwGUIMessages & GUI_MESSAGE_SET_PAST_ARMY_HUMAN)
		{
			((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(1))->SetState1();            
			((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(2))->SetState1();     
			((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(3))->SetState2();     

			//g_pCheckersBoard->m_iPlayerTypePAST = PLAYER_TYPE_HUMAN;
			//g_pCheckersBoard->m_iPlayerTypeFUTURE = PLAYER_TYPE_PC;
		}


		if (m_dwGUIMessages & GUI_MESSAGE_SET_FUTURE_ARMY_PC)
		{
			((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(2))->SetState1();   
			((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(0))->SetState2();     
			((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(1))->SetState1();     

			//g_pCheckersBoard->m_iPlayerTypePAST = PLAYER_TYPE_HUMAN;
			//g_pCheckersBoard->m_iPlayerTypeFUTURE = PLAYER_TYPE_PC;
		}

		if (m_dwGUIMessages & GUI_MESSAGE_SET_FUTURE_ARMY_HUMAN)
		{
			((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(3))->SetState1();    
			((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(0))->SetState1();     
			((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(1))->SetState2();    

			//g_pCheckersBoard->m_iPlayerTypePAST = PLAYER_TYPE_PC;
			//g_pCheckersBoard->m_iPlayerTypeFUTURE = PLAYER_TYPE_HUMAN;
		}
		

		if (m_dwGUIMessages & GUI_MESSAGE_SET_ADVANTAGE_WEAPON)
		{
			((CGUIImage*)m_pAdvantageMenu->GetElementNum(3))->Activate();
			((CGUIImage*)m_pAdvantageMenu->GetElementNum(4))->Deactivate();    

			((CGUIImage*)m_pAdvantageMenu->GetElementNum(5))->Deactivate();
			((CGUIImage*)m_pAdvantageMenu->GetElementNum(6))->Activate();    

			((CGUIImage*)m_pAdvantageMenu->GetElementNum(7))->Deactivate();
			((CGUIImage*)m_pAdvantageMenu->GetElementNum(8))->Activate();  

			st_EngineI->AddEngineMessage(ENGINE_MESSAGE_ADVANTAGE_WEAPON);
						
		}

		if (m_dwGUIMessages & GUI_MESSAGE_SET_ADVANTAGE_EXTRALIFE)
		{
			((CGUIImage*)m_pAdvantageMenu->GetElementNum(3))->Deactivate();
			((CGUIImage*)m_pAdvantageMenu->GetElementNum(4))->Activate(); 

			((CGUIImage*)m_pAdvantageMenu->GetElementNum(5))->Activate();
			((CGUIImage*)m_pAdvantageMenu->GetElementNum(6))->Deactivate();  

			((CGUIImage*)m_pAdvantageMenu->GetElementNum(7))->Deactivate();
			((CGUIImage*)m_pAdvantageMenu->GetElementNum(8))->Activate();  

			st_EngineI->AddEngineMessage(ENGINE_MESSAGE_ADVANTAGE_EXTRALIFE);
		}


		if (m_dwGUIMessages & GUI_MESSAGE_SET_ADVANTAGE_AMMO)
		{
			((CGUIImage*)m_pAdvantageMenu->GetElementNum(3))->Deactivate();
			((CGUIImage*)m_pAdvantageMenu->GetElementNum(4))->Activate(); 

			((CGUIImage*)m_pAdvantageMenu->GetElementNum(5))->Deactivate();
			((CGUIImage*)m_pAdvantageMenu->GetElementNum(6))->Activate();  

			((CGUIImage*)m_pAdvantageMenu->GetElementNum(7))->Activate();
			((CGUIImage*)m_pAdvantageMenu->GetElementNum(8))->Deactivate();  

			st_EngineI->AddEngineMessage(ENGINE_MESSAGE_ADVANTAGE_AMMO);
		}
		


		
		
		

		//m_dwGUIMessages=0;


}


void CGUIManager::ResetBoardGameMenus(void)
{
	m_bChoseNetwork=false;

	m_pBoardCharacterMenu->m_bActive=true;
	m_pBoardCharacterMenu->m_bRender=true;

	m_pTopMenu->m_bActive=true;
	m_pTopMenu->m_bRender=true;

	m_pChatMenu->m_bActive=true; // here here
	m_pChatMenu->m_bRender=true;

	m_pAdvantageMenu->m_bActive=true;
	m_pAdvantageMenu->m_bRender=true;
	
	m_pAreYouSureMenuQUIT->m_bActive=false;
	m_pAreYouSureMenuQUIT->m_bRender=false;

	m_pAreYouSureMenuNEWGAME->m_bActive=false;
	m_pAreYouSureMenuNEWGAME->m_bRender=false;

	m_pSaveGameMenu->m_bActive=false;
	m_pSaveGameMenu->m_bRender=false;

	m_pChooseIP->m_bActive=false;
	m_pChooseIP->m_bRender=false;

	m_pLoadGameMenu->m_bActive=false;
	m_pLoadGameMenu->m_bRender=false;

	

	

	m_pOptionsMenu->m_bActive=false;
	m_pOptionsMenu->m_bRender=false;

	st_Input::Inst()->DisableMouseButtons();
}

void CGUIManager::DeActivateAllMenus(void)
{
	m_pBoardCharacterMenu->m_bActive=false;
	m_pTopMenu->m_bActive=false;
	m_pChatMenu->m_bActive=false;
	m_pAreYouSureMenuQUIT->m_bActive=false;
	m_pAreYouSureMenuNEWGAME->m_bActive=false;
	m_pSaveGameMenu->m_bActive=false;
	m_pChooseIP->m_bActive=false;
	m_pLoadGameMenu->m_bActive=false;
	m_pAdvantageMenu->m_bActive=false;
	m_pOptionsMenu->m_bActive=false;
}

void CGUIManager::ChangeToFPSBattlesGUIMode(void)
{
	DeActivateAllMenus();
	m_pChatMenu->m_bActive=true;

	m_pBoardCharacterMenu->m_bRender=false;
	m_pTopMenu->m_bRender=false;
	m_pAreYouSureMenuQUIT->m_bRender=false;
	m_pAreYouSureMenuNEWGAME->m_bRender=false;
	m_pSaveGameMenu->m_bRender=false;
	m_pChooseIP->m_bRender=false;
	m_pLoadGameMenu->m_bRender=false;
	m_pAdvantageMenu->m_bRender=false;
	m_pOptionsMenu->m_bRender=false;


}

bool CGUIManager::IsDefaultMenusAndNotQuitMenu(void)
{
	/*if (m_pAreYouSureMenuQUIT->m_bActive)
		return false;*/

	if (m_pAreYouSureMenuNEWGAME->m_bActive)
		return false;

	if (m_pSaveGameMenu->m_bActive)
		return false;

	if (m_pChooseIP->m_bActive)
		return false;

	if (m_pLoadGameMenu->m_bActive)
		return false;

	/*if (m_pAdvantageMenu->m_bActive)
		return false;*/

	

	if (m_pOptionsMenu->m_bActive)
		return false;

	return true;
}



bool CGUIManager::CanPlay(void)
{
	if (m_pTopMenu->m_bActive)
		if (m_pTopMenu->m_iPosY!=m_pTopMenu->m_iAnimatedStartY)
			return false;

	//m_pChatMenu = ?

	if (m_pAreYouSureMenuQUIT->m_bActive)
		return false;

	if (m_pAreYouSureMenuNEWGAME->m_bActive)
		return false;

	if (m_pSaveGameMenu->m_bActive)
		return false;

	if (m_pChooseIP->m_bActive)
		return false;

	if (m_pLoadGameMenu->m_bActive)
		return false;


	if (m_pOptionsMenu->m_bActive)
		return false;

	return true;
}