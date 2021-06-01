#pragma once

#include <cereal/cereal.hpp>
#include "mesh_object.h"

class BillbordComponent :public MeshObject
{
public:
	explicit BillbordComponent() = default; // For cereal
	explicit BillbordComponent(Entity* entity);

	virtual void Update(const float delta_time) override;

	void Save(std::string file_path) override;
	void Load(Entity* entity, std::string file_path_and_name) override;

	template<typename Archive>
	void serialize(Archive&& archive)
	{
		archive(
			cereal::base_class<MeshObject>(this)
		);
	}
};

CEREAL_REGISTER_TYPE(BillbordComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MeshObject, BillbordComponent)