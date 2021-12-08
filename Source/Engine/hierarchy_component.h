#pragma once
#include "component_base.h"

namespace cumulonimbus::ecs
{
	class Registry;
} // cumulonimbus::ecs

namespace cumulonimbus::component
{
	class HierarchyComponent final : public ComponentBase
	{
	public:
		/**
		 * @brief : �e�̃{�[���ʒu�ɃA�^�b�`���Ɏg�p����p�����[�^
		 */
		struct ParentNodeData
		{
			int			node_index{-1};
			std::string node_name{};

			template<class Archive>
			void load(Archive&& archive, uint32_t version);
			template<class Archive>
			void save(Archive&& archive, uint32_t version) const;
		};

	public:
		explicit HierarchyComponent() = default; // for cereal
		explicit HierarchyComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit HierarchyComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const HierarchyComponent& copy_comp); // �v���t�@�u����̒ǉ��p
		~HierarchyComponent() override = default;

		template<class Archive>
		void load(Archive&& archive, uint32_t  version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		void CommonUpdate(float dt)    override;
		void PreCommonUpdate(float dt) override;

		void RenderImGui() override;

		void Load(ecs::Registry* registry) override;

		void OnDeserialize(
			ecs::Registry* registry,
			const std::map<mapping::rename_type::Entity, mapping::rename_type::Entity>& connector) override;

		[[nodiscard]]
		const mapping::rename_type::Entity& GetParentEntity() const { return parent_entity; }
		[[nodiscard]]
		const mapping::rename_type::Entity& GetFirstChild() const { return first_child; }
		[[nodiscard]]
		const mapping::rename_type::Entity& GetNextSibling() const { return next_sibling; }
		[[nodiscard]]
		const mapping::rename_type::Entity& GetBackSibling() const { return back_sibling; }
		[[nodiscard]]
		const ParentNodeData& GetParentNodeData() const { return parent_node_data; }

		/**
		 * @brief : �q�K�w�̑S�ẴG���e�B�e�B�̎擾
		 */
		[[nodiscard]]
		std::vector<mapping::rename_type::Entity> GetSubHierarchies() const;
		
		/**
		 * @brief : �e�K�w�̃Z�b�g
		 * @remark : �e�K�w(parent_entity)�̃Z�b�g�Ɠ����ɏ��߂̎q�K�w�G���e�B�e�B(first_child)�A
		 *			 ���̌Z��K�w�̃G���e�B�e�B(next_sibling)�A�O�̌Z��K�w(back_sibling)�̍Đݒ�
		 *			 ���s��
		 * @param registry :
		 * @param parent_ent : �e�K�w�̃G���e�B�e�B(�w�肪�Ȃ��ꍇ�͈�ԏ�̊K�w�̐e���݂Ȃ�)
		 */
		void SetParentEntity(
			ecs::Registry* registry,
			const mapping::rename_type::Entity& parent_ent = {});

		/**
		 * @brief : �폜���ɂ��̃G���e�B�e�B�����e�q�֌W�̃G���e�B�e�B�̍Đݒ�
		 * @remark : ���̃R���|�[�l���g���폜�����O�Ɏg�p
		 */
		void OnDestroyed(ecs::Registry* registry) const;

	private:
		mapping::rename_type::Entity parent_entity{}; // �e�K�w�̃G���e�B�e�B
		mapping::rename_type::Entity first_child{};	  // ���߂̎q�K�w�G���e�B�e�B
		mapping::rename_type::Entity next_sibling{};  // ���̌Z��K�w�G���e�B�e�B
		mapping::rename_type::Entity back_sibling{};  // �O�̌Z��K�w�G���e�B�e�B
		ParentNodeData parent_node_data{};

		[[nodiscard]]
		DirectX::SimpleMath::Vector3 GetParentsScale(const mapping::rename_type::Entity& ent) const;
	};
} // cumulonimbus::component
