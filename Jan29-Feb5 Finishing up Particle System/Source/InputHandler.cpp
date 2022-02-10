#include "stdafx.h"
#include "InputHandler.h"
#include "game.h"

InputHandler::InputHandler(Game* game) : mGame(game) {
	camfoward = new CamFoward(game);
	cambackward = new CamBackward(game);
	camleft = new CamLeft(game);
	camright = new CamRight(game);
}

InputHandler::~InputHandler() {
	delete camfoward;
	delete cambackward;
	delete camleft;
	delete camright;
}

Command* InputHandler::handleInput() {
	if (mGame->IsKeyHeld('W')) {
		return camfoward;
	}
	if (mGame->IsKeyHeld('S')) {
		return cambackward;
	}
	if (mGame->IsKeyHeld('A')) {
		return camleft;
	}
	if (mGame->IsKeyHeld('D')) {
		return camright;
	}
	return nullptr;
}

void InputHandler::Execute() {
	Command* command = handleInput();
	if (command) {
		command->execute();
	}
}

void CamFoward::execute() {
	mGame->camera->MoveForward();
}

void CamBackward::execute() {
	mGame->camera->MoveBackward();
}

void CamLeft::execute() {
	mGame->camera->MoveLeft();
}

void CamRight::execute() {
	mGame->camera->MoveRight();
}

void MouseMove::execute() {

}

CamFoward::CamFoward(Game* game) : Command(game) {}
CamFoward::~CamFoward() {}

CamBackward::CamBackward(Game* game) : Command(game) {}
CamBackward::~CamBackward() {}

CamLeft::CamLeft(Game* game) : Command(game) {}
CamLeft::~CamLeft() {}

CamRight::CamRight(Game* game) : Command(game) {}
CamRight::~CamRight() {}

MouseMove::MouseMove(Game* game) : Command(game) {}
MouseMove::~MouseMove() {}