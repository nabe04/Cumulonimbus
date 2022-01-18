#pragma once
#include "component_base.h"
#include "collision_component.h"
#include "event.h"

namespace cumulonimbus::component
{
	class ColliderMessageSenderComponent;
} // cumulonimbus::component

namespace cumulonimbus::component
{
	class ColliderMessageReceiverComponent final : public ComponentBase
	{
	public:
		using ComponentBase::ComponentBase;

		explicit ColliderMessageReceiverComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit ColliderMessageReceiverComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const ColliderMessageReceiverComponent& copy_comp);
		explicit ColliderMessageReceiverComponent() = default; // for cereal
		~ColliderMessageReceiverComponent() override = default;

		template<class Archive>
		void load(Archive && archive, uint32_t version);

		template<class Archive>
		void save(Archive && archive, uint32_t version) const;

		void Load(ecs::Registry * registry) override;

		void GameUpdate(float dt) override;

		void RenderImGui() override;

		void RegisterReceivedMessage(collision::HitEvent hit_event, mapping::rename_type::Entity ent, std::function<void(ColliderMessageSenderComponent&)> callback);

		// Senderから送信されたメッセージの受け取り処理
		void Receive(collision::HitEvent hit_event, ColliderMessageSenderComponent& sender);

	private:
		bool is_received{ false };
		// コリジョン判定時のメッセージ
		system::Event<void, ColliderMessageSenderComponent&> on_enter_received_message_event{};
		// コリジョン判定中のメッセージ
		system::Event<void, ColliderMessageSenderComponent&> on_stay_received_message_event{};
		// コリジョン判定終了時のメッセージ
		system::Event<void, ColliderMessageSenderComponent&> on_exit_received_message_event{};
		// コリジョン判定を行っていない間のメッセージ
		system::Event<void, ColliderMessageSenderComponent&> on_none_received_message_event{};
	};
} // cumulonimbus::component
