#include "stdafx.h"
#include "PointLight.h"
#include "RenderObj.h"
#include "game.h"

PointLight::PointLight(Game* game, RenderObj* pobj) : mGame(game), Component(pobj) {
	light = mGame->AllocateLight();
	
} 

PointLight::~PointLight()
{
	mGame->FreeLight(light);
}

void PointLight::LoadProperties(Vector3 color, float innerRadius, float outerRadius)
{
	light->innerRadius = innerRadius;
	light->outerRadius = outerRadius;
	light->lightColor = color;
}

void PointLight::Update(float deltaTime)
{
	light->position = obj->position;
}