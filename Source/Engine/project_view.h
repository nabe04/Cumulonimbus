#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

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
		DirectX::SimpleMath::Vector2 viewer_size{}; // ImGui��ł�viewer �E�B���h�E�̃T�C�Y

		/**
		 * @brief : �C���|�[�g���j���[
		 */
		void ImportMenu();
	};
} // cumulonimbus::editor