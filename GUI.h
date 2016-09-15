#ifndef YOEL_GUI_H
#define YOEL_GUI_H

#pragma once


#include "Shader.h"

#include <windows.h>


#define GUI_TYPE_IMAGE           1
#define GUI_TYPE_BUTTON_SIMPLE   2
#define GUI_TYPE_BUTTON_2_STATES 3
#define GUI_TYPE_WINDOW          4
#define GUI_TYPE_TEXT_EDIT_BOX   5

#include "SmartPointer.h"

class CGUIBase
{ 
 public: 
     
 CGUIBase() { m_iPosX=m_iPosY=m_iWidth=m_iHeight=m_iType=/*m_iID=*/0;m_bActive=true; }; 
 virtual ~CGUIBase() {}; 
 virtual void Render(void) {}; 
 virtual void OnMouse(void) {}; 
 virtual void OnKeyboard(void) {}; 
 bool IsMouseOn(void);

 	void Deactivate(void) { m_bActive=false;};
	void Activate(void) { m_bActive=true;};
	
 
 bool m_bActive;

 int m_iPosX;
 int m_iPosY; 
 int m_iWidth; 
 int m_iHeight; 
 
 int *m_piFatherX; 
 int *m_piFatherY; 
 
 int m_iType; 
 //int m_iID; 
 
  
};



#define BUTTON_STATE_UP     0
#define BUTTON_STATE_DOWN   1
#define BUTTON_STATE_HOVER  2

class CGUIImage : public CGUIBase
{
public:
	CGUIImage(){m_iType=GUI_TYPE_BUTTON_SIMPLE;m_iState=BUTTON_STATE_UP;m_bActive=true;};
	~CGUIImage() {};
	void Render(void);
	void OnMouse(void) {}; 
	void OnKeyboard(void) {}; 


	
	void SetShader(CShader* pSet) { m_spShader = pSet;}; 
	
private:



	int m_iState;

	CSmartPointer<CShader> m_spShader; 
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////


#define GUI_EDIT_BOX_MAX_SIZE 400

#define GUI_EDIT_BOX_TYPE_CHAT       0
#define GUI_EDIT_BOX_TYPE_IP_ADRESS  1

//#define GUI_EDIT_BOX_PRESS_DELAY 120.f
//#define GUI_EDIT_BOX_PRESS_DELAY 160.f
#define GUI_EDIT_BOX_PRESS_DELAY 200.f
#define GUI_EDIT_BOX_PRESS_BACKSPACE_DELAY 100.f

class CGUIEditBox : public CGUIBase
{
public:
	CGUIEditBox(){m_bIgnoreWriteMode=false;m_bWriteMode=false;caTextBACKUP[0]=NULL;m_iMaximumAcceptableChars=10;m_iBoxLength=10;m_fLastTimeButtonPressed=-1.f;m_bClearNextTime=false;m_bWritable=false;m_iEditBoxType=GUI_EDIT_BOX_TYPE_IP_ADRESS;m_dwPotentialMessage=0;m_iMessagesReciever=-1;updateMe=NULL;caText[0]='_';caText[1]=NULL;m_iPositionInText=0;m_iType=GUI_TYPE_TEXT_EDIT_BOX;m_bWritable=false;};
	~CGUIEditBox() {};
	void Render(void);
	void OnMouse(); 
	void OnKeyboard(void);

	int m_iEditBoxType;

	bool m_bWritable; // are we "focused" on this editbox

	bool m_bWriteMode; // when you press "enter" you move into writemode

	bool m_bIgnoreWriteMode; // always in writemode

	bool m_bClearNextTime;

	char caText[GUI_EDIT_BOX_MAX_SIZE];

	char caTextBACKUP[GUI_EDIT_BOX_MAX_SIZE];

	int m_iPositionInText;

	char* updateMe; // if not set to NULL, it will be updated with the contents of the edit box
	
	// in case of ENTER - Those message will be triggered
	int m_iMessagesReciever;  // do disable sent this to -1
	DWORD m_dwPotentialMessage;
	
	float m_fLastTimeButtonPressed;
	int   m_iLastButtonPressed;

	int m_iBoxLength;
	int m_iMaximumAcceptableChars;
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MESSAGES_RECIEVER_ENGINE  0
#define MESSAGES_RECIEVER_GUI     1



#define  GUI_MESSAGE_SHOW_NEW_GAME_MENU    (1<<1)
#define  GUI_MESSAGE_GET_TO_DEFAULT_MENUS   (1<<2)
#define  GUI_MESSAGE_SHOW_SAVE_GAME_MENU   (1<<3)
#define  GUI_MESSAGE_SHOW_LOAD_GAME_MENU   (1<<5)
#define  GUI_MESSAGE_SHOW_OPTIONS_MENU     (1<<7)
#define  GUI_MESSAGE_SHOW_QUIT_MENU        (1<<9)
#define  GUI_MESSAGE_SHOW_ADVANTAGE_MENU   (1<<10)
#define  GUI_MESSAGE_ESCAPE                (1<<11)
#define  GUI_MESSAGE_NEW _GAME             (1<<12)
#define  GUI_MESSAGE_LOAD_GAME             (1<<13)
#define  GUI_MESSAGE_SAVE_GAME             (1<<14)
#define  GUI_MESSAGE_SET_PAST_ARMY_HUMAN   (1<<15)
#define  GUI_MESSAGE_SET_PAST_ARMY_PC      (1<<16)
#define  GUI_MESSAGE_SET_FUTURE_ARMY_HUMAN (1<<17)
#define  GUI_MESSAGE_SET_FUTURE_ARMY_PC    (1<<18)
#define  GUI_MESSAGE_SET_ADVANTAGE_WEAPON  (1<<19)
#define  GUI_MESSAGE_SET_ADVANTAGE_EXTRALIFE  (1<<20)
#define  GUI_MESSAGE_SET_ADVANTAGE_AMMO  (1<<21)
#define  GUI_MESSAGE_SHOW_CHOOSE_IP_MENU  (1<<22)



class CGUIButton : public CGUIBase
{
public:
	CGUIButton(){m_iType=GUI_TYPE_BUTTON_SIMPLE;m_iState=BUTTON_STATE_UP;m_dwPotentialMessage=0;m_iMessagesReciever=MESSAGES_RECIEVER_ENGINE;};
	~CGUIButton() {};
	void Render(void);
	void OnMouse(); 
	void OnKeyboard(void) {}; 
	
	void SetShaderUp(CShader* pSet); 
    void SetShaderDown(CShader* pSet); 
    void SetShaderHover(CShader* pSet); 

	CShader*  GetShaderUp(void); 
    CShader*  GetShaderDown(void); 
    CShader*  GetShaderHover(void);  

	void SetPotentialMessage(DWORD dwSet) { m_dwPotentialMessage=dwSet;};
	void SetMessagesReciever(int iSet) { m_iMessagesReciever=iSet;};
	
private:

	int m_iMessagesReciever;
	DWORD m_dwPotentialMessage;

	int m_iState;

	CSmartPointer<CShader> m_spShaderUp; 
    CSmartPointer<CShader> m_spShaderDown; 
    CSmartPointer<CShader> m_spShaderHover;
};


#define TWO_SIDES_BUTTON_STATE_1   0
#define TWO_SIDES_BUTTON_STATE_2   1

class CGUI2StatesButton : public CGUIBase
{
public:
	CGUI2StatesButton(){m_iType=GUI_TYPE_IMAGE;m_iState=TWO_SIDES_BUTTON_STATE_1;m_dwPotentialMessage=0;m_iMessagesReciever=MESSAGES_RECIEVER_ENGINE;};
	~CGUI2StatesButton() {};
	void Render(void);
	void OnMouse(); 
	void OnKeyboard(void) {}; 
	
	void SetShaderState1(CShader* pSet); 
    void SetShaderState2(CShader* pSet); 

	CShader*  GetShaderState1(void); 
    CShader*  GetShaderState2(void); 

	void SetPotentialMessage(DWORD dwSet) { m_dwPotentialMessage=dwSet;};
	void SetMessagesReciever(int iSet) { m_iMessagesReciever=iSet;};

	void SetState1(void) { m_iState=TWO_SIDES_BUTTON_STATE_1;};
	void SetState2(void) { m_iState=TWO_SIDES_BUTTON_STATE_2;};

	int GetState(void) { return m_iState;};
	
private:

	int m_iMessagesReciever;
	DWORD m_dwPotentialMessage;

	int m_iState;


	CSmartPointer<CShader> m_spShaderState1; 
    CSmartPointer<CShader> m_spShaderState2; 
};




class CGUIWindow : public CGUIBase
{
public:
	CGUIWindow() {m_bAnimatedPop=false;m_piFatherX=m_piFatherY=NULL;m_iTotalElementsNum=0; m_iCurrentElementNum=0;m_bActive=true;m_bRender=true;};
	void Init(int iElementsNum);

	CGUIBase* GetElementNum(int iNum);
	CGUIBase* AddElement(int iType);

	void SetShader(CShader* pSet) { m_spShader = pSet;};

	void Render(void);
	void RenderText(void);
	void OnMouse(void); 
	void OnKeyboard(void);

	int m_iCurrentElementNum;
	int m_iTotalElementsNum;
	CGUIBase **m_ppElements;

	CSmartPointer<CShader> m_spShader;

	bool m_bActive;
	bool m_bRender;
    
	bool m_bAnimatedPop;

	int m_iAnimatedStartX;
	int m_iAnimatedStartY;

	int m_iAnimatedFinalX;
	int m_iAnimatedFinalY;

};



class CGUIManager
{
public:
	CGUIManager();
	~CGUIManager();

	bool UpdateMouseAndKeyboard(void); // returns true if the mouse is on any of menus/buttons/images and false if not

	void Init(bool bFirst);

	void Render(void);
	void RenderText(void);

	void HandleGUIMessages(void);
	void ResetGUIMessages(void) { m_dwGUIMessages=0;};
	
	void ResetBoardGameMenus(void);

	void DeActivateAllMenus(void);

	void ChangeToFPSBattlesGUIMode(void);

	bool CanPlay(void);

	bool IsDefaultMenusAndNotQuitMenu(void);
	
	
	CGUIWindow* m_pBoardCharacterMenu;
	CGUIWindow* m_pTopMenu;
	CGUIWindow* m_pAreYouSureMenuQUIT;
	CGUIWindow* m_pAreYouSureMenuNEWGAME;
	CGUIWindow* m_pSaveGameMenu;
	CGUIWindow* m_pLoadGameMenu;
	CGUIWindow* m_pOptionsMenu;
	CGUIWindow* m_pAdvantageMenu;
	CGUIWindow* m_pChooseIP;
	CGUIWindow* m_pChatMenu;

	DWORD m_dwActiveMenus;

	DWORD m_dwGUIMessages;

	bool m_bLastTimeEscape;
	bool m_bChoseNetwork;

};


#endif // YOEL_GUI_H