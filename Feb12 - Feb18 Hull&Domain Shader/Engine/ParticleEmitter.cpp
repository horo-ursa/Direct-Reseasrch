#include "stdafx.h"
#include "ParticleEmitter.h"


ParticleEmitter::ParticleEmitter(Vector3 pos) : emitterPosition(pos) {

}

ParticleEmitter::~ParticleEmitter() {

}

void ParticleEmitter::SetRange(float x, float y, float z)
{
	xRange = x;
	yRange = y;
	zRange = z;
}

void ParticleEmitter::SetParticleNumber(int num) {
	particleNumber = num;
}

void ParticleEmitter::GenerateParticle() {
	
}