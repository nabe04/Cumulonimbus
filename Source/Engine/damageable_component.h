#pragma once
#include <functional>

#include "event.h"
#include "component_base.h"

namespace cumulonimbus::collision
{
	struct HitResult;
} // cumulonimbus::collision

namespace cumulonimbus::component
{
	/**
	 * @brief : ���݃��x��
	 */
	enum class KnockBackLevel
	{
		None = 0,
		Small,
		Medium,
		Large
	};

	struct DamageData
	{
		// �_���[�W��
		u_int						damage_amount{};
		// �m�b�N�o�b�N���
		KnockBackLevel				knock_back_level{ KnockBackLevel::None };
		// �_���[�W���̃G�t�F�N�g�A�Z�b�gID
		mapping::rename_type::UUID	damage_effect_id{};
	};

	class DamageableComponent final : public ComponentBase
	{
	public:
		explicit DamageableComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit DamageableComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const DamageableComponent& copy_comp);
		explicit DamageableComponent()  = default; // for cereal
		~DamageableComponent() override = default;

		void Load(ecs::Registry* registry) override;

		void RegistryDamageEvent(const mapping::rename_type::UUID& id, const std::function<void(const DamageData&, const collision::HitResult&)>& func);
		void OnDamaged(const mapping::rename_type::UUID& id, const DamageData& damage_data, const collision::HitResult& hit_result);

	private:
		system::Event<void, const DamageData&, const collision::HitResult&> damage_event{};
	};
} // cumulonimbus::component
