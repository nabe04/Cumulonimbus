#include "scene_transition_component.h"
#include "locator.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::SceneTransitionComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::SceneTransitionComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::SceneTransitionComponent, 0);

namespace cumulonimbus::component
{
	template <class Archive>
	void SceneTransitionComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			CEREAL_NVP(transition_state),
			CEREAL_NVP(game_scene_id),
			CEREAL_NVP(title_scene_id)
		);
	}

	template <class Archive>
	void SceneTransitionComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			CEREAL_NVP(transition_state),
			CEREAL_NVP(game_scene_id),
			CEREAL_NVP(title_scene_id)
		);
	}

	void SceneTransitionComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}

	SceneTransitionComponent::SceneTransitionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		:ComponentBase{ registry,ent }
	{
	}

	SceneTransitionComponent::SceneTransitionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const SceneTransitionComponent& copy_comp)
		: ComponentBase{ registry,ent }
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	void SceneTransitionComponent::Start()
	{
		SceneTransitionComponent& scene_transition_comp = GetRegistry()->GetComponent<SceneTransitionComponent>(GetEntity());

		transition_state.AddState(TransitionState::Title_To_Game, [&scene_transition_comp](const float dt) { scene_transition_comp.TitleToGame(dt); });
		transition_state.AddState(TransitionState::Game_To_Title, [&scene_transition_comp](const float dt) { scene_transition_comp.GameToTitle(dt); });

		transition_state.SetState(TransitionState::Title_To_Game);
	}

	void SceneTransitionComponent::GameUpdate(const float dt)
	{
		transition_state.Update(dt);
	}

	void SceneTransitionComponent::RenderImGui()
	{
		if(GetRegistry()->CollapsingHeader<SceneTransitionComponent>(GetEntity(),"Scene Transition"))
		{
			// Title -> Gameへの遷移
			if(ImGui::TreeNode("To Game"))
			{



				ImGui::TreePop();
			}
			ImGui::Separator();
			if(ImGui::TreeNode("To Title"))
			{

				ImGui::TreePop();
			}

		}
	}

	void SceneTransitionComponent::TitleToGame(float dt)
	{
		// Aボタンの押下状態
		const auto button_a_state = locator::Locator::GetInput()->GamePad().GetState(GamePadButton::A);

		if(button_a_state == ButtonState::Press)
		{// ゲームシーンへの切り替え

		}

	}

	void SceneTransitionComponent::GameToTitle(float dt)
	{

	}

} // cumulonimbus::component