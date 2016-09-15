#ifndef YOEL_COMMUNICATION_H
#define YOEL_COMMUNICATION_H

#pragma once


#include <winsock.h>

#include "Math3D.h"

#define COMMUNICATION_NOT_SET 0
#define COMMUNICATION_SERVER  1
#define COMMUNICATION_CLIENT  2

#define SERVER_PORT_NUM_TCP 5555

//#define SERVER_PORT_NUM_UDP 5557 // i am splitting this only for the purpose of debugging on same pc!!!
#define PORT_NUM_UDP_SERVER 5557
#define PORT_NUM_UDP_CLIENT 5558


//#define FIRST_CONNECTION_MAX_TIME 10000
//#define FIRST_CONNECTION_MAX_TIME 5000
#define FIRST_CONNECTION_MAX_TIME 600000

// board game
#define NETWORK_BOARD_MESSAGE_BOARD_MOVE     0
#define NETWORK_BOARD_MESSAGE_CAST_MAGIC     1
// board game+fps battles
#define NETWORK_BOARD_MESSAGE_CHAT           2
#define NETWORK_BOARD_MESSAGE_NEXT_IS_CHAT   3
// board game
#define NETWORK_BOARD_MESSAGE_NEW_GAME_SERVER_IS_PAST    4
#define NETWORK_BOARD_MESSAGE_NEW_GAME_SERVER_IS_FUTURE  5
#define NETWORK_BOARD_MESSAGE_NEW_GAME_SERVER_IS_FUTURE  6 //wtf ? why 2 identical stuff ??
// fps battles
#define NETWORK_BOARD_MESSAGE_OPPONENT_KILLED_SELF          7
#define NETWORK_BOARD_MESSAGE_OPPONENT_ADVANTAGE_WEAPON     8
#define NETWORK_BOARD_MESSAGE_OPPONENT_ADVANTAGE_EXTRALIFE  9
#define NETWORK_BOARD_MESSAGE_OPPONENT_ADVANTAGE_AMMO      10



#define GAME_BOARD_MESSAGES_QUEUE_SIZE 100

// board game message

struct sBoardGameMessage
{
	int iSequence;
	int iType;
	int iCoordX;
	int iCoordY;
};

#define FPS_BATTLE_MESSAGE_NOTHING (0)
#define FPS_BATTLE_MESSAGE_SHOOT   (1<<1)
// FPS battles message

struct sFPSBattleMessage
{
	CVector3f vPlayerPosition;
	CVector3f vVelocity;
	CVector3f vAcceleration;
	//CVector3f vDesiredPosition;
	float fPlayerAngX,fPlayerAngY;
	DWORD dwPlayerState;
	DWORD dwPlayerActions;
};


class CCommunication
{
public:
	CCommunication() {Init();};
	~CCommunication() {closesocket(m_ConnectionSocket);closesocket(m_StreamingSocket);WSACleanup();};

	void Init(void);

	bool Update(void); // keeps trying to recieve clients in case of a server
	bool InitServer(void);
	bool InitClient(char ipAdress[16]);

	bool GetAlreadyInited(void) { return m_bAlreadyInited;};
	void SetAlreadyInited(bool bSet) { m_bAlreadyInited=bSet;};
	bool GetTryingToHost(void) { return m_bTryingToHost;};


	///////////////////////////////////////////////////////
	// Board game related
	// All board game networking is done with tcp/ip
	///////////////////////////////////////////////////////

	// tcp/ip
	int  SendBoardMessage(int iType,int x, int y,char* pChatString);
	bool RecieveBoardMessage(void);

	int GetCommunicationType(void) { return m_iType;};


	void StopConnectionGame(void);

	void SERVERStopWaitingForClients(void);
	void CLIENTStopConnection(void);

	void UpdateOptionsMenu(void);

	char RecieveBoardMessageString[100];
	
	sBoardGameMessage m_BoardGameData;                 // last board game data that was recieved
	//char              m_ChatRecieve[300];     // last chat that was recieved

	sBoardGameMessage m_BoardGameMessagesQueue[GAME_BOARD_MESSAGES_QUEUE_SIZE];                 // last board game data that was recieved
	int               m_iActiveMessagesInQueueNum;

	bool m_bStartedHandshake; // only client cares about this


	bool AddMessageToQueue(sBoardGameMessage &addMe);
	bool GetMessageFromQueue(sBoardGameMessage &changeMe);

	///////////////////////////////////////////////////////
	// FPS battles related
	// All board game networking is done with udp/ip
	///////////////////////////////////////////////////////

	sFPSBattleMessage m_FPSBattleData;                 // last FPS battle data that was recieved

	void InitUDPSockets(void);

	void SendFPSBattleMessage(CVector3f& vPlayerPosition,CVector3f& vVelocity, CVector3f& vAcceleration,CVector3f& vDesiredPosition,
		                   float fPlayerAngX, float fPlayerAngY,DWORD dwPlayerState,DWORD dwPlayerActions);
	bool RecieveFPSBattleMessage(void);

	//bool RecievedFirstUDPMessage(void) { return m_bRecievedFirstUDPMessage;};
	//void 

private:
	///////////////////////////////////////////////////////
	// Board game related
	// All board game networking is done with tcp/ip
	///////////////////////////////////////////////////////

	int  m_iBufferedDataCurrentLength;
	char m_cpBufferedData[1000];
	char m_cpCurrentReadData[500];

	float m_fTimeLastBoardMessageSent;
	int m_iType;

	sockaddr_in m_SocketAdressInPLAYER;
	sockaddr_in m_SocketAdressInOPONENT;

	bool m_bAlreadyInited;

	SOCKET m_ConnectionSocket;
	SOCKET m_StreamingSocket;

	int iDebugSentMessages;
	int iDebugRecieveMessages;	

	int m_iNextChatMessageLength;

	bool  m_bTryingToHost;
	float m_fTryingToHostTime;

	bool m_bInitedClientSockets;
	
	char cSavedOponentIPAdress[16];



	///////////////////////////////////////////////////////
	// FPS battles related
	// All board game networking is done with udp/ip
	///////////////////////////////////////////////////////


	SOCKET m_UDPSocket;

	sockaddr_in m_SocketAdressInPLAYER_UDP;
	sockaddr_in m_SocketAdressInOPONENT_UDP;
	
	

};



#endif // YOEL_COMMUNICATION_H