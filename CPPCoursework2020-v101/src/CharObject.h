#pragma once
#include "AnimatedObject.h"

#include "MovementPosition.h"
#include "Psybc5Engine.h"

//This class includes any functionality that is shared between ALL characters on the board (including the Player and Enemies)
//Inherits directly from AnimatedObject, as all characters are animated but not all AnimatedObjects are characters (hence the separation)

class CharObject :
	public AnimatedObject
{
public:

	CharObject(int xStart, int yStart, BaseEngine* pEngine, int width, int height, bool topleft);
	~CharObject();

	void setMovement(int iStartTime, int iEndTime, int iCurrentTime, int iStartX, int iStartY, int iEndX, int iEndY);
	virtual void move(int xmove, int ymove, int currentTime, int time);

	bool lineOfSight(const int x1, const int y1, const int x2, const int y2, const int range);

	enum State { stateIdle, stateWalk };

protected:
	MovementPosition objMovement;
	State currentState;

};

