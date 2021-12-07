#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>

#include "component_base.h"

namespace cumulonimbus::component
{
	class Actor3DComponent : public ComponentBase
	{
	public:
		explicit Actor3DComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit Actor3DComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const Actor3DComponent& copy_comp);
		explicit Actor3DComponent(mapping::component_tag::ComponentTag tag);
		explicit Actor3DComponent()  = default; // for cereal
		~Actor3DComponent() override = default;

		void PreGameUpdate(float dt) override{}
		void GameUpdate(float dt)    override{}
		void RenderImGui()		     override{}

		void Load(ecs::Registry* registry) override {}

		template<class Archive>
		void load(Archive&& archive, uint32_t version)
		{
			archive(
				cereal::base_class<ComponentBase>(this)
			);
		}

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const
		{
			archive(
				cereal::base_class<ComponentBase>(this)
			);
		}

	protected:
		/**
		 * @brief : 指定のエンティティの方向への回転
		 * @param registry : エンティティの持つレジストリ
		 * @param ent : 回転先のエンティティ
		 */
		void Rotate(ecs::Registry* registry, const mapping::rename_type::Entity& ent);
	};
} // cumulonimbus::component

CEREAL_REGISTER_TYPE(cumulonimbus::component::Actor3DComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::Actor3DComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::Actor3DComponent, 0)

