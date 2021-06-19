#pragma once

#include <string>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include <cereal/cereal.hpp>

#include "component_base.h"
#include "ecs.h"
#include "mesh_object.h"
#include "enum_state_map.h"

namespace cumulonimbus::component
{
	class ChildActorComponent : public MeshObjectComponent
	{
	public:
		explicit ChildActorComponent() = default;
		explicit ChildActorComponent(ecs::Registry* registry, mapping::rename_type::Entity ent,
									 EntityTag tag, std::string node_name);
		void NewFrame(float delta_time) override;
		void Update(float delta_time) override;
		void RenderImGui() override;

		void Link(std::string node_name);

		void Save(const std::string& file_path) override;
		void Load(const std::string& file_path_and_name) override;

		template<typename Archive>
		void serialize(Archive&& archive)
		{
			archive(
				cereal::base_class<MeshObjectComponent>(this),
				CEREAL_NVP(parent_tag),
				CEREAL_NVP(parent_matrix),
				CEREAL_NVP(tag_node_name)
			);
		}
	private:
		EnumStateMap<EntityTag> parent_tag{ EntityTag::Default };

		DirectX::SimpleMath::Matrix parent_matrix;
		std::string tag_node_name;
	};
}

CEREAL_REGISTER_TYPE(cumulonimbus::component::ChildActorComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::MeshObjectComponent, cumulonimbus::component::ChildActorComponent)
