#include "stdafx.h"
#include "InputHandler.h"
#include "game.h"

InputHandler::InputHandler(Game* game) : mGame(game){
	camfoward = new CamFoward(game);
	cambackward = new CamBackward(game);
	camleft = new CamLeft(game);
	camright = new CamRight(game);
	camPitchUp = new CamPitchUp(game);
	camPitchDown = new CamPitchDown(game);
	camYawUp = new CamYawUp(game);
	camYawDown = new CamYawDown(game);
}

InputHandler::~InputHandler() {
	delete camfoward;
	delete cambackward;
	delete camleft;
	delete camright;
	delete camPitchUp;
	delete camPitchDown;
	delete camYawUp;
	delete camYawDown;
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
	if (mGame->IsKeyHeld(VK_UP)) {
		return camYawUp;
	}
	if (mGame->IsKeyHeld(VK_DOWN)) {
		return camYawDown;
	}
	if (mGame->IsKeyHeld(VK_LEFT)) {
		return camPitchDown;
	}
	if (mGame->IsKeyHeld(VK_RIGHT)) {
		return camPitchUp;
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

void CamYawUp::execute() {
	mGame->camera->RotatePitchUp();
}

void CamYawDown::execute() {
	mGame->camera->RotatePitchDown();
}

void CamPitchUp::execute() {
	mGame->camera->RotateYawUp();
}

void CamPitchDown::execute() {
	mGame->camera->RotateYawDown();
}

void MouseMove::execute(){

}

CamFoward::CamFoward(Game* game): Command(game){}
CamFoward::~CamFoward() {}

CamBackward::CamBackward(Game* game) : Command(game) {}
CamBackward::~CamBackward() {}

CamLeft::CamLeft(Game* game) : Command(game) {}
CamLeft::~CamLeft() {}

CamRight::CamRight(Game* game) : Command(game) {}
CamRight::~CamRight() {}

MouseMove::MouseMove(Game* game) : Command(game){}
MouseMove::~MouseMove(){}

CamYawUp::CamYawUp(Game* game) : Command(game) {}
CamYawUp::~CamYawUp() {}

CamYawDown::CamYawDown(Game* game) : Command(game) {}
CamYawDown::~CamYawDown() {}

CamPitchUp::CamPitchUp(Game* game) : Command(game) {}
CamPitchUp::~CamPitchUp() {}

CamPitchDown::CamPitchDown(Game* game) : Command(game) {}
CamPitchDown::~CamPitchDown() {}