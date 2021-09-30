#pragma once
#include "sprite_component.h"
#include "rename_type_mapping.h"
#include "enum_state_map.h"

namespace cumulonimbus::render
{
	enum class RenderingTarget
	{
		SceneOnly,
		GameOnly,
		Both,

		End
	};
} // cumulonimbus::render

namespace cumulonimbus::component
{
	class BillboardComponent final : public SpriteComponent
	{
	public:
		using SpriteComponent::SpriteComponent;
		explicit BillboardComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit BillboardComponent(); // for cereal
		~BillboardComponent() = default;

		void SceneUpdate(float dt) override;
		void GameUpdate(float dt)  override;

		[[nodiscard]]
		const EnumStateMap<render::RenderingTarget>& GetRenderingTarget() const { return rendering_target; }

	private:
		EnumStateMap<render::RenderingTarget> rendering_target{};

	};
} // cumulonimbus::component
