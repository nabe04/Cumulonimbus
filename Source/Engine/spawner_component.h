#pragma once
#include "component_base.h"

namespace cumulonimbus::component
{
	class SpawnerComponent : public ComponentBase
	{
	public:
		using ComponentBase::ComponentBase;
		explicit SpawnerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit SpawnerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const SpawnerComponent& copy_comp);
		explicit SpawnerComponent() = default;
		~SpawnerComponent() override = default;

		void Load(ecs::Registry* registry) override;

		void SceneUpdate(float dt) override;

		void RenderImGui() override;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);
		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		/**
		 * @brief : �A�Z�b�g�̐����֐�
		 * @remark : �����������^�C�~���O�ň�x�����Ă�
		 * @param is_first : ���߂̐�����(true -> �����p�����[�^�����Z�b�g�����)
		 * @param pos : �����ʒu
		 */
		virtual void Spawn(bool is_first ,const DirectX::SimpleMath::Vector3& pos = {}, const DirectX::SimpleMath::Vector3& direction = { 0,0,1 });
	protected:
		// ��������A�Z�b�gID
		mapping::rename_type::UUID asset_id{};
		// ��������Ă��邩
		bool is_spawn{ false };
		// ������
		int spawn_count{ 1 };
		// ���݂̐�����
		int current_count{};
		// �������ԊԊu
		float spawn_interval_time{};
		// �o�ߎ���(�g�p�͎��R)
		float elapsed_time{};
		// �������ԊԊu
		float spawn_interval_distance{};
		// �ŏ��̐����ʒu
		DirectX::SimpleMath::Vector3 root_pos{};
		// ��������
		DirectX::SimpleMath::Vector3 direction{ 0,0,1 };

		/**
		 * @brief : �uSpawn�v�{�^����ImGui��Ŏ���
		 * @return : �{�^������������Spawn�֐�������
		 */
		void ImSpawnButton();
	};
} // cumulonimbus::component
