#include "scene_transition_component.h"

#include "cum_imgui_helper.h"
#include "locator.h"
#include "scene_loader.h"
#include "enum_state_map.h"
#include "scene.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::SceneTransitionComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::SceneTransitionComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::SceneTransitionComponent, 0);

namespace
{
	// ImGui描画時にTransitionState(enum class)から文字列一覧を取得する為に使用
	EnumStateMap<cumulonimbus::component::SceneTransitionComponent::TransitionState> transition_preset{};
}

namespace cumulonimbus::component
{
	template <class Archive>
	void SceneTransitionComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<ComponentBase>(this),
			CEREAL_NVP(transition_state),
			CEREAL_NVP(game_scene_id),
			CEREAL_NVP(title_scene_id)
		);
	}

	template <class Archive>
	void SceneTransitionComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(cereal::base_class<ComponentBase>(this),
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
		SceneTransitionComponent& scene_transition_comp = GetRegistry()->GetComponent<SceneTransitionComponent>(GetEntity());

		transition_state.AddState(TransitionState::Title_To_Game, [&scene_transition_comp](const float dt) { scene_transition_comp.TitleToGame(dt); });
		transition_state.AddState(TransitionState::Game_To_Title, [&scene_transition_comp](const float dt) { scene_transition_comp.GameToTitle(dt); });
	}

	SceneTransitionComponent::SceneTransitionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const SceneTransitionComponent& copy_comp)
		: ComponentBase{ registry,ent }
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	void SceneTransitionComponent::Initialize(ecs::Registry* registry, mapping::rename_type::Entity ent)
	{
		SceneTransitionComponent& scene_transition_comp = GetRegistry()->GetComponent<SceneTransitionComponent>(GetEntity());

		transition_state.AddState(TransitionState::Title_To_Game, [&scene_transition_comp](const float dt) { scene_transition_comp.TitleToGame(dt); });
		transition_state.AddState(TransitionState::Game_To_Title, [&scene_transition_comp](const float dt) { scene_transition_comp.GameToTitle(dt); });

		transition_state.SetState(TransitionState::Title_To_Game);
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
			auto& asset_manager = *locator::Locator::GetAssetManager();
			auto& scene_loader  = *asset_manager.GetLoader<asset::SceneLoader>();

			// transition_stateの設定
			if (std::string current_name = nameof::nameof_enum(transition_state.GetState()).data();
				helper::imgui::Combo("Pivot", current_name, transition_preset.GetStateNames()))
			{
				transition_state.SetState(transition_preset.GetStateMap().at(current_name));
			}

			if(transition_state.GetState() == TransitionState::Title_To_Game)
			{
				// Title -> Gameへの遷移
				if (ImGui::TreeNode("To Game"))
				{
					scene_loader.ImSelectableScene(asset_manager, game_scene_id);

					ImGui::TreePop();
				}
			}
			if(transition_state.GetState() == TransitionState::Game_To_Title)
			{
				ImGui::Separator();
				if (ImGui::TreeNode("To Title"))
				{
					scene_loader.ImSelectableScene(asset_manager, title_scene_id);

					ImGui::TreePop();
				}
				ImGui::Separator();
			}
		}
	}

	void SceneTransitionComponent::TitleToGame(float dt)
	{
		// Aボタンの押下状態
		const auto button_a_state = locator::Locator::GetInput()->GamePad().GetState(GamePadButton::A);

		if(button_a_state == ButtonState::Press)
		{// ゲームシーンへの切り替え
			auto& asset_manager = *locator::Locator::GetAssetManager();
			auto& scene_loader = *asset_manager.GetLoader<asset::SceneLoader>();

			// シーンアセットが存在しない場合処理を飛ばす
			if (!scene_loader.HasScene(game_scene_id))
				return;

			const auto scene_file_path = asset_manager.GetAssetSheetManager().GetAssetFilename<asset::SceneAsset>(game_scene_id);
			// シーンの展開
			locator::Locator::GetAssetManager()->GetLoader<asset::SceneLoader>()->OpenScene(*GetRegistry()->GetScene()->GetSceneManager(), scene_file_path);
		}
	}

	void SceneTransitionComponent::GameToTitle(float dt)
	{

	}

} // cumulonimbus::component