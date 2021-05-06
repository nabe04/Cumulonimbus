#pragma once

#include "mesh_object.h"

class BillbordComponent :public MeshObject
{
public:
	BillbordComponent(Entity* entity);

	virtual void Update(const float delta_time) override;
};