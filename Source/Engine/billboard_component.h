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
		explicit BillboardComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const BillboardComponent& copy_comp); // for prefab
		explicit BillboardComponent(); // for cereal & Inspector Viewè„Ç≈ÇÃAddComponentóp
		~BillboardComponent() override  = default;

		void CommonUpdate(float dt) override;
		void PostCommonUpdate(float dt) override;

		void SceneUpdate(float dt) override;

		void GameUpdate(float dt)  override;

		template<class Archive>
		void load(Archive&& archive, uint32_t  version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		[[nodiscard]]
		const EnumStateMap<render::RenderingTarget>& GetRenderingTarget() const { return rendering_target; }

		void SetRenderingTarget(const render::RenderingTarget target) { rendering_target.SetState(target); }
	private:
		EnumStateMap<render::RenderingTarget> rendering_target{};
	};
} // cumulonimbus::component
