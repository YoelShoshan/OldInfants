#include "CheckersAI.h"

#include <windows.h>
#include "Defines.h"


#include "GMath.h"

CCheckersAI::CCheckersAI()
{
	m_iCurrentPossibleMovesPAST = 0;
	m_CurrentPossibleMovesPAST = new sMoveAI[MAXIMUM_CURRENT_POSSIBLE_MOVES];

	m_iCurrentPossibleMovesFUTURE = 0;
	m_CurrentPossibleMovesFUTURE = new sMoveAI[MAXIMUM_CURRENT_POSSIBLE_MOVES];

	
}

CCheckersAI::~CCheckersAI()
{
	SafeDeleteArray(m_CurrentPossibleMovesPAST);

	SafeDeleteArray(m_CurrentPossibleMovesFUTURE);
}

bool CCheckersAI::FillCurrentPossibleMoves(bool bArmySide) // return false if NONE found, else returns true
{

	if (bArmySide==ARMY_SIDE_PAST)
		m_iCurrentPossibleMovesPAST=0;
	else
		m_iCurrentPossibleMovesFUTURE=0;


	/**/
	/*sUnitAI (*pBoard)[SQUARES_NUM_X][SQUARES_NUM_Y];
	pBoard = &m_Board;*/
	/**/



	sMoveAI* pPossibleMoves;

	if (bArmySide==ARMY_SIDE_PAST)
		pPossibleMoves = m_CurrentPossibleMovesPAST;
	else
		pPossibleMoves = m_CurrentPossibleMovesFUTURE;

	int iPossibleMoves = 0;

	//m_CurrentPossibleMovesPAST
	


	// hmm - i should consider cases that the unit can kill... a walk move by that unit is ILLEGAL!!!




	// first check for attack moves

	for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
		{
			if (m_Board[i][j].m_bHoldAUnit) // there is a unit here
				if (m_Board[i][j].m_bArmySide==bArmySide) // it's from "our" army
				{
					if (m_Board[i][j].m_bParalised)
						continue;

					if (m_Board[i][j].m_iType==UNIT_TYPE_SOLDIER) // it's a soldier
					{		
					int iDirectionY;
					if (bArmySide==ARMY_SIDE_PAST)
						iDirectionY = 1; // going up
					else
						iDirectionY = -1; // going down

					if (bArmySide==ARMY_SIDE_PAST)
					{
						if (j>SQUARES_NUM_Y-3)
							continue;
					}
					else
					{
						if (j<2)
							continue;
					}
						

					// left (up/down) attack move
					if (i>1 )
						if (m_Board[i-1][j+iDirectionY].m_bHoldAUnit) // if there is a unit to attack
							if (m_Board[i-1][j+iDirectionY].m_bArmySide!=bArmySide) //if the unit is from the enemy army							
								if (!m_Board[i-2][j+(2*iDirectionY)].m_bHoldAUnit) // if the landing place is free
								{
									pPossibleMoves[iPossibleMoves].vFrom.x = i;
									pPossibleMoves[iPossibleMoves].vFrom.y = j;
									pPossibleMoves[iPossibleMoves].vTo.x = i-2;
									pPossibleMoves[iPossibleMoves].vTo.y = j+(2*iDirectionY);

									iPossibleMoves++;
								}

					// right (up/down) attack move
					if (i<SQUARES_NUM_X-2)
						if (m_Board[i+1][j+iDirectionY].m_bHoldAUnit) // if there is a unit to attack
							if (m_Board[i+1][j+iDirectionY].m_bArmySide!=bArmySide) //if the unit is from the enemy army							
								if (!m_Board[i+2][j+(2*iDirectionY)].m_bHoldAUnit) // if the landing place is free
								{
									pPossibleMoves[iPossibleMoves].vFrom.x = i;
									pPossibleMoves[iPossibleMoves].vFrom.y = j;
									pPossibleMoves[iPossibleMoves].vTo.x = i+2;
									pPossibleMoves[iPossibleMoves].vTo.y = j+(2*iDirectionY);

									iPossibleMoves++;
								}

					}
					else if (m_Board[i][j].m_iType==UNIT_TYPE_QUEEN) // it's a queen
					{
						// handle queen

						for (int ii=0;ii<SQUARES_NUM_X;ii++)
							for (int jj=0;jj<SQUARES_NUM_Y;jj++)
							if ( IsOdd(ii+jj))
								if (IsKillMove(i,j,ii,jj))
								{
									pPossibleMoves[iPossibleMoves].vFrom.x = i;
									pPossibleMoves[iPossibleMoves].vFrom.y = j;
									pPossibleMoves[iPossibleMoves].vTo.x = ii;
									pPossibleMoves[iPossibleMoves].vTo.y = jj;

									iPossibleMoves++;
								}

						
					}

				}	
		}


		bool bStop=false;

	// then check for free walk moves

	for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
		{
			if (m_Board[i][j].m_bHoldAUnit) // there is a unit here
				if (m_Board[i][j].m_bArmySide==bArmySide) // it's from "our" army
				{

					if (m_Board[i][j].m_bParalised)
						continue;

					/////////////////////////////////////////////////////////////////////////////////
					// if this unit capable of attacking, a walk move is ILLEGAL!!!
					
					bStop=false;

					for (int c=0;c<iPossibleMoves;c++)
					{
						if (i==pPossibleMoves[iPossibleMoves].vFrom.x &&
							j==pPossibleMoves[iPossibleMoves].vFrom.y)
						if ( abs(j-pPossibleMoves[iPossibleMoves].vFrom.y)>1)
							bStop=true;				
					}

					if (bStop)
						continue;


					/////////////////////////////////////////////////////////////////////////////////

					if (m_Board[i][j].m_iType==UNIT_TYPE_SOLDIER) // it's a soldier
					{		
					int iDirectionY;
					if (bArmySide==ARMY_SIDE_PAST)
						iDirectionY = 1; // going up
					else
						iDirectionY = -1; // going down

					if (bArmySide==ARMY_SIDE_PAST)
					{
						if (j>SQUARES_NUM_Y-2)
							continue;
					}
					else
					{
						if (j<1)
							continue;
					}

					// left (up/down) free walk move
					if (i>0)
						if (!m_Board[i-1][j+iDirectionY].m_bHoldAUnit) 
						{
							pPossibleMoves[iPossibleMoves].vFrom.x = i;
							pPossibleMoves[iPossibleMoves].vFrom.y = j;
							pPossibleMoves[iPossibleMoves].vTo.x = i-1;
							pPossibleMoves[iPossibleMoves].vTo.y = j+iDirectionY;

							iPossibleMoves++;
						}

					// right (up/down) free walk move
					if (i<SQUARES_NUM_X-1)
						if (!m_Board[i+1][j+iDirectionY].m_bHoldAUnit) 
						{

							/*pPossibleMoves[iPossibleMoves].vFrom.x = i;
							pPossibleMoves[iPossibleMoves].vFrom.y = j;
							pPossibleMoves[iPossibleMoves].vTo.x = ;
							pPossibleMoves[iPossibleMoves].vTo.y = ;*/

							pPossibleMoves[iPossibleMoves].vFrom.x = i;
							pPossibleMoves[iPossibleMoves].vFrom.y = j;
							pPossibleMoves[iPossibleMoves].vTo.x = i+1;
							pPossibleMoves[iPossibleMoves].vTo.y = j+iDirectionY;

							iPossibleMoves++;
						}

					}
					else if (m_Board[i][j].m_iType==UNIT_TYPE_QUEEN) // it's a queen
					{
						// handle queen

						// free walk
						
						for (int iTo=0;iTo<SQUARES_NUM_X;iTo++)
							for (int jTo=0;jTo<SQUARES_NUM_Y;jTo++)
							{
								if ( (iTo==i) && (jTo==j))
									continue; // don't check itself

								int iDifferenceX,iDifferenceY;
								iDifferenceX = iTo - i;
								iDifferenceY = jTo - j;

								if (abs(iDifferenceX)==abs(iDifferenceY)) // we are in a diagonal line
								{
									bool bFreeQueenWalk=true;
					

									for (int m=1;m<abs(iDifferenceX)+1;m++)
										{
											int iCurrentX=i + (iDifferenceX/(abs(iDifferenceX)))*m;
											int iCurrentY=j + (iDifferenceY/(abs(iDifferenceY)))*m;
										
											if (m_Board[iCurrentX][iCurrentY].m_bHoldAUnit)
											{
												bFreeQueenWalk=false;
												break;
											}									
										}

									if (bFreeQueenWalk)
									{
										pPossibleMoves[iPossibleMoves].vFrom.x = i;
										pPossibleMoves[iPossibleMoves].vFrom.y = j;
										pPossibleMoves[iPossibleMoves].vTo.x = iTo;
										pPossibleMoves[iPossibleMoves].vTo.y = jTo;

										iPossibleMoves++;

									}
								}

								/*if (abs(iTo-i) == abs(jTo-j)) // a diagonal line
								{
									for (int m=0;m<abs(iTo-i);m++)
									{

									}

								}*/
							}

					}

				}	
		}


	if (bArmySide==ARMY_SIDE_PAST)
	{
		m_iCurrentPossibleMovesPAST = iPossibleMoves;

		if (m_iCurrentPossibleMovesPAST>0)
		return true;
	else
		return false;
	}
	else
	{
		m_iCurrentPossibleMovesFUTURE = iPossibleMoves;

		if (m_iCurrentPossibleMovesFUTURE>0)
		return true;
	else
		return false;
	}

	
}

bool CCheckersAI::IsKillMove(int fromX,int fromY,int toX, int toY)
{

	int iDifferenceX = toX-fromX;
	int iDifferenceY = toY-fromY;

		if (abs(iDifferenceX)==abs(iDifferenceY)) // a diagonal line
			if ( (m_Board[fromX][fromY].m_iType==UNIT_TYPE_QUEEN)
				|| ( (m_Board[fromX][fromY].m_iType==UNIT_TYPE_SOLDIER) && (abs(iDifferenceX)<3 ) )		)
			if (abs(iDifferenceX)>1) // not a short walk
			{
				for (int m=1;m<abs(iDifferenceX)+1;m++)
					{
						int iCurrentX=fromX + (iDifferenceX/(abs(iDifferenceX)))*m;
						int iCurrentY=fromY + (iDifferenceY/(abs(iDifferenceY)))*m;

						if (m==abs(iDifferenceX)-1)
						{
							// if there isn't a unit in the one square before last then it's not a kill move
							if (!m_Board[iCurrentX][iCurrentY].m_bHoldAUnit)
								return false;

							// if from the same army then we can't eat it...
							if (m_Board[fromX][fromY].m_bArmySide==m_Board[iCurrentX][iCurrentY].m_bArmySide)
								return false;

						}
						else
							//if (m_Board[iCurrentX][iCurrentY].m_bArmySide)
							if (m_Board[iCurrentX][iCurrentY].m_bHoldAUnit)
							return false;
					}

					return true;

			}


			return false;

}

void CCheckersAI::GetUnitsThatShouldBeBurned(bool bArmySide,sMoveAI &playerMove,MOVES_VECTOR & fillMe)
{
	/*MOVES_VECTOR shouldBeBurned;*/

	sMoveAI* pPossibleMoves;
	int      iPossibleMovesNum;

	if (bArmySide==ARMY_SIDE_PAST)
	{
		pPossibleMoves = m_CurrentPossibleMovesPAST;
		iPossibleMovesNum = m_iCurrentPossibleMovesPAST;
	}
	else
	{
		pPossibleMoves = m_CurrentPossibleMovesFUTURE;
		iPossibleMovesNum = m_iCurrentPossibleMovesFUTURE;
	}

	for (int i=0;i<iPossibleMovesNum;i++)
	{
		// if same move as player did - ignore it.
		if ( (pPossibleMoves[i].vFrom.x == playerMove.vFrom.x) &&
			(pPossibleMoves[i].vFrom.y == playerMove.vFrom.y))
			continue;

		if (m_Board[pPossibleMoves[i].vFrom.x][pPossibleMoves[i].vFrom.y].m_iType==UNIT_TYPE_SOLDIER)
		{
			// if attack move
			if ( (pPossibleMoves[i].vTo.x == pPossibleMoves[i].vFrom.x-2) || (pPossibleMoves[i].vTo.x == pPossibleMoves[i].vFrom.x+2))
			{
				// add it to the vector
				fillMe.push_back(pPossibleMoves[i]);
			}
		}
		else // handle queen
		{
			/*for (int ii=0;ii<SQUARES_NUM_X;ii++)
				for (int jj=0;jj<SQUARES_NUM_Y;jj++)
				{*/
					if (IsKillMove(pPossibleMoves[i].vFrom.x,pPossibleMoves[i].vFrom.y,pPossibleMoves[i].vTo.x,pPossibleMoves[i].vTo.y))
						fillMe.push_back(pPossibleMoves[i]);
				/*}*/

		}

	}


}

sMoveAI CCheckersAI::FakeChooseBestMove(bool bArmySide)
{
	sMoveAI result;

	sMoveAI* pPossibleMoves;
	int      iPossibleMovesNum;

	if (bArmySide==ARMY_SIDE_PAST)
	{
		pPossibleMoves = m_CurrentPossibleMovesPAST;
		iPossibleMovesNum = m_iCurrentPossibleMovesPAST;
	}
	else
	{
		pPossibleMoves = m_CurrentPossibleMovesFUTURE;
		iPossibleMovesNum = m_iCurrentPossibleMovesFUTURE;
	}

	// if no possible moves return a special non-sense move
	if (iPossibleMovesNum<1)
	{
		result.vFrom.x=0;
		result.vFrom.y=0;
		result.vTo.x=0;
		result.vTo.y=0;
		return result;
	}

	int iBestScore=-9999;
	int iCurrentScore =-9999;

	for (int i=0;i<iPossibleMovesNum;i++)
	{
		if ( (pPossibleMoves[i].vTo.x==pPossibleMoves[i].vFrom.x+1) || (pPossibleMoves[i].vTo.x==pPossibleMoves[i].vFrom.x-1)) // free walk move
		{
			if (bArmySide==ARMY_SIDE_FUTURE)
			{
				if (pPossibleMoves[i].vTo.y==0)
					iCurrentScore = 3; // becoming a queen is very good....
				else
				if (pPossibleMoves[i].vTo.y==1)
					iCurrentScore = 2; // becoming a queen is very good....
				else
				iCurrentScore = 1; // a walk move gives no points
			}
			else
			if (bArmySide==ARMY_SIDE_PAST)
			{
				if (pPossibleMoves[i].vTo.y==SQUARES_NUM_Y-1)
					iCurrentScore = 3; // becoming a queen is very good....
				else
				if (pPossibleMoves[i].vTo.y==SQUARES_NUM_Y-2)
					iCurrentScore = 2; // becoming a queen is very good....
				else
				iCurrentScore = 1; // a walk move gives no points
			}
		}
		else
		{
			if (m_Board[pPossibleMoves[i].vFrom.x][pPossibleMoves[i].vFrom.y].m_iType==UNIT_TYPE_SOLDIER)
			{
				if ( (pPossibleMoves[i].vTo.x==pPossibleMoves[i].vFrom.x+2) || (pPossibleMoves[i].vTo.x==pPossibleMoves[i].vFrom.x-2)) // kill move
					iCurrentScore = 2; // a kill move gives 1 point
			}
			else // a queen
			{
				// check if it's a kill move
				/*if ( (pPossibleMoves[i].vTo.x>pPossibleMoves[i].vFrom.x+1) || (pPossibleMoves[i].vTo.x<pPossibleMoves[i].vFrom.x-1))*/
				if (IsKillMove(pPossibleMoves[i].vFrom.x,pPossibleMoves[i].vFrom.y,pPossibleMoves[i].vTo.x,pPossibleMoves[i].vTo.y))
					iCurrentScore = 3;
				else
					iCurrentScore = 0;


			}
		}

		if (iCurrentScore>iBestScore)
		{
			iBestScore = iCurrentScore;
			result.vFrom.x = pPossibleMoves[i].vFrom.x;
			result.vFrom.y = pPossibleMoves[i].vFrom.y;
			result.vTo.x = pPossibleMoves[i].vTo.x;
			result.vTo.y = pPossibleMoves[i].vTo.y;
		}

	}


	return result;
}
