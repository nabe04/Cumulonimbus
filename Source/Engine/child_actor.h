#pragma once

#include <string>

#include <d3d11.h>

#include <DirectXMath.h>
#include <SimpleMath.h>

#include "mesh_object.h"

class ChildActorComponent : public MeshObject
{
private:
	EntityTag parent_tag;

	DirectX::SimpleMath::Matrix parent_matrix;
	std::string tag_node_name;

public:
	ChildActorComponent(Entity* entity, EntityTag tag, std::string node_name);
	void NewFrame(const float delta_time) override;
	void Update(const float delta_time) override;
	void RenderImGui() override;


	void Link(std::string node_name);
};