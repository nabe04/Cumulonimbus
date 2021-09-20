#include "tool_bar.h"

namespace cumulonimbus::editor
{
	ToolBar::ToolBar()
	{
		play_back_state.reset();
	}

	void ToolBar::Render(ecs::Registry* registry)
	{
		ImGuiWindowFlags window_flags = 0;
		static ImVec4 color{ 0,0,0,1 };
		window_flags |= ImGuiWindowFlags_NoTitleBar;
		ImGui::Begin("##Tool Bar",nullptr);
		const float im_window_width = ImGui::GetWindowWidth();

		ImGui::SetCursorPosX((im_window_width / 2) - playback_size.x);
		PlayButton();
		ImGui::SameLine();
		PauseButton();
		ImGui::SameLine();
		StepButton();

		//ImGui::PushStyleColor(ImGuiCol_Button, { .235f,0.235f,0.235f,1.f });
		//ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { .607f,.607f ,.607f ,1.f });
		//ImGui::PushStyleColor(ImGuiCol_ButtonActive, { .784f,.784f,.784f,1.f });
		////ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);

		//ImGui::SetCursorPosX((im_window_width / 2) - playback_size.x);
		//if(ImGui::Button(ICON_FA_PLAY, { playback_size.x,playback_size.y }))
		//{

		//}
		//ImGui::SameLine();
		//if (ImGui::Button(ICON_FA_PAUSE, { playback_size.x,playback_size.y }))
		//{

		//}
		//ImGui::SameLine();

		//ImGui::PopStyleColor();
		//ImGui::PopStyleColor();
		//ImGui::PopStyleColor();

		//ImGui::PushStyleColor(ImGuiCol_Button, color);

		//if (ImGui::Button(ICON_FA_STEP_FORWARD, { playback_size.x,playback_size.y }))
		//{

		//}
		//ImGui::ColorPicker4("color", (float*)&color);
		//ImGui::PopStyleColor();
		//ImGui::PopStyleColor();

		ImGui::End();

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

		if (play_back_state.test(static_cast<size_t>(PlaybackMode::Play)))
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
			play_back_state.flip(static_cast<size_t>(PlaybackMode::Play));
			if (!play_back_state.test(static_cast<size_t>(PlaybackMode::Play)))
				play_back_state.reset();
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

		if (play_back_state.test(static_cast<size_t>(PlaybackMode::Pause)))
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
			play_back_state.flip(static_cast<size_t>(PlaybackMode::Pause));
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

		if (play_back_state.test(static_cast<size_t>(PlaybackMode::Play)))
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
			if(!play_back_state.test(static_cast<size_t>(PlaybackMode::Play)))
			{
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				return;
			}
			if (!play_back_state.test(static_cast<size_t>(PlaybackMode::Pause)))
			{
				play_back_state.set(static_cast<size_t>(PlaybackMode::Pause));
			}
			play_back_state.set(static_cast<size_t>(PlaybackMode::Step));
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}
} // cumulonimbus::editor
