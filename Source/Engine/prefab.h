#pragma once
#include <map>

#include "component_base.h"
#include "rename_type_mapping.h"

namespace cumulonimbus::ecs
{
	class Registry;
	class ComponentAssetBase;
} // cumulonimbus::ecs

namespace cumulonimbus::asset
{
	class Prefab final
	{
	public:
		explicit Prefab();
		~Prefab() = default;

		/**
		 * @brief : Prefab�̍쐬
		 * @remark : Hierarchy View���Entity��Prefab�����������Ɏg�p
		 * @param registry :
		 * @param ent : Prefab��������Entity
		 */
		void CreatePrefab(ecs::Registry* registry, const mapping::rename_type::Entity& ent);

		[[nodiscard]]
		std::map<mapping::rename_type::ComponentName, std::shared_ptr<ecs::ComponentAssetBase>>& GetComponentsAssets()
		{
			return component_assets;
		}

	private:
		std::map<mapping::rename_type::ComponentName, std::shared_ptr<ecs::ComponentAssetBase>> component_assets;

		template<class T>
		void RegistryComponent();
	};
} // cumulonimbus::asset