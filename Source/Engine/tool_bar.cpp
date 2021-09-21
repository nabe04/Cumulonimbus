#include "tool_bar.h"

namespace cumulonimbus::editor
{
	ToolBar::ToolBar()
	{
		playback_state.reset();
	}

	void ToolBar::Update()
	{
		//old_playback_state = playback_state;
	}

	void ToolBar::Render(ecs::Registry* registry)
	{
		old_playback_state = playback_state;
		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoTitleBar;
		ImGui::Begin("##Tool Bar",nullptr);
		const float im_window_width = ImGui::GetWindowWidth();

		ImGui::SetCursorPosX((im_window_width / 2) - playback_size.x);
		PlayButton();
		ImGui::SameLine();
		PauseButton();
		ImGui::SameLine();
		StepButton();

		ImGui::Text("State %d", GetButtonState(PlaybackMode::Play));

		ImGui::End();
	}

	ButtonState ToolBar::GetButtonState(PlaybackMode mode) const
	{
		if(playback_state.test(static_cast<size_t>(mode)))
		{
			if (old_playback_state.test(static_cast<size_t>(mode)))
				return ButtonState::Held;
			return ButtonState::Press;
		}

		if(old_playback_state.test(static_cast<size_t>(mode)))
		{
			return ButtonState::Release;
		}

		return ButtonState::None;
	}

	void ToolBar::PlayButton()
	{
		//-- �{�^���F --//
		// �ʏ펞(Scene��)
		static const ImVec4 button_col_normal{ .235f,0.235f,0.235f,1.f };
		// �J�[�\���z�o�[��
		static const ImVec4 button_col_hovered{ .607f,.607f ,.607f ,1.f };
		// �{�^���I����
		static const ImVec4 button_col_active{ .784f,.784f,.784f,1.f };
		// �{�^��������(�Q�[����)
		static const ImVec4 button_col_pressed{ .313f,.254f,.862f,1.f };

		if (playback_state.test(static_cast<size_t>(PlaybackMode::Play)))
		{// �Q�[�����͑S�Ă̐F�𓯂��F��(button_col_pressed)
			ImGui::PushStyleColor(ImGuiCol_Button		, button_col_pressed);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, button_col_pressed);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive	, button_col_pressed);
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button		, button_col_normal);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, button_col_hovered);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive	, button_col_active);
		}

		if (ImGui::Button(ICON_FA_PLAY, { playback_size.x,playback_size.y }))
		{
			playback_state.flip(static_cast<size_t>(PlaybackMode::Play));
			if (!playback_state.test(static_cast<size_t>(PlaybackMode::Play)))
				playback_state.reset();
			//Todo :  �Q�[���̊J�n & �I������
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}

	void ToolBar::PauseButton()
	{
		//-- �{�^���F --//
		// �ʏ펞
		static const ImVec4 button_col_normal{ .235f,0.235f,0.235f,1.f };
		// �J�[�\���z�o�[��
		static const ImVec4 button_col_hovered{ .607f,.607f ,.607f ,1.f };
		// �{�^���I����
		static const ImVec4 button_col_active{ .784f,.784f,.784f,1.f };
		// �{�^��������
		static const ImVec4 button_col_pressed{ .784f,.784f,.784f,1.f };

		if (playback_state.test(static_cast<size_t>(PlaybackMode::Pause)))
		{// �Q�[�����͑S�Ă̐F�𓯂��F��(button_col_pressed)
			ImGui::PushStyleColor(ImGuiCol_Button		, button_col_pressed);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, button_col_pressed);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive	, button_col_pressed);
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button		, button_col_normal);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, button_col_hovered);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive	, button_col_active);
		}

		if (ImGui::Button(ICON_FA_PAUSE, { playback_size.x,playback_size.y }))
		{
			playback_state.flip(static_cast<size_t>(PlaybackMode::Pause));
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}

	void ToolBar::StepButton()
	{
		//-- �{�^���F --//
		// �ʏ펞(Scene��)
		static const ImVec4 button_col_scene{ .235f,0.235f,0.235f,.274f };
		// �ʏ펞(Game��)
		static const ImVec4 button_col_game{ .235f,0.235f,0.235f,1.f };
		// �J�[�\���z�o�[��
		static const ImVec4 button_col_hovered{ .607f,.607f ,.607f ,1.f};
		// �{�^���I����
		static const ImVec4 button_col_active{ .784f,.784f,.784f,1.f };

		if (playback_state.test(static_cast<size_t>(PlaybackMode::Play)))
		{// �Q�[�����F
			ImGui::PushStyleColor(ImGuiCol_Button		, button_col_game);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, button_col_hovered);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive	, button_col_active);
		}
		else
		{// �V�[�����F
			ImGui::PushStyleColor(ImGuiCol_Button		, button_col_scene);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, button_col_scene);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive	, button_col_scene);
		}

		if (ImGui::Button(ICON_FA_STEP_FORWARD, { playback_size.x,playback_size.y }))
		{
			if(!playback_state.test(static_cast<size_t>(PlaybackMode::Play)))
			{
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				return;
			}
			if (!playback_state.test(static_cast<size_t>(PlaybackMode::Pause)))
			{
				playback_state.set(static_cast<size_t>(PlaybackMode::Pause));
			}
			playback_state.set(static_cast<size_t>(PlaybackMode::Step));
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}
} // cumulonimbus::editor
