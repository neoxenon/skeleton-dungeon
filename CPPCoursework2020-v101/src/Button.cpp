#include "header.h"
#include "Button.h"

#include "StateMenu.h"
#include "StateStart.h"

Button::Button(GameEngine* pEngine, int x, int y, int width, int height, int colText, int colButton, const char* text, Font* font = NULL)
	: pEngine(pEngine), x(x), y(y), width(width), height(height), colText(colText), colButton(colButton), text(text), font(font)
{
}

Button::~Button() {}

void Button::onClick()
{
}

void Button::drawButton(bool drawBack = true)
{
	if (drawBack) pEngine->drawForegroundRectangle(x, y, x + width, y + height, colButton);
	pEngine->drawForegroundString(x+5, y+5, text, colText, font);
}

bool Button::isClicked(int xClick, int yClick)
{
	if (xClick > x&& yClick > y&& xClick < x + width && yClick < y + height) {
		return true;
	}
	return false;
}


//NEW GAME
void ButtonNewGame::onClick()
{
	pEngine->setState(pEngine->stateCharCreate);
}

//CONTINUE
void ButtonContinue::onClick()
{
	pEngine->setState(pEngine->stateStart);
}

//LEVEL EDITOR
void ButtonEditor::onClick()
{
	pEngine->setState(pEngine->stateStart);
}

//EXIT
void ButtonExit::onClick()
{
	pEngine->setExitWithCode(0);
}