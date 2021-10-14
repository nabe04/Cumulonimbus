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
		explicit HierarchyComponent() = default; // for cereal
		explicit HierarchyComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit HierarchyComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const HierarchyComponent& copy_comp); // プレファブからの追加用
		//HierarchyComponent(const HierarchyComponent& other);
		//HierarchyComponent& operator=(const HierarchyComponent& other);
		~HierarchyComponent() override = default;

		template<class Archive>
		void load(Archive&& archive, uint32_t  version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		void CommonUpdate(float dt)    override;
		void PreCommonUpdate(float dt) override;

		void Load(ecs::Registry* registry) override;

		/**
		 * @brief :
		 */
		void RegistryFamily(
			ecs::Registry* registry,
			const mapping::rename_type::Entity& parent_ent,
			const mapping::rename_type::Entity& selected_ent);

		[[nodiscard]]
		const mapping::rename_type::Entity& GetParentEntity() const { return parent_entity; }
		[[nodiscard]]
		const mapping::rename_type::Entity& GetFirstChild() const { return first_child; }
		[[nodiscard]]
		const mapping::rename_type::Entity& GetNextSibling() const { return next_sibling; }
		[[nodiscard]]
		const mapping::rename_type::Entity& GetBackSibling() const { return back_sibling; }

		void SetParentEntity(const mapping::rename_type::Entity& parent) { parent_entity = parent; }
		void SetFirstChild(const mapping::rename_type::Entity& first) { first_child = first; }
		void SetNext(const mapping::rename_type::Entity& next) { next_sibling = next; }
		void SetBack(const mapping::rename_type::Entity& back) { back_sibling = back; }
	private:
		mapping::rename_type::Entity parent_entity{}; // 親階層のエンティティ
		mapping::rename_type::Entity first_child{};
		mapping::rename_type::Entity next_sibling{};
		mapping::rename_type::Entity back_sibling{};
	};
} // cumulonimbus::component
