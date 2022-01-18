#include "collider_message_receiver_component.h"

#include "collider_message_sender_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::ColliderMessageReceiverComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::ColliderMessageReceiverComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::ColliderMessageReceiverComponent, 0)


namespace cumulonimbus::component
{
	template <class Archive>
	void ColliderMessageReceiverComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<ComponentBase>(this)
		);
	}

	template <class Archive>
	void ColliderMessageReceiverComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<ComponentBase>(this)
		);
	}

	ColliderMessageReceiverComponent::ColliderMessageReceiverComponent(ecs::Registry* registry, const mapping::rename_type::Entity ent)
		:ComponentBase{ registry,ent }
	{
	}

	ColliderMessageReceiverComponent::ColliderMessageReceiverComponent(ecs::Registry* registry, const mapping::rename_type::Entity ent, const ColliderMessageReceiverComponent& copy_comp)
		: ComponentBase{ registry,ent }
	{
	}

	void ColliderMessageReceiverComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}

	void ColliderMessageReceiverComponent::GameUpdate(float dt)
	{
	}

	void ColliderMessageReceiverComponent::RenderImGui()
	{
		if (GetRegistry()->CollapsingHeader<ColliderMessageReceiverComponent>(GetEntity(), "Collider Message Receiver"))
		{

		}
	}

	void ColliderMessageReceiverComponent::RegisterReceivedMessage(
		const collision::HitEvent hit_event,
		const mapping::rename_type::Entity ent,
		const std::function<void(ColliderMessageSenderComponent&)> callback)
	{
		// メッセージ受取時の関数登録
		// 判定時
		if (hit_event == collision::HitEvent::OnCollisionEnter)
			on_enter_received_message_event.RegistryEvent(ent, callback);
		// 判定中
		if (hit_event == collision::HitEvent::OnCollisionStay)
			on_stay_received_message_event.RegistryEvent(GetEntity(), callback);
		// 判定終了時
		if (hit_event == collision::HitEvent::OnCollisionExit)
			on_exit_received_message_event.RegistryEvent(GetEntity(), callback);
		// 判定を行っていない間
		if (hit_event == collision::HitEvent::None)
			on_none_received_message_event.RegistryEvent(GetEntity(), callback);
	}

	void ColliderMessageReceiverComponent::Receive(
		const collision::HitEvent hit_event,
		ColliderMessageSenderComponent& sender)
	{
		// 登録されたメッセージ関数を呼ぶ
		// 判定時
		if(hit_event == collision::HitEvent::OnCollisionEnter)
			on_enter_received_message_event.Invoke(GetEntity(), sender);
		// 判定中
		if (hit_event == collision::HitEvent::OnCollisionStay)
			on_stay_received_message_event.Invoke(GetEntity(), sender);
		// 判定終了時
		if (hit_event == collision::HitEvent::OnCollisionExit)
			on_exit_received_message_event.Invoke(GetEntity(), sender);
		// 判定を行っていない間
		if (hit_event == collision::HitEvent::None)
			on_none_received_message_event.Invoke(GetEntity(), sender);
	}
} // cumulonimbus::component