#pragma once
#include <map>
#include <functional>

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

} // cumulonimbus

namespace cumulonimbus::system
{
	class System final
	{
	public:
		explicit System();
		~System() = default;

		void Render(ecs::Registry* registry);
		/**
		 * @brief : プロジェクト共通の設定関数の登録
		 * @remark : ImGui上での設定を目的としているため
		 *           ImGui用の関数を登録する
		 */
		void RegisterRenderFunction(const mapping::rename_type::Hash& hash, const  std::function<void(ecs::Registry* registry)>& render_func);
	private:
		// 登録された型のRender関数の保持
		std::map<mapping::rename_type::Hash, std::function<void(ecs::Registry* registry)>> render_functions{};

		std::unique_ptr<camera::CameraTexture> camera_texture{};
	};
} // cumulonimbus::system