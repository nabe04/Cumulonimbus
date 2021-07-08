#pragma once
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>

#include "component_base.h"
#include "state_machine.h"

namespace cumulonimbus::component
{
	class PlayerComponent final : public ComponentBase
	{
	private:
		// プレイヤーの状態を表す
		enum class PlayerState
		{
			Stay,
			Run,
			Evasion,
			Attack01,
			Attack02,
			Attack03,
			Damage,
			Counter,
			Death,

			End
		};

		// FBXが持つアニメーションデータ
		enum class AnimationState
		{

		};

	public:
		explicit PlayerComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent);
		explicit PlayerComponent() = default; // for cereal
		~PlayerComponent() override = default;

		void NewFrame(float dt) override;
		void Update(float dt)	override;
		void RenderImGui()		override;

		void Save(const std::string& file_path) override;
		void Load(const std::string& file_path_and_name) override;
	private:
		StateMachine<PlayerState, void, const float> player_state{};

		// StateMachineで管理するプレイヤーの状態関数
		void Stay(float dt);
		void Run(float dt);
		void Evasinon(float dt);
		void Attack01(float dt);
		void Attack02(float dt);
		void Attack03(float dt);
		void Damage(float dt);
		void Counter(float dt);
		void Death(float dt);
	};
} // cumulonimbus::component

CEREAL_REGISTER_TYPE(cumulonimbus::component::PlayerComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::PlayerComponent)
