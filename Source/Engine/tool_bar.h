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
		/**
		 * @brief : PPS -> Play,Pause,Step�̗�
		 */
		enum class PlaybackMode
		{
			Play,
			Pause,
			Step,

			End
		};

	public:
		explicit ToolBar();
		~ToolBar() = default;

		void Update();
		void Render(ecs::Registry* registry);

		[[nodiscard]]
		ButtonState GetButtonState(PlaybackMode mode) const;

	private:
		// Play,Pause,Step�{�^�����
		std::bitset<static_cast<size_t>(PlaybackMode::End)> playback_state{};
		std::bitset<static_cast<size_t>(PlaybackMode::End)> old_playback_state{};
		std::array<ButtonState, static_cast<size_t>(PlaybackMode::End)> button_state{};

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

		/**
		 *
		 */
		bool IsPlaybackState(PlaybackMode mode);
	};
} // cumulonimbus::editor