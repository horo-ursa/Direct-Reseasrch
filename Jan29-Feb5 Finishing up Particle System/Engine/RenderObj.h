#pragma once
#include "engineMath.h"

class VertexBuffer;
class Graphics;
class RenderObj
{
public:
	struct MatchPOC
	{
		Matrix4 c_modelToWorld;
	};
	RenderObj();
	RenderObj(const VertexBuffer* vertexBuffer);
	virtual ~RenderObj();
	virtual void Draw();
	MatchPOC match;

protected:
	const VertexBuffer* vBuffer;
	ID3D11Buffer* constBuffer;
	Graphics* mGraphics;
};