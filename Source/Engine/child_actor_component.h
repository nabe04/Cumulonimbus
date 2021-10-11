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
#include "rename_type_mapping.h"

namespace cumulonimbus::component
{
	class ChildActorComponent : public MeshObjectComponent
	{
	public:
		explicit ChildActorComponent() = default;
		explicit ChildActorComponent(ecs::Registry* registry, mapping::rename_type::Entity ent,
									 EntityTag tag, std::string node_name);
		void PreGameUpdate(float delta_time) override;
		void GameUpdate(float delta_time) override;
		void RenderImGui() override;

		void Link(std::string node_name);

		void Load(ecs::Registry* registry) override;

		[[nodiscard]]
		const mapping::rename_type::Entity& GetParentEntity() const { return parent_ent; }

		template<typename Archive>
		void serialize(Archive&& archive)
		{
			archive(
				cereal::base_class<MeshObjectComponent>(this),
				CEREAL_NVP(parent_matrix),
				CEREAL_NVP(tag_node_name)
			);
		}
	private:
		mapping::rename_type::Entity parent_ent{}; // 親のエンティティ
		//EnumStateMap<EntityTag> parent_tag{ EntityTag::Default };

		DirectX::SimpleMath::Matrix parent_matrix;
		std::string tag_node_name;
	};
} // cumulonimbus::component

CEREAL_REGISTER_TYPE(cumulonimbus::component::ChildActorComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::MeshObjectComponent, cumulonimbus::component::ChildActorComponent)
