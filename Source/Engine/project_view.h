#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include <filesystem>

namespace cumulonimbus
{
	namespace ecs
	{
		class Registry;
	} // ecs
} // cumulonimbus


namespace cumulonimbus::editor
{
	class ProjectView final
	{
	public:
		ProjectView();
		void Render(ecs::Registry* registry);
	private:
		DirectX::SimpleMath::Vector2 viewer_size{}; // ImGui上でのviewer ウィンドウのサイズ

		/**
		 * @brief : アセットのインポートボタン
		 */
		void ImportMenu();
		/**
		 * @brief : ファイルツリーの表示
		 */
		std::filesystem::path ShowFileTree(const std::filesystem::path& path);
	};
} // cumulonimbus::editor