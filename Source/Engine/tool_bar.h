#pragma once
#include <bitset>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "input_system.h"

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
		enum class Button
		{
			Play,
			Pause,
			Step,

			End
		};

		struct ToolBarButton
		{
			std::array<ButtonState, static_cast<size_t>(Button::End)> button_state{};

			void Update();
			[[nodiscard]]
			ButtonState GetButtonState(Button mode) const;

			std::bitset<static_cast<size_t>(Button::End)> button_flg{};
			std::bitset<static_cast<size_t>(Button::End)> old_button_flg{};
		};

	public:
		explicit ToolBar() = default;
		~ToolBar() = default;

		void Update();
		void Render(ecs::Registry* registry);

		/**
		 * @brief : 指定されたPlaybackModeのビットが立っているか確認
		 * @return : true -> ビットが立っている
		 * @return : false -> ビットが立っていない
		 */
		[[nodiscard]]
		bool IsPlaybackState(Button mode) const;

		[[nodiscard]]
		const ToolBarButton& GetToolBarButton() const { return tool_bar_button; }

		/**
		 * @breif : ゲーム実行中(プレイボタンが押されている)かの判断
		 * @return : true -> ゲーム実行中(プレイボタンが押されている)
		 */
		[[nodiscard]]
		bool IsPlayGame() const { return IsPlaybackState(Button::Play); }

	private:
		ToolBarButton tool_bar_button{};
		// playback mode ボタンのサイズ
		const DirectX::SimpleMath::Vector2 playback_size{ 40,30 };
		// playback mode(pb)ボタンの通常時色
		const DirectX::SimpleMath::Vector4 pb_col_normal{ .235f,0.235f,0.235f,1.f };
		// playback mode(pb)ボタンのカーソルホバー時色
		const DirectX::SimpleMath::Vector4 pb_col_hovered{ .607f,.607f ,.607f ,1.f };
		// playback mode(pb)ボタンの選択時色

		void PlayButton();
		void PauseButton();
		void StepButton();
	};
} // cumulonimbus::editor