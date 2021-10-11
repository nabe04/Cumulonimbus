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

	namespace graphics
	{
		class SkyBox;
	} // graphics

} // cumulonimbus

namespace cumulonimbus::system
{
	class System final
	{
	public:
		explicit System();
		~System() = default;

		//template<class Archive>
		//void serialize(Archive&& archive, std::uint32_t version);

		template<class Archive>
		void save(Archive&& archive, std::uint32_t version) const;

		template<class Archive>
		void load(Archive&& archive, std::uint32_t version);

		/**
		 * @brief : 「.system」ファイルのセーブ
		 * @param save_path : 「.system」ファイルまでのパス(※拡張子(「.sys」)含む)
		 *					   (パスを指定しない場合デフォルトの
		 *					   パスに保存される(./Data/System/system.sys))
		 */
		void Save(const std::filesystem::path& save_path = {""});

		/**
		 * @brief : 「.system」ファイルのロード
		 * @param load_path : 「.system」ファイルまでのパス(※拡張子(「.sys」)含む)
		 *					   (パスを指定しない場合デフォルトの
		 *					   パスからロードされる(./Data/System/system.sys))
		 */
		void Load(const std::filesystem::path& load_path = {""});

		void Update(float dt);

		/**
		 * @brief : render_functionsに登録された関数の実行
		 */
		void Render(ecs::Registry* registry);

		/**
		 * @brief : プロジェクト共通の設定関数の登録
		 * @remark : ImGui上での設定を目的としているため
		 *           ImGui用の関数を登録する
		 */
		void RegisterRenderFunction(const mapping::rename_type::Hash& hash, const  std::function<void(ecs::Registry* registry)>& render_func);

		[[nodiscard]]
		camera::CameraTexture& GetCameraTexture() const;
		[[nodiscard]]
		graphics::SkyBox& GetSkyBox() const;
		[[nodiscard]]
		const std::string& GetCurrentScenePath() const { return current_scene_path; }
		[[nodiscard]]
		const std::string& GetDefaultScenePath() const { return default_scene_path; }

		void SetCurrentScenePath(const std::string& scene_path)
		{
			current_scene_path = scene_path;
			int a;
			a = 0;
		}
	private:
		// 登録された型のRender関数の保持
		std::map<mapping::rename_type::Hash, std::function<void(ecs::Registry* registry)>> render_functions{};

		std::unique_ptr<camera::CameraTexture> camera_texture{};
		std::unique_ptr<graphics::SkyBox>	   sky_box{};

		std::string current_scene_path{};  // 現在開かれているシーン(cerealがstd::filesystem未対応のためstd::stringにしている)
		std::string default_scene_path{}; // ウィンドウ作成時に開くシーン(cerealがstd::filesystem未対応のためstd::stringにしている)
	};
} // cumulonimbus::system