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

		// Sender���瑗�M���ꂽ���b�Z�[�W�̎󂯎�菈��
		void Receive(collision::HitEvent hit_event, ColliderMessageSenderComponent& sender);

	private:
		bool is_received{ false };
		// �R���W�������莞�̃��b�Z�[�W
		system::Event<void, ColliderMessageSenderComponent&> on_enter_received_message_event{};
		// �R���W�������蒆�̃��b�Z�[�W
		system::Event<void, ColliderMessageSenderComponent&> on_stay_received_message_event{};
		// �R���W��������I�����̃��b�Z�[�W
		system::Event<void, ColliderMessageSenderComponent&> on_exit_received_message_event{};
		// �R���W����������s���Ă��Ȃ��Ԃ̃��b�Z�[�W
		system::Event<void, ColliderMessageSenderComponent&> on_none_received_message_event{};
	};
} // cumulonimbus::component
