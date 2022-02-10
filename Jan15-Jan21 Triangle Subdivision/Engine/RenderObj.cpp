#include "stdafx.h"
#include "RenderObj.h"
#include "VertexBuffer.h"

RenderObj::RenderObj(const VertexBuffer* vertexBuffer) : vBuffer(vertexBuffer){

}

RenderObj::~RenderObj() {
	delete vBuffer;
}

void RenderObj::Draw() {
	vBuffer->Draw();
}