#pragma once
#include <filesystem>
#include <memory>
#include <string>

#include "collision_manager.h"
#include "ecs.h"
#include "editor_manager.h"
#include "framework.h"
#include "input_manager.h"
#include "light.h"
#include "render_path.h"
#include "system.h"

class GeometricPrimitiveResource;
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
	class Scene final
	{
	public:
		explicit Scene();
		~Scene() = default;

		template<class Archive>
		void serialize(Archive&& archive);

		void Execute(Framework* framework);

		/**
		 * @brief : 新規シーンの作成
		 */
		void CreateScene() const;
		/**
		 * @brief    : シリアライズされた際に保存するファイル
		 * @param file_dir  : 保存するファイルまでのディレクトリパス
		 * @param scene_name : 保存するシーン名(※拡張子除く)
		 */
		void SaveScene(const std::string& file_dir, const std::string& scene_name);
		/**
		 * @brief : シーンのロード
		 * @param file_dir : ロードするファイルまでのディレクトリパス
		 * @param scene_name : ロードするファイル名(※拡張子を含む)
		 */
		void LoadScene(const std::string& file_dir, const std::string& scene_name);

		[[nodiscard]]
		Framework* GetFramework() const { return framework; }
		[[nodiscard]]
		editor::EditorManager* GetEditorManager() const { return editor_manager.get(); }
		[[nodiscard]]
		collision::CollisionManager* GetCollisionManager() const { return collision_manager.get(); }

	private:
		Framework*							  framework{};
		std::unique_ptr<Light>				  light{}; // Todo : 後にComponent化する予定
		std::unique_ptr<ecs::Registry>		  registry{};
		std::unique_ptr<renderer::RenderPath> render_path{};
		std::shared_ptr<system::System>		  system{};
		// Manager's
		std::shared_ptr<asset::AssetManager>		 asset_manager{};
		std::unique_ptr<editor::EditorManager>		 editor_manager{};
		std::unique_ptr<collision::CollisionManager> collision_manager{};

		void Initialize();
		void UnInitialize();
		void Update(float dt);
		void Render();
	};
} // cumulonimbus::scene


//enum class SceneType
//{
//	Title,
//	Game,
//	Tutorial,
//
//	End,
//};
//
//class Scene
//{
//protected:
//	//inline static Scene* next_scene		= nullptr;
//	inline static std::shared_ptr<Scene> next_scene { nullptr };
//	inline static Framework*			 framework  { nullptr };
//
//	SceneType current_scene = SceneType::End;
//
//	std::shared_ptr<cumulonimbus::asset::AssetManager>  asset_manager		{ nullptr };
//	std::shared_ptr<ResourceManager>					resource_manager	{ nullptr };
//	std::shared_ptr<cumulonimbus::manager::texture::TextureResourceManager> texture_resource_manager{ nullptr };
//	std::unique_ptr<cumulonimbus::ecs::Registry>		registry			{ nullptr };
//	std::unique_ptr<cumulonimbus::collision::CollisionManager> collision_manager{ nullptr };
//	std::unique_ptr<cumulonimbus::editor::EditorManager>	   editor_manager		{ nullptr };
//	std::unique_ptr<GeometricPrimitiveResource>			geom_prim_res		{ nullptr };
//	std::unique_ptr<Light>								light				{ nullptr };
//	std::unique_ptr<SoundResource>						sound_resource		{ nullptr };
//	std::unique_ptr<cumulonimbus::renderer::RenderPath> render_path			{ nullptr };
//
//	//std::unique_ptr <pad_link::PadLink>			pad_combine			= nullptr;
//
//	bool is_paused{ false };
//
//	// For cereal
//	std::string scene_name{};
//
//public:
//	void operator=(Scene&) = delete;
//
//	explicit Scene() = default;
//	virtual ~Scene() = default;
//
//	[[nodiscard]] const std::shared_ptr<Scene>& Execute(Framework* framework);
//	void SetNextScene(const std::shared_ptr<Scene>& scene) { next_scene = scene; }
//
//	[[nodiscard]] auto* GetFramework() const { return framework; }
//	[[nodiscard]] auto* GetLight() const { return light.get(); }
//	[[nodiscard]] auto* GetGeomPrimRes() const { return geom_prim_res.get(); }
//	//auto* GetPadLink()			{ return pad_combine.get(); }
//	[[nodiscard]] auto* GetSoundResourceManager() const { return sound_resource.get(); }
//	void IsPaused(bool flg) { is_paused = flg; }
//	[[nodiscard]] bool IsPaused() const { return is_paused; }
//
//
//	[[nodiscard]] const SceneType& GetCurrentScene() const { return current_scene; }
//
//	//-- ImGui --//
//	void WriteImGui() const;
//
//	//-- Serialize --//
//	/*
//	* brief    : シリアライズされた際に保存するファイル
//	* file_dir  : 保存するファイルまでのディレクトリ
//	*/
//	void SaveScene(const std::string& file_dir, const std::string& scene_name);
//
//	/*
//	 * brief : シーンのロード
//	 * filename : ファイル名(※拡張子を含む)
//	 */
//	void LoadScene(const std::string& file_dir, const std::string& scene_name);
//
//	template <class Archive>
//	void serialize(Archive&& archive);
//
//private:
//	void			Initialize();
//	void			UnInitialize();
//	void			Update(const float elapsed_time);
//	void			Render();
//	virtual void	InitializeScene() {}
//	virtual void	UnInitializeScene() = 0;
//	virtual void	UpdateScene(const float elapsed_time) = 0;
//	virtual void	RenderImGui() {}
//};
//
//class SceneManager final
//{
//public:
//	explicit SceneManager() = default;
//	~SceneManager() = default;
//
//	void Execute(Framework* framework);
//};