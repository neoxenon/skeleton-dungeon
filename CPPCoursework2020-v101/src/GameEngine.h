#pragma once
#include "BaseEngine.h"

#include "DrawingFilters.h" 

#include "Psybc5TileManager.h"
#include "ImageManager.h"

#include "AudioPlayer.h"


//This class controls transitions between states
//Also responsible for holding any data that must persist between states or be stored for later use, ie. game objects, inventory, tiles, etc.

class BaseState;
class StateMenu;
class StateCharCreate;

class PlayerObject;

class GameEngine :
	public BaseEngine
{

public:

	GameEngine();

	void setState(BaseState* state);
	BaseState* getState();

	int virtInitialise() override;
	void virtCleanUp() override;

	//GENERIC DRAW FUNCTIONS
	virtual void virtSetupBackgroundBuffer() override;
	void virtDrawStringsOnTop() override;
	void virtDrawStringsUnderneath() override;

	//OBJECTS
	virtual int virtInitialiseObjects() override;

	//CONTROLS
	virtual void virtMouseDown(int iButton, int iX, int iY) override;
	virtual void virtMouseWheel(int x, int y, int which, int timestamp) override;
	virtual void virtKeyDown(int iKeyCode) override;

	//UPDATE FUNCTIONS
	virtual void virtMainLoopPreUpdate() override;
	virtual void virtMainLoopPostUpdate() override;

	//RETURN PRIVATE POINTERS
	SolidTileManager* GetTilesSolid() { return &objTilesSolid; }
	BackgroundTileManager* GetTilesBack() { return &objTilesBack; }
	InventoryTileManager* GetTilesInv() { return &objInvTiles; }
	PlayerObject* GetPlayer() { return player; }

	PlayerObject* player;

	//Made menu state public, since any state needs to be able to get back to it
	StateMenu* stateMenu;
	StateCharCreate* stateCharCreate;

private:
	BaseState* currentState;

	AudioPlayer audio;

	SolidTileManager objTilesSolid;
	BackgroundTileManager objTilesBack;
	InventoryTileManager objInvTiles;

	//For scrolling and zooming the draw Surfaces
	FilterPointsScaling filterScaling;
	FilterPointsTranslation filterTranslation;

};