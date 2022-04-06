#pragma once
#include <engineMath.h>
#include "Component.h"

class VertexBuffer;
class Graphics;
class Material;
class Mesh;
class Shader;

class RenderObj
{
public:
	struct MatchPOC
	{
		Matrix4 c_modelToWorld;
	};
	RenderObj();
	//RenderObj(const VertexBuffer* vertexBuffer, Material* material);
	RenderObj(const Mesh* mesh);
	virtual ~RenderObj();
	virtual void Draw();
	virtual void Draw(Shader* mShader);
	virtual void Update(float deltaTime) {
		for (auto& component : mComponent) {
			component->Update(deltaTime);
		}
	};
	void AddComponent(Component* pComp);
	MatchPOC match;
	Vector3 position;

protected:
	//const VertexBuffer* vBuffer;
	ID3D11Buffer* objectBuffer;
	Graphics* graphic;
	const Mesh* mMesh;
	std::vector<Component*> mComponent;
	//Material* mMaterial;
};

