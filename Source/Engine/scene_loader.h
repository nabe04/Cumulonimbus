#pragma once
#include <map>
#include "loader.h"

namespace cumulonimbus::scene
{
	class Scene;
} // cumulonimbus::scene

namespace cumulonimbus::asset
{
	struct SceneAsset
	{
		std::filesystem::path scene_file_path; // 「.scene」ファイルまでのパス
	};

	class SceneLoader final : public Loader
	{
	public:
		explicit SceneLoader()  = default;
		~SceneLoader() override = default;

		void Load(AssetManager& asset_manager, const std::filesystem::path& path) override;
		void Load(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) override;
		void Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id) override;

		void Save(scene::Scene& scene, const std::filesystem::path& path);

		void OpenScene(scene::Scene& scene, const std::filesystem::path& path);
		//void CreateScene()

		void Delete(AssetManager& asset_manager, const std::filesystem::path& path) override;
		void Delete(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id) override;

		bool Supported(std::filesystem::path extension) override;

	private:
		std::map<mapping::rename_type::UUID, SceneAsset> scenes{};

		mapping::rename_type::UUID Convert(
			AssetManager& asset_manager,
			const std::filesystem::path& from,
			const std::filesystem::path& to) override;

		/**
		 * @brief : シーンのフォルダと対応するscenesのIDの削除
		 * @param scene_id : シーンID(UUID)
		 * @param delete_path : 「.scene」までのファイルパス
		 */
		void DeleteScene(
			const mapping::rename_type::UUID& scene_id,
			const std::filesystem::path& delete_path);
	};
} // cumulonimbus::asset
