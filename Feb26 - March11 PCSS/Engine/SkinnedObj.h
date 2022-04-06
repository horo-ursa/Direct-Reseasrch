#pragma once
#include "RenderObj.h"
#include "engineMath.h"

const int matrixNum = 80;

class Graphics;
class Mesh;

class SkinnedObj : public RenderObj
{
public:
	struct SkinConstant {
		Matrix4 c_skinMatrix[matrixNum];
	};
	SkinnedObj(Mesh* mesh);
	~SkinnedObj();
	void Draw();
	SkinConstant constant;
	void BindtoConstant(std::vector<Matrix4> data);
private:
	Graphics* mGraphic;
	ID3D11Buffer* constBuffer;
};