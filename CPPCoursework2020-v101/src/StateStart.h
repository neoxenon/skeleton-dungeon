#pragma once
#include "BaseState.h"
#include "GameEngine.h"

class StateStart :
	public BaseState
{
public:
	StateStart(GameEngine* pEngine);

	virtual void onStateEnter();
	virtual void onStateExit();

};

