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
	class GameView final
	{
	public:
		explicit GameView() = default;
		~GameView() = default;

		void Render(ecs::Registry* registry);
	private:
		DirectX::SimpleMath::Vector2 image_size{}; // Game Viewに表示するイメージサイズ
		DirectX::SimpleMath::Vector2 window_offset{};
 	};
} // cumulonimbus::editor