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
		void serialize(Archive&& archive);

		void CommonUpdate(float dt)    override;
		void PreCommonUpdate(float dt) override;

		void Load(ecs::Registry* registry) override;

		[[nodiscard]]
		const mapping::rename_type::Entity& GetParentEntity() const { return parent_entity; }

		void SetParentEntity(const mapping::rename_type::Entity& ent) { parent_entity = ent; }
	private:
		mapping::rename_type::Entity parent_entity{}; // 親階層のエンティティ
	};
} // cumulonimbus::component
