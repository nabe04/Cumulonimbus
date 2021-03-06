#pragma once
#include <unordered_map>

#include "scene.h"
#include "rename_type_mapping.h"

class Framework;

namespace cumulonimbus
{
	namespace asset
	{
		class AssetManager;
	} // asset

	namespace system
	{
		class EffekseerManager;
		class WaveSystem;
	} // system

	namespace renderer
	{
		class RenderPath;
	} // renderer

	namespace collision
	{
		class CollisionManager;
	} // collision
} // cumulonimbus

namespace cumulonimbus::scene
{
	class SceneManager final
	{
	public:
		struct BeginSceneData
		{
			// シーンID
			mapping::rename_type::UUID scene_id{};
			// 「Play」ボタンを押した時のシーンファイルパス(.scene拡張子なし)
			std::string	save_scene_path{};
			// 「Play」ボタンを押した時のシーン名
			std::string	save_scene_name{};
		};
		
	public:
		explicit SceneManager(const std::shared_ptr<Window>& window);
		~SceneManager() = default;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);
		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		void Run();

		/**
		 * @brief : 新規シーンの作成
		 * @remark : 現在開かれているシーンを全て削除して
		 *			 新しいシーンを作成する
		 */
		scene::Scene& CreateNewScene();

		/**
		 * @brief : 既存シーンの読み込み
		 * @remark : 現在開かれているシーンを全て削除して
		 *			既存シーンの読み込む
		 * @param scene_id : 読み込みたいシーンID
		 * @param scene_file_path : 読み込むシーン(拡張子「.scene」)までのファイルパス
		 */
		void OpenScene(const mapping::rename_type::UUID & scene_id, const std::filesystem::path& scene_file_path);

		/**
		 * @brief : 既存シーンを読み込む候補を保存
		 * @remark : 実際の読み込み(OpenScene関数)はアップデート
		 *			 が全て終わったあとに行う
		 * @param scene_id : 読み込みたいシーンID
		 * @param scene_file_path : 読み込むシーン(拡張子「.scene」)までのファイルパス
		 */
		void SwitchScene(const mapping::rename_type::UUID& scene_id, const std::filesystem::path& scene_file_path);

		/**
		 * @brief : シーンの追加読み込み
		 * @remark : 現在開かれているシーンに追加して
		 *			 新しいシーンを作成する
		 * @remark : 初期シーン名は「NoTitled」
		 */
		void AddScene();

		/**
		 * @brief : シーンの追加読み込み
		 * @remark : 現在開かれているシーンに追加して
		 *			 新しいシーンを作成する
		 * @remark : 既存のシーンを読み込んで追加する
		 * @param scene_id : 追加するシーンのID
		 * @param scene_file_path : 読み込むシーン(拡張子「.scene」)までのファイルパス
		 */
		void AddScene(const mapping::rename_type::UUID& scene_id, const std::filesystem::path& scene_file_path);

		/**
		 * @brief : シーンの削除
		 * @remark : この関数では削除時に「保存するか」などの確認を行わないため注意
		 * @remark : 削除するIDが存在しない場合早期リターンする(例外処理などは出さない)
		 * @param scene_id : 削除するシーンID
		 */
		void DeleteScene(const mapping::rename_type::UUID& scene_id);

		/**
		 * @brief : 全シーンの削除
		 */
		void DeleteAllScene();

		//-- ゲッター --//
		[[nodiscard]]
		std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<Scene>>& GetActiveScenes()
		{
			return active_scenes;
		}
		[[nodiscard]]
		const std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<Scene>>& GetActiveScenes() const
		{
			return active_scenes;
		}
		[[nodiscard]]
		Scene& GetScene(const mapping::rename_type::UUID& scene_id)
		{
			return *active_scenes.at(scene_id).get();
		}
		[[nodiscard]]
		const Scene& GetScene(const mapping::rename_type::UUID& scene_id) const
		{
			return *active_scenes.at(scene_id).get();
		}
		[[nodiscard]]
		Framework* GetFramework() const
		{
			return framework.get();
		}
		[[nodiscard]]
		system::WaveSystem* GetWaveSystem() const
		{
			return wave_system.get();
		}
		[[nodiscard]]
		editor::EditorManager* GetEditorManager() const
		{
			return editor_manager.get();
		}
		[[nodiscard]]
		collision::CollisionManager* GetCollisionManager() const
		{
			return collision_manager.get();
		}
		[[nodiscard]]
		system::EffekseerManager* GetEffekseerManager() const
		{
			return effekseer_manager.get();
		}
		[[nodiscard]]
		bool IsInGame() const;

	private:
		// 現在開かれているシーン
		std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<Scene>> active_scenes{};
		// フレームワーク
		std::unique_ptr<Framework>			  framework{};
		// 描画処理を担う
		std::unique_ptr<renderer::RenderPath> render_path{};
		// エディター全体で統一の設定
		std::shared_ptr<system::System>		  system{};
		// ウェーブ管理
		std::unique_ptr<system::WaveSystem>   wave_system{};

		//-- Manager's --//
		// アセットシート管理用マネジャー
		std::shared_ptr<asset::AssetManager>	asset_manager{};
		// エフェクシア
		std::shared_ptr<system::EffekseerManager> effekseer_manager{};
		// エディター(GUI)管理用マネジャー
		std::unique_ptr<editor::EditorManager>	editor_manager{};
		// 当たり判定管理用マネジャー
		std::unique_ptr<collision::CollisionManager> collision_manager{};

		// 「Playボタンを押した時のシーンデータ
		std::vector<BeginSceneData> begin_scene_datum{};

		// シーン切替時の新規シーン読み込み用ID
		mapping::rename_type::UUID	next_scene_id{};
		// シーン切替時の新規シーン読み込み用ID
		std::filesystem::path		next_scene_file_path{};

		void Execute();
		void Initialize();
		void UnInitialize();
		void Update(float dt);
		void Render();

		/**
		 * @brief : ゲーム開始時処理
		 * @remark : 「Play」ボタンが押された時の処理
		 */
		void BeginGame();
		/**
		 * @brief : ゲーム終了時処理
		 * @remark : 「Play」ボタンが離された時の処理
		 */
		void EndGame();

		/**
		 * @brief : プレファブの初期作成
		 */
		void InitialCreatePrefab();
	};
} // cumulonimbus::scene