#include "tool_bar.h"

#include "scene.h"

namespace cumulonimbus::editor
{
	void ToolBar::ToolBarButton::Update()
	{
		for (int i = 0; i < static_cast<int>(ToolBar::Button::End); ++i)
		{
			if(button_flg.test(i))
			{
				if (old_button_flg.test(i))
					button_state.at(i) = ButtonState::Held;

				else
					button_state.at(i) = ButtonState::Press;
			}
			else
			{
				if (old_button_flg.test(i))
					button_state.at(i) = ButtonState::Release;
				else
					button_state.at(i) = ButtonState::None;
			}
		}

		old_button_flg = button_flg;
	}

	ButtonState ToolBar::ToolBarButton::GetButtonState(Button mode) const
	{
		return button_state.at(static_cast<size_t>(mode));
	}

	void ToolBar::Update()
	{
		tool_bar_button.Update();

		if(tool_bar_button.GetButtonState(Button::Play) == ButtonState::Press)
		{// ゲーム開始時処理
			// 現在の位置の保存
			//scene->SaveScene(file_path_helper::GetS)
		}
		if(tool_bar_button.GetButtonState(Button::Play) == ButtonState::Release)
		{// ゲーム終了時処理
			// ゲーム開始時に保存した位置に戻す
			int a;
			a = 0;
		}
	}

	void ToolBar::Render(ecs::Registry* registry)
	{
		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoTitleBar;
		if (ImGui::Begin("##Tool Bar", nullptr))
		{
			const float im_window_width = ImGui::GetWindowWidth();

			ImGui::SetCursorPosX((im_window_width / 2) - playback_size.x);
			PlayButton();
			ImGui::SameLine();
			PauseButton();
			ImGui::SameLine();
			StepButton();
		}
		ImGui::End();
	}

	bool ToolBar::IsPlaybackState(Button mode) const
	{
		return tool_bar_button.button_flg.test(static_cast<size_t>(mode));
	}

	void ToolBar::PlayButton()
	{
		//-- ボタン色 --//
		// 通常時(Scene時)
		static const ImVec4 button_col_normal{ .235f,0.235f,0.235f,1.f };
		// カーソルホバー時
		static const ImVec4 button_col_hovered{ .607f,.607f ,.607f ,1.f };
		// ボタン選択時
		static const ImVec4 button_col_active{ .784f,.784f,.784f,1.f };
		// ボタン押下時(ゲーム時)
		static const ImVec4 button_col_pressed{ .313f,.254f,.862f,1.f };

		if (tool_bar_button.button_flg.test(static_cast<size_t>(ToolBar::Button::Play)))
		{// ゲーム中は全ての色を同じ色に(button_col_pressed)
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
			tool_bar_button.button_flg.flip(static_cast<size_t>(ToolBar::Button::Play));
			if (!tool_bar_button.button_flg.test(static_cast<size_t>(ToolBar::Button::Play)))
				tool_bar_button.button_flg.reset();
			//Todo :  ゲームの開始 & 終了処理
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}

	void ToolBar::PauseButton()
	{
		//-- ボタン色 --//
		// 通常時
		static const ImVec4 button_col_normal{ .235f,0.235f,0.235f,1.f };
		// カーソルホバー時
		static const ImVec4 button_col_hovered{ .607f,.607f ,.607f ,1.f };
		// ボタン選択時
		static const ImVec4 button_col_active{ .784f,.784f,.784f,1.f };
		// ボタン押下時
		static const ImVec4 button_col_pressed{ .784f,.784f,.784f,1.f };

		if (tool_bar_button.button_flg.test(static_cast<size_t>(ToolBar::Button::Pause)))
		{// ゲーム中は全ての色を同じ色に(button_col_pressed)
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
			tool_bar_button.button_flg.flip(static_cast<size_t>(ToolBar::Button::Pause));
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}

	void ToolBar::StepButton()
	{
		//-- ボタン色 --//
		// 通常時(Scene時)
		static const ImVec4 button_col_scene{ .235f,0.235f,0.235f,.274f };
		// 通常時(Game時)
		static const ImVec4 button_col_game{ .235f,0.235f,0.235f,1.f };
		// カーソルホバー時
		static const ImVec4 button_col_hovered{ .607f,.607f ,.607f ,1.f};
		// ボタン選択時
		static const ImVec4 button_col_active{ .784f,.784f,.784f,1.f };

		if (tool_bar_button.button_flg.test(static_cast<size_t>(ToolBar::Button::Play)))
		{// ゲーム中色
			ImGui::PushStyleColor(ImGuiCol_Button		, button_col_game);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, button_col_hovered);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive	, button_col_active);
		}
		else
		{// シーン中色
			ImGui::PushStyleColor(ImGuiCol_Button		, button_col_scene);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, button_col_scene);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive	, button_col_scene);
		}

		if (ImGui::Button(ICON_FA_STEP_FORWARD, { playback_size.x,playback_size.y }))
		{
			if(!tool_bar_button.button_flg.test(static_cast<size_t>(ToolBar::Button::Play)))
			{
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				return;
			}
			if (!tool_bar_button.button_flg.test(static_cast<size_t>(ToolBar::Button::Pause)))
			{
				tool_bar_button.button_flg.set(static_cast<size_t>(ToolBar::Button::Pause));
			}
			tool_bar_button.button_flg.set(static_cast<size_t>(ToolBar::Button::Step));
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}
} // cumulonimbus::editor
