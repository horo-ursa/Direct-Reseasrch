#include "stdafx.h"
#include "RenderCube.h"
#include "VertexBuffer.h"


RenderCube::RenderCube() : RenderObj(){
	static VertexPosColor cubeVertex[] = {
		//front face 0-3
		{ Vector3(-0.5f, 0.5f, -0.5f), Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f) }, //1
		{ Vector3(0.5f, 0.5f, -0.5f), Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f) },  //2
		{ Vector3(0.5f, -0.5f, -0.5f), Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f) }, //3
		{ Vector3(-0.5f, -0.5f, -0.5f), Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f) },//4

		//back face 4-7
		{ Vector3(-0.5f, 0.5f, 0.5f), Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f) },  //5
		{ Vector3(0.5f, 0.5f, 0.5f), Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f) },   //6
		{ Vector3(0.5f, -0.5f, 0.5f), Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f) },  //7
		{ Vector3(-0.5f, -0.5f, 0.5f), Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f) }, //8

		//up face 8-11
		{ Vector3(-0.5f, 0.5f, 0.5f), Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f) }, //5
		{ Vector3(0.5f, 0.5f, 0.5f), Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f) },  //6
		{ Vector3(0.5f, 0.5f, -0.5f), Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f) }, //2
		{ Vector3(-0.5f, 0.5f, -0.5f), Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f) },//1

		//up bottom 12-15
		{ Vector3(-0.5f, -0.5f, 0.5f), Graphics::Color4(1.0f, 1.0f, 0.0f, 1.0f) },//8  12
		{ Vector3(0.5f, -0.5f, 0.5f), Graphics::Color4(1.0f, 1.0f, 0.0f, 1.0f) }, //7  13
		{ Vector3(0.5f, -0.5f, -0.5f), Graphics::Color4(1.0f, 1.0f, 0.0f, 1.0f) },//3  14
		{ Vector3(-0.5f, -0.5f, -0.5f), Graphics::Color4(1.0f, 1.0f, 0.0f, 1.0f) },//4 15

		//left face 16-19
		{ Vector3(-0.5f, 0.5f, 0.5f), Graphics::Color4(0.0f, 1.0f, 1.0f, 1.0f) },  //5
		{ Vector3(-0.5f, 0.5f, -0.5f), Graphics::Color4(0.0f, 1.0f, 1.0f, 1.0f) }, //1
		{ Vector3(-0.5f, -0.5f, -0.5f), Graphics::Color4(0.0f, 1.0f, 1.0f, 1.0f) },//4
		{ Vector3(-0.5f, -0.5f, 0.5f), Graphics::Color4(0.0f, 1.0f, 1.0f, 1.0f) }, //8

		//right 20-23
		{ Vector3(0.5f, 0.5f, -0.5f), Graphics::Color4(1.0f, 0.0f, 1.0f, 1.0f) },  //2
		{ Vector3(0.5f, 0.5f, 0.5f), Graphics::Color4(1.0f, 0.0f, 1.0f, 1.0f) },   //6
		{ Vector3(0.5f, -0.5f, 0.5f), Graphics::Color4(1.0f, 0.0f, 1.0f, 1.0f) },  //7
		{ Vector3(0.5f, -0.5f, -0.5f), Graphics::Color4(1.0f, 0.0f, 1.0f, 1.0f) }, //3

	};

	static uint16_t cubeIndex[] =
	{
		//front
		2, 1, 0,
		3, 2, 0,
		//back
		4, 5, 6,
		4, 6, 7,
		//up
		10, 9, 8,
		11, 10, 8,
		//bottom
		12, 13, 14,
		12, 14, 15,
		//left
		17, 16, 19,
		17, 19, 18,
		//right
		22, 21, 20,
		23, 22, 20,
	};

	vBuffer = new VertexBuffer(
		cubeVertex, 24, sizeof(VertexPosColor), 
		cubeIndex, 36, sizeof(uint16_t)
	);

}