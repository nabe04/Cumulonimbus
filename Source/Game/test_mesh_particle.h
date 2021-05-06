#pragma once
#include <vector>
#include "mesh_particle.h"

class TestMeshParticle final :public MeshParticle
{
private:
	std::vector<float> velocity;

public:
	TestMeshParticle(Entity* entity)
		:MeshParticle{ entity } {}

	void NewFrame(const float delta_time) override;
	void Update(const float delta_time) override;
	void RenderImGui() override;
};