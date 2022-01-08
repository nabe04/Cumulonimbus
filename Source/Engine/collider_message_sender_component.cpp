#include "collider_message_sender_component.h"

#include "capsule_collison_component.h"
#include "sphere_collision_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::ColliderMessageSenderComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::ColliderMessageSenderComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::ColliderMessageSenderComponent, 0)

namespace cumulonimbus::component
{
	template <class Archive>
	void ColliderMessageSenderComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<ComponentBase>(this)
		);
	}

	template <class Archive>
	void ColliderMessageSenderComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<ComponentBase>(this)
		);
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
		auto* capsule_collider = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(GetEntity());

		// どちらのコライダーも保持していない場合処理を抜ける
		if(!sphere_collider && !capsule_collider)
			return;

		//
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

	void ColliderMessageSenderComponent::Send()
	{
		// 受け取り先のエンティティが存在しない場合処理を抜ける
		if (!GetRegistry()->GetEntities().contains(receiver_ent))
			return;

		//if(auto* )
	}
} // cumulonimbus::component
