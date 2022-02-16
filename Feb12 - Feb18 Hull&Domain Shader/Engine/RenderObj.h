#pragma once
#include <engineMath.h>
class VertexBuffer;
class Graphics;

class RenderObj
{
public:
	struct MatchPOC
	{
		Matrix4 c_modelToWorld;
	};
	struct tess {
		float tess_factor;
		Vector3 padding;
	};
	RenderObj();
	RenderObj(const VertexBuffer* vertexBuffer);
	virtual ~RenderObj();
	virtual void Draw();
	MatchPOC match;
	tess shaderData;
protected:
	const VertexBuffer* vBuffer;
	ID3D11Buffer* objectBuffer;
	ID3D11Buffer* tessBuffer;
	Graphics* graphic;
};

