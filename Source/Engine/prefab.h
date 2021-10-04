#pragma once
#include <map>

#include "component_base.h"
#include "rename_type_mapping.h"
#include "ecs.h"


namespace cumulonimbus::asset
{
	class ComponentAssetBase
	{
	public:
		virtual component::ComponentBase* AddComponent(ecs::Registry* registry) = 0;
	};

	template<class T>
	class ComponentAsset final : public ComponentAssetBase
	{
	public:
		component::ComponentBase* AddComponent(ecs::Registry* registry) override
		{
			const auto ent = registry->CreateEntity();
			auto& comp = registry->AddComponent<T>(ent);
			comp = component_data;
			comp.SetEntity(ent);
			comp.SetRegistry(registry);

			return nullptr;
		}

	private:
		T component_data;
	};

	class Prefab final
	{
	public:
		explicit Prefab(const mapping::rename_type::Entity& ent, ecs::Registry* registry);
		explicit Prefab() = default; // for cereal

		/**
		 * @brief : Prefabの作成
		 * @remark : Hierarchy View上でEntityをPrefab化したい時に使用
		 * @param registry :
		 * @param ent : Prefab化したいEntity
		 */
		void CreatePrefab(ecs::Registry* registry, const mapping::rename_type::Entity& ent);

	private:
		std::map<mapping::rename_type::ComponentName, std::shared_ptr<ComponentAssetBase>> components;

		template<class T>
		void RegistryComponent();
	};
} // cumulonimbus::asset