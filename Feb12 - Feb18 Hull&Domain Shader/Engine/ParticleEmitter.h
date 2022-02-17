#pragma once
#include "engineMath.h"

class ParticleEmitter {
	class Particle {
	public:
		Particle(Vector3 pos) { position = pos; };
		~Particle() {};
		Vector3 position;
	};
public:
	ParticleEmitter(Vector3 pos);
	~ParticleEmitter();
	void SetRange(float x, float y, float z);
	void SetParticleNumber(int num);
	void GenerateParticle();
private:
	Vector3 emitterPosition;
	int particleNumber;
	float xRange;
	float yRange;
	float zRange;
	std::vector<Vector3> allParticles;
};