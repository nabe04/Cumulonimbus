#pragma once
#include "component_base.h"
#include "collision_component.h"

namespace cumulonimbus::component
{
	class ColliderMessageSenderComponent final : public ComponentBase
	{
	public:
		using ComponentBase::ComponentBase;
		explicit ColliderMessageSenderComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit ColliderMessageSenderComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const ColliderMessageSenderComponent& copy_comp);
		explicit ColliderMessageSenderComponent()  = default; // for cereal
		~ColliderMessageSenderComponent() override = default;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		void Load(ecs::Registry* registry) override;

		void GameUpdate(float dt) override;

		void RenderImGui() override;

		[[nodiscard]]
		collision::CollisionTag GetCollisionTag() const
		{
			return collision_tag;
		}

		[[nodiscard]]
		const collision::HitResult& GetHitResult() const
		{
			return hit_result;
		}
	private:
		// �󂯎���̃G���e�B�e�B
		mapping::rename_type::Entity receiver_ent{};
		//
		collision::CollisionTag collision_tag{};
		collision::HitResult hit_result{};

		/**
		 * @brief : receiver_ent�ւ̃��b�Z�[�W���M
		 */
		void Send(collision::HitEvent hit_event);
	};

} // cumulonimbus::component
