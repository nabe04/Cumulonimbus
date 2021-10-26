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
		explicit SceneManager(const std::shared_ptr<Window>& window);
		~SceneManager() = default;

		[[nodiscard]]
		Framework* GetFramework() const
		{
			return framework.get();
		}
		[[nodiscard]]
		editor::EditorManager* GetEditorManager() const
		{
			return editor_manager.get();
		}
		[[nodiscard]]
		Scene* GetScene(const mapping::rename_type::UUID& scene_id)
		{
			return active_scenes.at(scene_id).get();
		}

		void Run();

	private:
		std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<Scene>> active_scenes{};
		std::unique_ptr<Framework>			  framework{};
		std::unique_ptr<renderer::RenderPath> render_path{};
		std::shared_ptr<system::System>		  system{};
		// Manager's
		std::shared_ptr<asset::AssetManager>	asset_manager{};
		std::unique_ptr<editor::EditorManager>	editor_manager{};

		void Execute();
		void Initialize();
		void UnInitialize();
		void Update(float dt);
		void Render();

		// Todo : スクリプト機能が出来れば消す
		/**
		 * @brief : プレファブの初期作成
		 */
		void InitialCreatePrefab() const;
	};
} // cumulonimbus::scene