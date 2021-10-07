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
		explicit Actor3DComponent()  = default; // for cereal
		~Actor3DComponent() override = default;

		void PreGameUpdate(float dt) override{}
		void GameUpdate(float dt)    override{}
		void RenderImGui()		     override{}

		void Load(ecs::Registry* registry) override {}
	};
} // cumulonimbus::component

CEREAL_REGISTER_TYPE(cumulonimbus::component::Actor3DComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::Actor3DComponent)

