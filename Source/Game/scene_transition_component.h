#pragma once
#include "component_base.h"
#include "state_machine.h"

namespace cumulonimbus::component
{
	class SceneTransitionComponent final : public ComponentBase
	{
	public:
		enum class TransitionState
		{
			Title_To_Game,
			Game_To_Title,

			End
		};

	public:
		using ComponentBase::ComponentBase;
		explicit SceneTransitionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit SceneTransitionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const SceneTransitionComponent& copy_comp);
		explicit SceneTransitionComponent()  = default;
		~SceneTransitionComponent() override = default;

		void Initialize(ecs::Registry* registry, mapping::rename_type::Entity ent) override;

		void Start() override;
		void GameUpdate(float dt) override;

		void RenderImGui() override;

		void Load(ecs::Registry* registry) override;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;
	private:
		StateMachine<TransitionState, void, float> transition_state{};

		mapping::rename_type::UUID game_scene_id{};  // ゲームシーンアセットID
		mapping::rename_type::UUID title_scene_id{}; // タイトルシーンアセットID

		//
		void TitleToGame(float dt);
		//
		void GameToTitle(float dt);
	};
} // cumulonimbus::component
