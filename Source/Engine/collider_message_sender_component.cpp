#include "collider_message_sender_component.h"

#include "capsule_collison_component.h"
#include "collider_message_receiver_component.h"
#include "hierarchy_component.h"
#include "sphere_collision_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::ColliderMessageSenderComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::ColliderMessageSenderComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::ColliderMessageSenderComponent, 1)

namespace cumulonimbus::component
{
	template <class Archive>
	void ColliderMessageSenderComponent::load(Archive&& archive, const uint32_t version)
	{
		archive(
			cereal::base_class<ComponentBase>(this)
		);

		if(version == 1)
		{
			archive(
				CEREAL_NVP(receiver_ent)
			);
		}
	}

	template <class Archive>
	void ColliderMessageSenderComponent::save(Archive&& archive, const uint32_t version) const
	{
		archive(
			cereal::base_class<ComponentBase>(this)
		);

		if (version == 1)
		{
			archive(
				CEREAL_NVP(receiver_ent)
			);
		}
	}

	ColliderMessageSenderComponent::ColliderMessageSenderComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		:ComponentBase{ registry,ent }
	{
	}

	ColliderMessageSenderComponent::ColliderMessageSenderComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const ColliderMessageSenderComponent& copy_comp)
		: ComponentBase{ registry,ent }
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	void ColliderMessageSenderComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}

	void ColliderMessageSenderComponent::GameUpdate(float dt)
	{
		auto* sphere_collider  = GetRegistry()->TryGetComponent<SphereCollisionComponent>(GetEntity());

		if(!sphere_collider)
			return;

		for(const auto& sphere : sphere_collider->GetSpheres() | std::views::values)
		{
			for (const auto& hit_result : sphere.hit_results | std::views::values)
			{
				if (!hit_result.is_hit)
					continue;

				Send();
				return;
			}
		}

		auto* capsule_collider = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(GetEntity());

		if (!capsule_collider)
			return;

		for(const auto& capsule : capsule_collider->GetCapsules() | std::views::values)
		{
			for(const auto& hit_result : capsule.hit_results | std::views::values)
			{
				if(!hit_result.is_hit)
					continue;;

				Send();
				return;
			}
		}

	}

	void ColliderMessageSenderComponent::RenderImGui()
	{
		if (GetRegistry()->CollapsingHeader<ColliderMessageSenderComponent>(GetEntity(), "Collider Message Sender"))
		{
			if (ImGui::BeginCombo("Receiver", GetRegistry()->GetName(receiver_ent).c_str()))
			{
				for (const auto& parent : GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).GetParents())
				{
					const bool is_selected = (parent == receiver_ent);
					if (ImGui::Selectable(GetRegistry()->GetName(parent).c_str(), is_selected, 0))
					{
						receiver_ent = parent;
					}
					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		}
	}

	void ColliderMessageSenderComponent::Send()
	{
		// 受け取り先のエンティティが存在しない場合処理を抜ける
		if (!GetRegistry()->GetEntities().contains(receiver_ent))
			return;

		if (auto* receiver = GetRegistry()->TryGetComponent<ColliderMessageReceiverComponent>(receiver_ent);
			receiver)
		{
			receiver->Receive(*this);
		}
	}
} // cumulonimbus::component
