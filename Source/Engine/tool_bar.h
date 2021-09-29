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
		 * @brief : �w�肳�ꂽPlaybackMode�̃r�b�g�������Ă��邩�m�F
		 * @return : true -> �r�b�g�������Ă���
		 * @return : false -> �r�b�g�������Ă��Ȃ�
		 */
		[[nodiscard]]
		bool IsPlaybackState(Button mode) const;

		[[nodiscard]]
		const ToolBarButton& GetToolBarButton() const { return tool_bar_button; }

		/**
		 * @breif : �Q�[�����s��(�v���C�{�^����������Ă���)���̔��f
		 * @return : true -> �Q�[�����s��(�v���C�{�^����������Ă���)
		 */
		[[nodiscard]]
		bool IsPlayGame() const { return IsPlaybackState(Button::Play); }

	private:
		ToolBarButton tool_bar_button{};
		// playback mode �{�^���̃T�C�Y
		const DirectX::SimpleMath::Vector2 playback_size{ 40,30 };
		// playback mode(pb)�{�^���̒ʏ펞�F
		const DirectX::SimpleMath::Vector4 pb_col_normal{ .235f,0.235f,0.235f,1.f };
		// playback mode(pb)�{�^���̃J�[�\���z�o�[���F
		const DirectX::SimpleMath::Vector4 pb_col_hovered{ .607f,.607f ,.607f ,1.f };
		// playback mode(pb)�{�^���̑I�����F

		void PlayButton();
		void PauseButton();
		void StepButton();
	};
} // cumulonimbus::editor