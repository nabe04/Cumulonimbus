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
		 * @brief : 親のボーン位置にアタッチ時に使用するパラメータ
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
		explicit HierarchyComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const HierarchyComponent& copy_comp); // プレファブからの追加用
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
		 * @brief : 子階層の全てのエンティティの取得
		 */
		[[nodiscard]]
		std::vector<mapping::rename_type::Entity> GetSubHierarchies() const;
		
		/**
		 * @brief : 親階層のセット
		 * @remark : 親階層(parent_entity)のセットと同時に初めの子階層エンティティ(first_child)、
		 *			 次の兄弟階層のエンティティ(next_sibling)、前の兄弟階層(back_sibling)の再設定
		 *			 も行う
		 * @param registry :
		 * @param parent_ent : 親階層のエンティティ(指定がない場合は一番上の階層の親をみなす)
		 */
		void SetParentEntity(
			ecs::Registry* registry,
			const mapping::rename_type::Entity& parent_ent = {});

		/**
		 * @brief : 削除時にこのエンティティを持つ親子関係のエンティティの再設定
		 * @remark : このコンポーネントが削除される前に使用
		 */
		void OnDestroyed(ecs::Registry* registry) const;

	private:
		mapping::rename_type::Entity parent_entity{}; // 親階層のエンティティ
		mapping::rename_type::Entity first_child{};	  // 初めの子階層エンティティ
		mapping::rename_type::Entity next_sibling{};  // 次の兄弟階層エンティティ
		mapping::rename_type::Entity back_sibling{};  // 前の兄弟階層エンティティ
		ParentNodeData parent_node_data{};

		[[nodiscard]]
		DirectX::SimpleMath::Vector3 GetParentsScale(const mapping::rename_type::Entity& ent) const;
	};
} // cumulonimbus::component
