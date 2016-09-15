#include "Communication.h"

#include "stdio.h"

#include "CheckersBoard.h"
extern CCheckersBoard* g_pCheckersBoard;

#include "Timer.h"

#include "SoundManager.h"
extern CSoundManager g_SoundManager;

#include "GUI.h"
extern CGUIManager* g_pGUIManager;

#include "LogFile.h"
extern CLogFile g_LogFile;

#include "TextMessages.h"
extern CTextMessages g_TextMessages;

#include "GUI.h"
extern CGUIManager* g_pGUIManager;

#include "q3amap.h"
extern CQuake3BSP* g_pQuake3Map;

#include <assert.h>
#include <crtdbg.h>

void CCommunication::Init(void)
{
	m_bInitedClientSockets=false;
	m_bStartedHandshake=false;
	m_bTryingToHost=false;
	m_fTryingToHostTime=0.f;

	m_fTimeLastBoardMessageSent=-1.f;
	m_iType=COMMUNICATION_NOT_SET;
	ZeroMemory(&RecieveBoardMessageString[0],100);
	RecieveBoardMessageString[0]='N';

	m_iNextChatMessageLength=-1;

	m_ConnectionSocket=NULL;

	m_iActiveMessagesInQueueNum = 0;

	m_iBufferedDataCurrentLength = 0;
}

bool CCommunication::AddMessageToQueue(sBoardGameMessage &addMe)
{
	if (m_iActiveMessagesInQueueNum>GAME_BOARD_MESSAGES_QUEUE_SIZE-1)
		return false;

	m_BoardGameMessagesQueue[m_iActiveMessagesInQueueNum].iType = addMe.iType;
	m_BoardGameMessagesQueue[m_iActiveMessagesInQueueNum].iSequence = addMe.iSequence; // we don't really care about this i think
	m_BoardGameMessagesQueue[m_iActiveMessagesInQueueNum].iCoordX = addMe.iCoordX;
	m_BoardGameMessagesQueue[m_iActiveMessagesInQueueNum].iCoordY = addMe.iCoordY;
	m_iActiveMessagesInQueueNum++;
	
	return true;
}

bool CCommunication::GetMessageFromQueue(sBoardGameMessage &changeMe)
{
	if (m_iActiveMessagesInQueueNum==0)
		return false;


	changeMe.iType = m_BoardGameMessagesQueue[0].iType;
	changeMe.iSequence = m_BoardGameMessagesQueue[0].iSequence;
	changeMe.iCoordX = m_BoardGameMessagesQueue[0].iCoordX;
	changeMe.iCoordY = m_BoardGameMessagesQueue[0].iCoordY;

	for (int i=0;i<m_iActiveMessagesInQueueNum-1;i++)
	{
		m_BoardGameMessagesQueue[i].iType = m_BoardGameMessagesQueue[i+1].iType;
		m_BoardGameMessagesQueue[i].iSequence = m_BoardGameMessagesQueue[i+1].iSequence;
		m_BoardGameMessagesQueue[i].iCoordX = m_BoardGameMessagesQueue[i+1].iCoordX;
		m_BoardGameMessagesQueue[i].iCoordY = m_BoardGameMessagesQueue[i+1].iCoordY;

	}

	m_iActiveMessagesInQueueNum--;

	/*changeMe.iType = m_BoardGameMessagesQueue[m_iActiveMessagesInQueueNum-1].iType;
	changeMe.iSequence = m_BoardGameMessagesQueue[m_iActiveMessagesInQueueNum-1].iSequence;
	changeMe.iCoordX = m_BoardGameMessagesQueue[m_iActiveMessagesInQueueNum-1].iCoordX;
	changeMe.iCoordY = m_BoardGameMessagesQueue[m_iActiveMessagesInQueueNum-1].iCoordY;
	m_iActiveMessagesInQueueNum--;*/

	return true;
}


/*

  void handle_error(void)
  {
    switch ( WSAGetLastError() )
    {
      case WSANOTINITIALISED :
        printf("Unable to initialise socket.\n");
      break;
      case WSAEAFNOSUPPORT :
        printf("The specified address family is not supported.\n");
      break;
      case WSAEADDRNOTAVAIL :
        printf("Specified address is not available from the local machine.\n");
      break;
      case WSAECONNREFUSED :
        printf("The attempt to connect was forcefully rejected.\n");
        break;
      case WSAEDESTADDRREQ :
        printf("address destination address is required.\n");
      break;
      case WSAEFAULT :
        printf("The namelen argument is incorrect.\n");
      break;
      case WSAEINVAL :
        printf("The socket is not already bound to an address.\n");
      break;
      case WSAEISCONN :
        printf("The socket is already connected.\n");
      break;
      case WSAEADDRINUSE :
        printf("The specified address is already in use.\n");
      break;
      case WSAEMFILE :
        printf("No more file descriptors are available.\n");
      break;
      case WSAENOBUFS :
        printf("No buffer space available. The socket cannot be created.\n");
      break;
      case WSAEPROTONOSUPPORT :
        printf("The specified protocol is not supported.\n");
        break;
      case WSAEPROTOTYPE :
        printf("The specified protocol is the wrong type for this socket.\n");
      break;
      case WSAENETUNREACH :
        printf("The network can't be reached from this host at this time.\n");
      break;
      case WSAENOTSOCK :
         printf("The descriptor is not a socket.\n");
      break;
      case WSAETIMEDOUT :
        printf("Attempt timed out without establishing a connection.\n");
      break;
      case WSAESOCKTNOSUPPORT :
         printf("Socket type is not supported in this address family.\n");
      break;
      case WSAENETDOWN :
        printf("Network subsystem failure.\n");
      break;
      case WSAHOST_NOT_FOUND :
        printf("Authoritative Answer Host not found.\n");
      break;
      case WSATRY_AGAIN :
        printf("Non-Authoritative Host not found or SERVERFAIL.\n");
       break;
      case WSANO_RECOVERY :
         printf("Non recoverable errors, FORMERR, REFUSED, NOTIMP.\n");
      break;
      case WSANO_DATA :
        printf("Valid name, no data record of requested type.\n");
      break;
        case WSAEINPROGRESS :
        printf("address blocking Windows Sockets operation is in progress.\n");
      break;
      case WSAEINTR :
        printf("The (blocking) call was canceled via WSACancelBlockingCall().\n");
      break;
      default :
        printf("Unknown error.\n");
       break;
  }

  WSACleanup();
  exit(0);
}

*/


int CCommunication::SendBoardMessage(int iType,int x, int y,char* pChatString)
{
	

	if (iType==NETWORK_BOARD_MESSAGE_NEXT_IS_CHAT && pChatString)
		x = strlen(pChatString)+1;
	else
	if (iType==NETWORK_BOARD_MESSAGE_CHAT && !pChatString)
		return 0;

	sBoardGameMessage s;

	s.iCoordX=x;
	s.iCoordY=y;
	s.iType=iType;

	if (iType!=NETWORK_BOARD_MESSAGE_CHAT)
		iDebugSentMessages++;
	s.iSequence=iDebugSentMessages;
	
	
	/*char msg[20];
	ZeroMemory(msg,20);
	sprintf(msg,"%d,%d",g_pCheckersBoard->m_iChosenUnitX);*/

	void* pData;

	pData = (char*) &s;

    //int iMsgLen, int iBytesSent;
    
    //iMsgLen = strlen(msg);

	int iReturn = WSAEINPROGRESS;

	int iTotalBytesSent=0;
	

	//todo: ADD SOME TIMEOUT!!!

	//WSAEINPROGRESS
	/*while (iReturn==WSAEINPROGRESS)
	{*/
		//Sleep(30);
		if (iType!=NETWORK_BOARD_MESSAGE_CHAT)
		{
			while (iTotalBytesSent < sizeof(sBoardGameMessage))
			{
				iReturn = send(m_StreamingSocket, &((char*)pData)[iTotalBytesSent], sizeof(sBoardGameMessage) - iTotalBytesSent, 0);
				if (iReturn>0)
					iTotalBytesSent+=iReturn;
				else
				if (iReturn==0)
				{
					g_TextMessages.AddMessage("Opponent Disconnected From Network.",0.f,0.f,1.f,TEXT_MESSAGE_TYPE_SYSTEM);
					return iReturn;
				}
				/*else 	//	if (iReturn <0) 
					return iReturn;*/
			}
		}
		else // iType==NETWORK_BOARD_MESSAGE_CHAT
		{			
			while (iTotalBytesSent < sizeof(sBoardGameMessage))
			{
				iReturn = send(m_StreamingSocket, &((char*)pChatString)[iTotalBytesSent], sizeof(char)*(strlen(pChatString)+1) - iTotalBytesSent, 0);
				if (iReturn>0)
					iTotalBytesSent+=iReturn;
				else
				if (iReturn==0)
				{
					g_TextMessages.AddMessage("Opponent Disconnected From Network.",0.f,0.f,1.f,TEXT_MESSAGE_TYPE_SYSTEM);
					return iReturn;
				}
				/*else 
					if (iReturn <0)
					return iReturn;*/
			}
		}



	//sprintf("


	/*}*/

	return iReturn;
}

void CCommunication::StopConnectionGame(void)
{
	m_bAlreadyInited=false;
	closesocket(m_ConnectionSocket);
	closesocket(m_StreamingSocket);
	m_ConnectionSocket=NULL;
	m_StreamingSocket=NULL;

	// end game
	g_pCheckersBoard->StopBackgroundMusic();
  	g_SoundManager.PlayStream(g_pCheckersBoard->m_ppBackgroundStreams[CB_SOUND_STREAM_INTRO]);
	g_pCheckersBoard->PreInit();
	g_pGUIManager->ResetBoardGameMenus();
	g_pCheckersBoard->m_iBoardGameState=GAME_BOARD_STATE_MENU;

	// maybe update the "options"
	g_pCheckersBoard->m_iPlayerTypePAST=PLAYER_TYPE_HUMAN;
 	g_pCheckersBoard->m_iPlayerTypeFUTURE=PLAYER_TYPE_PC;

	Init();
}




bool CCommunication::RecieveBoardMessage(void)
{
	char debugText[400];

	int iBytesRecieved;


	
	if (m_iNextChatMessageLength<0)
		iBytesRecieved = recv(m_StreamingSocket,m_cpCurrentReadData, sizeof(sBoardGameMessage) - m_iBufferedDataCurrentLength,0); 		
	else
		iBytesRecieved = recv(m_StreamingSocket,m_cpCurrentReadData, (sizeof(char)*m_iNextChatMessageLength) - m_iBufferedDataCurrentLength,0); 				


	if (iBytesRecieved>0)
		{
			memcpy(&m_cpBufferedData[m_iBufferedDataCurrentLength],m_cpCurrentReadData,iBytesRecieved);
			m_iBufferedDataCurrentLength+=iBytesRecieved;
		}
	
	/*ZeroMemory(RecieveBoardMessageString,20);
	sprintf(RecieveBoardMessageString,"%d,%d",m_BoardGameData.iCoordX,m_BoardGameData.iCoordY);*/
	
	 if (iBytesRecieved<0) // error
	 {
		int iError =  WSAGetLastError();
		/*if (iError!=WSAEWOULDBLOCK)
		{
    	 char error[100];
		 sprintf(error,"Recieving Failed - Error Code:%d",iError);			 
		 MessageBox(HWND_DESKTOP,error, "Error", MB_OK);
		}*/

		if (iError==WSAENOTSOCK || iError==WSAECONNABORTED ) // oponent disconnected
		{
			m_bTryingToHost=false;
			g_TextMessages.AddMessage("Opponent Disconnected From Network.",0.f,0.f,1.f,TEXT_MESSAGE_TYPE_SYSTEM);
			
     		StopConnectionGame();

			//// logging game for debug ////
			static char buffer[1024];
			 sprintf(buffer, "Opponent Disconnected!");
			 g_LogFile.OutPutPlainText(buffer,"3","009900",true);			
			 /////////////////////////////////		 
		}
		else
		if (iError==WSAEWOULDBLOCK)
		{
			// do nothing			
		}
		else
		{
			int iDebug=1;
		}

         //return false;
	 }
	 else
	 if (iBytesRecieved==0)
	 {
		
		 // recv() can return 0. This can mean only one thing: the remote side has closed the connection on you! 
		 // A return value of 0 is recv()'s way of letting you know this has occurred.

		 m_bTryingToHost=false;
		g_TextMessages.AddMessage("Opponent Disconnected From Network.",0.f,0.f,1.f,TEXT_MESSAGE_TYPE_SYSTEM);
			
     	StopConnectionGame();

		//// logging game for debug ////
		static char buffer[1024];
		 sprintf(buffer, "Opponent Disconnected!");
		 g_LogFile.OutPutPlainText(buffer,"3","009900",true);			
		 /////////////////////////////////

	 }
	 /*else // everything is ok
	 {*/


	 if (iBytesRecieved>0)
	 {
		 if (iBytesRecieved!=sizeof(sBoardGameMessage))
		 {
			 sprintf(debugText,"TCP: Bytes Recieved: %d",iBytesRecieved);
			 g_TextMessages.AddMessage(debugText,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
		 }
		 else
		 {
			 sprintf(debugText,"TCP: Bytes Recieved: %d",iBytesRecieved);
			 g_TextMessages.AddMessage(debugText,0.f,1.f,1.f,TEXT_MESSAGE_TYPE_SYSTEM);
		 }
	 }

		 


		 if ( m_iNextChatMessageLength < 0 ) 
		 {
			 if (m_iBufferedDataCurrentLength!=sizeof(sBoardGameMessage))
				 return false; // board action message not complete

			 // we have a valid board action message

			 memcpy((char*) &m_BoardGameData,m_cpBufferedData,sizeof(sBoardGameMessage));

			 m_iBufferedDataCurrentLength = 0;

			 if (m_BoardGameData.iType == NETWORK_BOARD_MESSAGE_NEXT_IS_CHAT )
				 m_iNextChatMessageLength = m_BoardGameData.iCoordX;

			 if (m_BoardGameData.iSequence>iDebugRecieveMessages+1)
				g_TextMessages.AddMessage("Packets difference too big!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			 iDebugRecieveMessages=m_BoardGameData.iSequence;

			 // DEBUG DEBUG DEBUG DEBUG DEBUG

		 	char message[500];

			sprintf(message,"Message:%d,%d,%d,%d",m_BoardGameData.iCoordX,m_BoardGameData.iCoordY,m_BoardGameData.iSequence,m_BoardGameData.iType);

			g_TextMessages.AddMessage(message,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);

			// DEBUG DEBUG DEBUG DEBUG DEBUG
				

			 return true;
		 }
		 else		 
		 {
			 if (m_iBufferedDataCurrentLength!=m_iNextChatMessageLength)
				 return false; // board chat message not complete

			 // we have a valid board chat message

			 g_TextMessages.AddMessage(m_cpBufferedData,1.f,0.f,1.f,TEXT_MESSAGE_TYPE_CHAT_OPONENT);
			 m_iNextChatMessageLength=-1;
 			 m_iBufferedDataCurrentLength = 0;

 			 if (m_BoardGameData.iSequence>iDebugRecieveMessages+1)
				g_TextMessages.AddMessage("Packets difference too big!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			 iDebugRecieveMessages=m_BoardGameData.iSequence;

			 return true;
		 }


		

			 //m_iBufferedDataCurrentLength

		/* if (m_BoardGameData.iSequence>iDebugRecieveMessages || m_iNextChatMessageLength>=0)
		 {			 */





  		    //// logging game for debug ///////////////////////////////////////////////////////////
		    /*static char buffer[1024];
			sprintf(buffer, "Recieve");
			g_LogFile.OutPutPlainText(buffer,"3","009900",false);
			g_LogFile.OutPutPlainText(":","3","000000",false);		
			sprintf(buffer, "#%d",m_BoardGameData.iSequence);
			g_LogFile.OutPutPlainText(buffer,"3","000099",true);
			sprintf(buffer, "Type: %d",m_BoardGameData.iType);
			g_LogFile.OutPutPlainText(buffer,"3","990000",true);
			sprintf(buffer, "Coords: %d,%d",m_BoardGameData.iCoordX,m_BoardGameData.iCoordY);
			g_LogFile.OutPutPlainText(buffer,"3","990000",true);	*/
			///////////////////////////////////////////////////////////////////////////////////////

			 



			 // chat

			 
/*		 }
		 else
			 return false;

		 
	 }

	 return false;*/
}

bool CCommunication::InitClient(char ipAdress[16])
{

	if (m_ConnectionSocket)
	{
		closesocket(m_ConnectionSocket);
		m_ConnectionSocket=NULL;
	}


	assert(_CrtCheckMemory());

	if (ipAdress[0]==NULL || ipAdress[1]==NULL)
	{
		g_TextMessages.AddMessage("IP doesn't fit standarts!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
		return false;
	}

	/*if (ipAdress[strlen(ipAdress)-1]=='_')
		ipAdress[strlen(ipAdress)-1]=NULL;*/


	assert(_CrtCheckMemory());

	int iDotsCount=0;
	for (int i=0;i<strlen(ipAdress);i++)
	{
		if (ipAdress[i]=='.')
			iDotsCount++;
	}

	assert(_CrtCheckMemory());

	if (iDotsCount!=3)
	{
		g_TextMessages.AddMessage("IP doesn't fit standarts!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
		return false;
	}

	assert(_CrtCheckMemory());


	m_bAlreadyInited = false;
	// ToDo: I need to test that ipAdress is VALID!!!!!!!!!!!!!!
	m_iType = COMMUNICATION_CLIENT;

	WSADATA wsaData;   // if this doesn't work
    
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) 
	{
	   //MessageBox(HWND_DESKTOP,"WSAStartup failed.\n", "Error", MB_OK);
	   g_TextMessages.AddMessage("WSAStartup failed.\n",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
       return false;
    } 

	

	m_ConnectionSocket = socket(AF_INET, SOCK_STREAM,0); 

	if (m_ConnectionSocket==INVALID_SOCKET)
	{
		 int iError =  WSAGetLastError();
    	 char error[50];
		 sprintf(error,"Creating Connection Socket Failed - Error Code:%d",iError);			 
		 //MessageBox(HWND_DESKTOP,error, "Error", MB_OK);
		 g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
         return false;
	}

    m_SocketAdressInPLAYER.sin_family = AF_INET;         // host byte order
	m_SocketAdressInPLAYER.sin_port = htons(SERVER_PORT_NUM_TCP);     // short, network byte order
	m_SocketAdressInPLAYER.sin_addr.s_addr = inet_addr(ipAdress); 
	memset(&(m_SocketAdressInPLAYER.sin_zero), '\0', 8); // zero the rest of the struct 


	// lose the pesky "Address already in use" error message
	char yes=1;
		
	if (setsockopt(m_ConnectionSocket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(char))) 
	{
	    int iError =  WSAGetLastError();
		 char error[50];
		 sprintf(error,"CLIENT: setsockopt Failed - Error Code:%d",iError);			 
		 //MessageBox(HWND_DESKTOP,error, "Error", MB_OK);
		 g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
	     return false;
	} 

/*
	unsigned long ulYes = 1; 
	if (ioctlsocket(m_ConnectionSocket, FIONBIO, &ulYes) == SOCKET_ERROR) 
	{
		char error[50];
		sprintf(error,"Client:Non Blocking Failed.");			 
		//MessageBox(HWND_DESKTOP,error, "Error", MB_OK);
		g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
		return false;
	}

	*/

	// "The TCP_NODELAY option disables the Nagle algorithm"
	yes=1;
		
	if (setsockopt(m_ConnectionSocket,SOL_SOCKET,TCP_NODELAY,&yes,sizeof(char))) 
	{
	    int iError =  WSAGetLastError();
		 char error[50];
		 sprintf(error,"CLIENT: setsockopt Failed - Error Code:%d",iError);			 
		 //MessageBox(HWND_DESKTOP,error, "Error", MB_OK);
		 g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
	     return false;
	} 
	m_bInitedClientSockets=true;


	// try connecting to the server
	if (connect(m_ConnectionSocket, (struct sockaddr *)&m_SocketAdressInPLAYER, sizeof(struct sockaddr)))
	//if (connect(m_StreamingSocket, (struct sockaddr *)&m_SocketAdressInPLAYER, sizeof(struct sockaddr)))	
	{
		int iError =  WSAGetLastError();
		char error[200];
		 sprintf(error,"CLIENT: Connecting To Server ""%s"" Failed - Error Code:%d",ipAdress,iError);			 
		 //MessageBox(HWND_DESKTOP,error, "Error", MB_OK);
		 g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
		 closesocket(m_ConnectionSocket);
	    return false;
		// here i should handle that "error"
		// look in the .net help for return value failure... it seems quite important
	}

	unsigned long ulYes = 1; 
	if (ioctlsocket(m_ConnectionSocket, FIONBIO, &ulYes) == SOCKET_ERROR) 
	{
		char error[50];
		 sprintf(error,"non blocking");			 
	    //MessageBox(HWND_DESKTOP,error, "Error", MB_OK);
		 g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
		return false;
	}

	m_StreamingSocket=m_ConnectionSocket;


	m_bAlreadyInited = true;

	iDebugSentMessages=0;
	iDebugRecieveMessages=0;

	assert(_CrtCheckMemory());

	static char done[200];

	sprintf(done,"CLIENT:Network Connection To IP:%s Succesfully Created.",ipAdress);

	strcpy(cSavedOponentIPAdress,ipAdress);

	g_TextMessages.AddMessage(done,0.f,1.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);

	assert(_CrtCheckMemory());

	m_bStartedHandshake=true;


	/////  init udp

	InitUDPSockets();

	return true;

}

bool CCommunication::InitServer(void)
{
	
	if (!m_bTryingToHost) // if first attempt
	{
		int iTemp = FIRST_CONNECTION_MAX_TIME / 1000;
		char WaitingForClient[200];
		g_TextMessages.AddMessage("Waiting For Clients Started",0.f,0.f,1.f,TEXT_MESSAGE_TYPE_SYSTEM);		
		sprintf(WaitingForClient,"(auto timeout %d Seconds)",iTemp);
		g_TextMessages.AddMessage(WaitingForClient,0.f,0.f,1.f,TEXT_MESSAGE_TYPE_SYSTEM);

		m_bAlreadyInited = false;
		m_iType = COMMUNICATION_SERVER;

		WSADATA wsaData;   // if this doesn't work
    
		if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) 
		{
		//MessageBox(HWND_DESKTOP,"WSAStartup failed.\n", "Error", MB_OK);
			g_TextMessages.AddMessage("WSAStartup failed.\n",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
		return false;
		} 




		m_ConnectionSocket = socket(AF_INET, SOCK_STREAM,0); 

		if (m_ConnectionSocket==INVALID_SOCKET)
		{
			int iError =  WSAGetLastError();
    		char error[50];
			sprintf(error,"Creating Connection Socket Failed - Error Code:%d",iError);			 
			//MessageBox(HWND_DESKTOP,error, "Error", MB_OK);
			g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			return false;
		}
	 

		m_SocketAdressInPLAYER.sin_family = AF_INET;         // host byte order
		m_SocketAdressInPLAYER.sin_port = htons(SERVER_PORT_NUM_TCP);     // short, network byte order
		m_SocketAdressInPLAYER.sin_addr.s_addr = htonl (INADDR_ANY);
		memset(&(m_SocketAdressInPLAYER.sin_zero), '\0', 8); // zero the rest of the struct 


		// if bind() return zero than it means it's OK!
	    if (bind(m_ConnectionSocket, (struct sockaddr *)&m_SocketAdressInPLAYER, sizeof(struct sockaddr)))
		{
    		int iError =  WSAGetLastError();
			char error[50];
			sprintf(error,"SERVER: Binding Socket Failed - Error Code:%d",iError);			 
			//MessageBox(HWND_DESKTOP,error, "Error", MB_OK);
			g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			return false;
		}


		unsigned long ulYes = 1; 
		if (ioctlsocket(m_ConnectionSocket, FIONBIO, &ulYes) == SOCKET_ERROR) 
		{
			char error[50];
			sprintf(error,"non blocking");			 
			//MessageBox(HWND_DESKTOP,error, "Error", MB_OK);
			g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			return false;
		}

		// "The TCP_NODELAY option disables the Nagle algorithm"
		char yes=1;
			
		if (setsockopt(m_ConnectionSocket,SOL_SOCKET,TCP_NODELAY,&yes,sizeof(char))) 
		{
			int iError =  WSAGetLastError();
			char error[50];
			sprintf(error,"CLIENT: setsockopt Failed - Error Code:%d",iError);			 
			//MessageBox(HWND_DESKTOP,error, "Error", MB_OK);
			g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			return false;
		} 
	}// end of "if first try"

	// try listenning to the client - allowing only 1 client
	listen(m_ConnectionSocket, 1);
	
	bool bSuccess=false;
	bool bTimeOut=false;

	int sin_size = sizeof(struct sockaddr_in);

	
	if (!m_bTryingToHost)
	{
		m_bTryingToHost=true;
		m_fTryingToHostTime=TIMER_GET_TIME;
	}

	
	if ((TIMER_GET_TIME-m_fTryingToHostTime)>=FIRST_CONNECTION_MAX_TIME)
	{
		m_bTryingToHost=false;
		closesocket(m_StreamingSocket);
		closesocket(m_ConnectionSocket);
		m_bAlreadyInited=false;

		g_TextMessages.AddMessage("Waiting For Clients TimeOut.",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);

		return false;
	}
	

	
	
	
	m_StreamingSocket = accept(m_ConnectionSocket, (struct sockaddr *)&m_SocketAdressInOPONENT, &sin_size);
	if (m_StreamingSocket!=INVALID_SOCKET)
		bSuccess=true;
	else // INVALID_SOCKET
		return false;
	
	

	strcpy(cSavedOponentIPAdress,inet_ntoa(m_SocketAdressInOPONENT.sin_addr));


	m_bAlreadyInited = true;

	iDebugSentMessages=0;
	iDebugRecieveMessages=0;

	char message[100];

	sprintf(message,"SERVER:Successfull client join! (%s)",cSavedOponentIPAdress);
	
	g_TextMessages.AddMessage(message,0.f,1.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
	



	/////  init udp

	InitUDPSockets();
	
	return true;
}

void CCommunication::SendFPSBattleMessage(CVector3f& vPlayerPosition,CVector3f& vVelocity, CVector3f& vAcceleration,
	        CVector3f& vDesiredPosition,float fPlayerAngX, float fPlayerAngY,DWORD dwPlayerState,DWORD dwPlayerActions)
{
	static sFPSBattleMessage message;

	message.vPlayerPosition = vPlayerPosition;
	message.vVelocity       = vVelocity;
	message.vAcceleration   = vAcceleration;
	//message.vDesiredPosition = vDesiredPosition;

	message.fPlayerAngX     = fPlayerAngX;
	message.fPlayerAngY     = fPlayerAngY;
	message.dwPlayerState  = dwPlayerState;
	message.dwPlayerActions = dwPlayerActions;

	sFPSBattleMessage* pData = &message;

	int iAdressLength = sizeof(sockaddr);
	int iNumBytes;
	if ((iNumBytes=sendto(m_UDPSocket,((char*) pData), sizeof(sFPSBattleMessage), 0,
		(sockaddr *)&m_SocketAdressInOPONENT_UDP, sizeof(m_SocketAdressInOPONENT_UDP))) == -1)
	{
           //g_TextMessages.AddMessage("Failed To UDP Send!!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
    }

}

bool CCommunication::RecieveFPSBattleMessage(void)
{
	//g_TextMessages.AddMessage("Attempting To Recieve!.",0.f,0.f,1.f,TEXT_MESSAGE_TYPE_SYSTEM);

	sFPSBattleMessage* pData = &m_FPSBattleData;
	//m_FPSBattleData.vPlayerPosition.Set(0.f,0.f,0.f);

	 int iAdressLength = sizeof(sockaddr);
	 int iNumBytes;
	 if ((iNumBytes=recvfrom(m_UDPSocket,(char*)pData, sizeof(sFPSBattleMessage), 0,
                           (struct sockaddr *)&m_SocketAdressInOPONENT_UDP, &iAdressLength)) == -1) 
	 {
            //perror("recvfrom");
            //exit(1);
		 return false;
     }
	 else
	 {
		 g_pQuake3Map->m_bRecievedFirstUDPMessage=true;
		 return true;
	 }

	 return false;
}

void CCommunication::InitUDPSockets(void)
{
	if ((m_UDPSocket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) 
	{
		g_TextMessages.AddMessage("UDP: socket() failed",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
		return;
    }


	/****/

	unsigned long ulYes = 1; 
	if (ioctlsocket(m_UDPSocket, FIONBIO, &ulYes) == SOCKET_ERROR) 
	{
		char error[50];
		sprintf(error,"UDP:non blocking");			 
		//MessageBox(HWND_DESKTOP,error, "Error", MB_OK);
		g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
		return;
	}



	/****/


	m_SocketAdressInPLAYER_UDP.sin_family = AF_INET;         // host byte order

	if (m_iType==COMMUNICATION_SERVER)
		m_SocketAdressInPLAYER_UDP.sin_port = htons(PORT_NUM_UDP_SERVER);     // short, network byte order
	else
		m_SocketAdressInPLAYER_UDP.sin_port = htons(PORT_NUM_UDP_CLIENT);     // short, network byte order

    m_SocketAdressInPLAYER_UDP.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
    memset(&(m_SocketAdressInPLAYER_UDP.sin_zero), '\0', 8); // zero the rest of the struct

	int iBindResult = bind(m_UDPSocket, (sockaddr *)&m_SocketAdressInPLAYER_UDP,sizeof(sockaddr));
	
	if (iBindResult)
	{     g_TextMessages.AddMessage("UDP: bind() failed",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);;


			int iError =  WSAGetLastError();
			char error[50];
			sprintf(error,"UDP:bind() failed:%d",iError);			 
			//MessageBox(HWND_DESKTOP,error, "Error", MB_OK);
			g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);

          return;
    }




	m_SocketAdressInOPONENT_UDP.sin_family = AF_INET;         // host byte order

	if (m_iType==COMMUNICATION_SERVER)
		m_SocketAdressInOPONENT_UDP.sin_port = htons(PORT_NUM_UDP_CLIENT);     // short, network byte order
	else
		m_SocketAdressInOPONENT_UDP.sin_port = htons(PORT_NUM_UDP_SERVER);     // short, network byte order

    m_SocketAdressInOPONENT_UDP.sin_addr.s_addr =  inet_addr(cSavedOponentIPAdress);
    memset(&(m_SocketAdressInOPONENT_UDP.sin_zero), '\0', 8); // zero the rest of the struct


	/*
	// trying to make it faster

	// lose the pesky "Address already in use" error message
	char yes=1;
		
	if (setsockopt(m_UDPSocket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(char))) 
	{
	    int iError =  WSAGetLastError();
		 char error[50];
		 sprintf(error,"UDP: setsockopt Failed - Error Code:%d",iError);			 
		 //MessageBox(HWND_DESKTOP,error, "Error", MB_OK);
		 g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
	     return;
	} 

	*/

	/*
	yes=1;
		
	if (setsockopt(m_UDPSocket,SOL_SOCKET,UDP_NODELAY,&yes,sizeof(char))) 
	{
	    int iError =  WSAGetLastError();
		 char error[50];
		 sprintf(error,"UDP: setsockopt Failed - Error Code:%d",iError);			 
		 //MessageBox(HWND_DESKTOP,error, "Error", MB_OK);
		 g_TextMessages.AddMessage(error,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
	     return;
	} 
	*/

	/***/

	/////

	/*if ((m_UDPSocketOutgoing = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) 
	{
		g_TextMessages.AddMessage("UDP: socket() failed",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
		return;
    }

	m_SocketAdressInOPONENT_UDP.sin_family = AF_INET;         // host byte order
    m_SocketAdressInOPONENT_UDP.sin_port = htons(SERVER_PORT_NUM_UDP_OUTGOING);     // short, network byte order
    m_SocketAdressInOPONENT_UDP.sin_addr.s_addr =  inet_addr(cSavedOponentIPAdress);
    memset(&(m_SocketAdressInOPONENT_UDP.sin_zero), '\0', 8); // zero the rest of the struct

	iBindResult = bind(m_UDPSocketOutgoing, (sockaddr *)&m_SocketAdressInOPONENT_UDP,sizeof(struct sockaddr));
	
	if (iBindResult)
	{
          g_TextMessages.AddMessage("UDP: socket() failed",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);;
          return;
    }*/



}

void CCommunication::SERVERStopWaitingForClients(void)
{	
	if (!m_bTryingToHost)
		g_TextMessages.AddMessage("There Is No Hosting Attempt To Stop.",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);

	m_bTryingToHost=false;
	closesocket(m_StreamingSocket);
	closesocket(m_ConnectionSocket);
	

	if (m_bAlreadyInited)
	{
		g_TextMessages.AddMessage("You Have Disconnected From Network.",0.f,0.f,1.f,TEXT_MESSAGE_TYPE_SYSTEM);
		 StopConnectionGame();
	}
	else
		g_TextMessages.AddMessage("Waiting For Clients Stopped By User.",0.f,0.f,1.f,TEXT_MESSAGE_TYPE_SYSTEM);


	m_bAlreadyInited=false;
}

void CCommunication::CLIENTStopConnection(void)
{	
	closesocket(m_StreamingSocket);
	closesocket(m_ConnectionSocket);
	
	if (m_bAlreadyInited)
	{
		g_TextMessages.AddMessage("You Have Disconnected From Network.",0.f,0.f,1.f,TEXT_MESSAGE_TYPE_SYSTEM);
		 StopConnectionGame();
	}

	m_bAlreadyInited=false;
}

bool CCommunication::Update(void)
{
	if (m_bAlreadyInited)
		return false;

	if (m_bTryingToHost)
	{
		if (m_iType==COMMUNICATION_SERVER)
			if (InitServer())
				return true;
	}


	return false;
}

void CCommunication::UpdateOptionsMenu(void)
{
	CGUIWindow* pMenu= g_pGUIManager->m_pOptionsMenu;

	if (GetAlreadyInited())
			{
				if (GetCommunicationType()==COMMUNICATION_SERVER)
				{
					
					((CGUIButton*)pMenu->GetElementNum(7))->Activate();
					((CGUIButton*)pMenu->GetElementNum(8))->Deactivate();


					((CGUIButton*)pMenu->GetElementNum(4))->Activate();
					((CGUIButton*)pMenu->GetElementNum(5))->Deactivate();
				}
				else
				if (GetCommunicationType()==COMMUNICATION_CLIENT)
				{
					((CGUIButton*)pMenu->GetElementNum(7))->Deactivate();
					((CGUIButton*)pMenu->GetElementNum(8))->Activate();

					((CGUIButton*)pMenu->GetElementNum(4))->Deactivate();
					((CGUIButton*)pMenu->GetElementNum(5))->Activate();
				}
				
			}
			else
			{
				if (GetTryingToHost())
				{
					((CGUIButton*)pMenu->GetElementNum(7))->Activate();
					((CGUIButton*)pMenu->GetElementNum(8))->Deactivate();
					((CGUIButton*)pMenu->GetElementNum(4))->Activate();
					((CGUIButton*)pMenu->GetElementNum(5))->Deactivate();
				}
				else
				{
					((CGUIButton*)pMenu->GetElementNum(4))->Activate();
					((CGUIButton*)pMenu->GetElementNum(5))->Activate();
					((CGUIButton*)pMenu->GetElementNum(7))->Deactivate();
					((CGUIButton*)pMenu->GetElementNum(8))->Deactivate();
				}
			}

}