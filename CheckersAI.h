
#ifndef YOEL_CHECKERS_BOARD_AI_H
#define YOEL_CHECKERS_BOARD_AI_H

#pragma once


#include <vector>

#define SQUARES_NUM_X 8
#define SQUARES_NUM_Y 8


#define ARMY_SIDE_PAST     0
#define ARMY_SIDE_FUTURE   1

#define UNIT_TYPE_SOLDIER                    0
#define UNIT_TYPE_QUEEN                      1

struct sUnitAI
{
	int m_iExperiencePoints;
	int m_iType;
	bool m_bHoldAUnit;
	bool m_bArmySide;
	bool m_bParalised;
};

struct tCoord
{
	int x,y;
};

struct sMoveAI
{
	tCoord vFrom;
	tCoord vTo;	
	//tCoord* vComboMove; // only used in combo moves, for regular moves (free walk / one kill it remains untouched)
};

struct tCharmedUnit
{
	int    iPlayerIndex;
	int    iTurnCharmedNum;
};


// i need to check that it's still the same player index
// or else something like this can happen:
// a unit is paralised
// another unit attacks and kills it
// a unit goes to the place it occupied
// it's also being paralised now
// it's being revived although it shouldn't...

struct tParalisedUnit
{
	int    iPlayerIndex;
	tCoord coord;
	int    iTurnParalisedNum;
};



#define MAXIMUM_CURRENT_POSSIBLE_MOVES 100

typedef std::vector<sMoveAI> MOVES_VECTOR;
typedef std::vector<sMoveAI>::iterator MOVES_VECTOR_I;


class CCheckersAI
{
public:

	CCheckersAI();
	~CCheckersAI();

	void UpdateBoardFromGamePlay(void);

	bool FillCurrentPossibleMoves(bool bArmySide); // return false if NONE found, else returns true

	bool IsKillMove(int fromX,int fromY,int toX, int toY);


	//////////////// all this functions assume that possible moves were filled ///////////////////////

	sMoveAI FakeChooseBestMove(bool bArmySide); 

	void GetUnitsThatShouldBeBurned(bool bArmySide,sMoveAI &playerMove,MOVES_VECTOR & fillMe);


	//////////////////////////////////////////////////////////////////////////////////////////////////

	sUnitAI m_Board[SQUARES_NUM_X][SQUARES_NUM_Y];

	int m_iCurrentPossibleMovesPAST;
	sMoveAI* m_CurrentPossibleMovesPAST;

	int m_iCurrentPossibleMovesFUTURE;
	sMoveAI* m_CurrentPossibleMovesFUTURE;
	
};

#endif // YOEL_CHECKERS_BOARD_AI_H