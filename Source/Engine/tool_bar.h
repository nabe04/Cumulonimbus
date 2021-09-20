#pragma once
#include <bitset>

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
	class ToolBar final
	{
	public:
		/**
		 * @brief : PPS -> Play,Pause,Step‚Ì—ª
		 */
		enum class PPS_Button
		{
			Play,
			Pause,
			Step,

			End
		};

	public:
		explicit ToolBar();
		~ToolBar() = default;

		void Render(ecs::Registry* registry);

	private:
		// Play,Pause,Stepƒ{ƒ^ƒ“ó‘Ô
		std::bitset<static_cast<size_t>(PPS_Button::End)> pps_state{};

		const DirectX::SimpleMath::Vector2 pps_size{ 40,30 };
	};
} // cumulonimbus::editor