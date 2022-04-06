#include "Component.h"
#include "Lights.h"
#include "engineMath.h"
class Game;
class RenderObj;

class PointLight : public Component {
public:
	PointLight(Game* game, RenderObj* pobj);
	~PointLight();
	void Update(float deltaTime);
	void LoadProperties(Vector3 color, float innerRadius, float outerRadius);
private:
	Lights::PointLightData* light;
	Game* mGame;
};