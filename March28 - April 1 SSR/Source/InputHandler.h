#pragma once	

class Game;
class Command
{
public:
	Command(Game* game) { mGame = game; };
	virtual ~Command() {};
	virtual void execute() {};
protected:
	Game* mGame;
};

class CamFoward : public Command
{
public:
	CamFoward(Game* game);
	~CamFoward();
	void execute() override;

};

class CamBackward : public Command
{
public:
	CamBackward(Game* game);
	~CamBackward();
	void execute();
};

class CamLeft : public Command
{
public:
	CamLeft(Game* game);
	~CamLeft();
	void execute();
};

class CamRight : public Command
{
public:
	CamRight(Game* game);
	~CamRight();
	void execute();
};

class MouseMove : public Command
{
public:
	MouseMove(Game* game);
	~MouseMove();
	void execute();
};

//class GameobjCommand : public Command
//{
//	GameobjCommand();
//	~GameobjCommand();
//	void execute() override;
//};

class InputHandler
{
public:
	InputHandler(Game* game);
	~InputHandler();
	Command* handleInput();
	void Execute();
private:
	Game* mGame;
	Command* camleft;
	Command* camright;
	Command* camfoward;
	Command* cambackward;
};