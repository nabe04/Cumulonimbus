#pragma once
#include <filesystem>
#include <functional>
#include <map>

#include "rename_type_mapping.h"

namespace cumulonimbus
{
	namespace ecs
	{
		class Registry;
	} // ecs

	namespace camera
	{
		class CameraTexture;
	} // camera

	namespace collision
	{
		class CollisionPrimitiveAsset;
	} // collision

	namespace graphics
	{
		class SkyBox;
	} // graphics

	namespace system
	{
		class Time;
		class PostEffectManager;
	} // system

} // cumulonimbus

namespace cumulonimbus::system
{
	class System final
	{
	public:
		explicit System();
		~System() = default;

		template<class Archive>
		void load(Archive&& archive, std::uint32_t version);


		template<class Archive>
		void save(Archive&& archive, std::uint32_t version) const;

		/**
		 * @brief : 「.system」ファイルのロード
		 * @param load_path : 「.system」ファイルまでのパス(※拡張子(「.sys」)含む)
		 *					   (パスを指定しない場合デフォルトの
		 *					   パスからロードされる(./Data/System/system.sys))
		 */
		void Load(const std::filesystem::path& load_path = {""});

		/**
		 * @brief : 「.system」ファイルのセーブ
		 * @param save_path : 「.system」ファイルまでのパス(※拡張子(「.sys」)含む)
		 *					   (パスを指定しない場合デフォルトの
		 *					   パスに保存される(./Data/System/system.sys))
		 */
		void Save(const std::filesystem::path& save_path = { "" });

		void Update(float dt);

		/**
		 * @brief : render_functionsに登録された関数の実行
		 */
		void Render(ecs::Registry* registry);

		/**
		 * @brief : プロジェクト共通の設定の更新関数の登録
		 */
		void RegisterUpdateFunction(const mapping::rename_type::Hash& hash, const  std::function<void(float dt)>& update_func);

		/**
		 * @brief : プロジェクト共通の設定のImGui描画関数の登録
		 * @remark : ImGui上での設定を目的としているため
		 *           ImGui用の関数を登録する
		 */
		void RegisterRenderFunction(const mapping::rename_type::Hash& hash, const std::function<void(ecs::Registry* registry)>& render_func);

		[[nodiscard]]
		camera::CameraTexture& GetCameraTexture() const;
 		[[nodiscard]]
		graphics::SkyBox& GetSkyBox() const;
		[[nodiscard]]
		system::Time& GetTime() const;
		[[nodiscard]]
		system::PostEffectManager& GetPostEffectManager() const;
 		[[nodiscard]]
		collision::CollisionPrimitiveAsset& GetCollisionPrimitive() const;
		[[nodiscard]]
		const std::string& GetCurrentScenePath() const { return current_scene_path; }
		[[nodiscard]]
		const std::string& GetDefaultScenePath() const { return default_scene_path; }

		void SetCurrentScenePath(const std::string& scene_path)
		{
			current_scene_path = scene_path;
		}
	private:
		// 登録された方のUpdate関数の保持
		std::map<mapping::rename_type::Hash, std::function<void(float dt)>> update_functions{};
		// 登録された型のRender関数の保持
		std::map<mapping::rename_type::Hash, std::function<void(ecs::Registry* registry)>> render_functions{};

		std::unique_ptr<camera::CameraTexture>				camera_texture{};
		std::unique_ptr<graphics::SkyBox>					sky_box{};
		std::unique_ptr<system::Time>						time{};
		std::unique_ptr<system::PostEffectManager>			post_effect_manager{};
		std::unique_ptr<collision::CollisionPrimitiveAsset> collision_primitive{};

		std::string current_scene_path{}; // 現在開かれているシーン(cerealがstd::filesystem未対応のためstd::stringにしている)
		std::string default_scene_path{}; // ウィンドウ作成時に開くシーン(cerealがstd::filesystem未対応のためstd::stringにしている)
	};
} // cumulonimbus::system