#pragma once
#include <map>
#include <set>
#include <filesystem>

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

		template<class Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(components_name)
			);
		}

		/**
		 * @brief : Prefab�̍쐬
		 * @remark : Hierarchy View���Entity��Prefab�����������Ɏg�p
		 * @param registry :
		 * @param ent : Prefab��������Entity
		 * @param path : �ۑ�����v���t�@�u�̃p�X(���t�@�C���p�X + �t�@�C���� + �t�@�C���g���q)
		 */
		void CreatePrefab(
			ecs::Registry* registry, const mapping::rename_type::Entity& ent,
			const std::filesystem::path& path);

		void AddComponent(ecs::Registry* registry, const mapping::rename_type::Entity& ent);

		void Save(const std::filesystem::path& path);
		void Load(const std::filesystem::path& path);

		[[nodiscard]]
		std::map<mapping::rename_type::ComponentName, std::shared_ptr<ecs::ComponentAssetBase>>& GetComponentsAssets()
		{
			return component_assets;
		}

	private:
		std::map<mapping::rename_type::ComponentName, std::shared_ptr<ecs::ComponentAssetBase>> component_assets;
		std::set<mapping::rename_type::ComponentName> components_name;

		template<class T>
		void RegistryComponent();
	};
} // cumulonimbus::asset