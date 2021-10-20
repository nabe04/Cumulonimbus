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

		/**
		 * @brief : シーンの名前を付けての保存
		 * @remark : 保存時に保存するパスとシーンの確認を取る
		 * @param scene : 現在のシーン
		 * @param path : 「.scene」ファイルがある親のパス
		 *				  例) ./Data/Scenes/Sample/sample.scene -> ./Data/Scenes/Sample
		 */
		void SaveAs(scene::Scene& scene, const std::filesystem::path& path);

		/**
		 * @brief : シーンの保存
		 * @remark : SaveAsとの違いはシーンが既に存在していていることを前提に
		 *			 保存を行うので、シーン保存時に名前を付けずscene_file_dirで
		 *			 指定されたパスを元の保存を行う
		 * @remark : ※注意 保存したいシーンファイルが存在しない場合例外処理が出る
		 * @param scene : 現在のシーン
		 * @param scene_file_dir : 「.scene」ファイルまでのパス
		 *							例) ./Data/Scenes/Sample/sample.scene -> ./Data/Scenes/Sample/sample.scene
		 *
		 */
		void Save(scene::Scene& scene, const std::filesystem::path& scene_file_dir);

		void CreateScene(scene::Scene& scene);
		void OpenScene(scene::Scene& scene, const std::filesystem::path& path);

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
