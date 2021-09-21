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
		//-- ボタン色 --//
		// 通常時(Scene時)
		static const ImVec4 button_col_normal{ .235f,0.235f,0.235f,1.f };
		// カーソルホバー時
		static const ImVec4 button_col_hovered{ .607f,.607f ,.607f ,1.f };
		// ボタン選択時
		static const ImVec4 button_col_active{ .784f,.784f,.784f,1.f };
		// ボタン押下時(ゲーム時)
		static const ImVec4 button_col_pressed{ .313f,.254f,.862f,1.f };

		if (playback_state.test(static_cast<size_t>(PlaybackMode::Play)))
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
			playback_state.flip(static_cast<size_t>(PlaybackMode::Play));
			if (!playback_state.test(static_cast<size_t>(PlaybackMode::Play)))
				playback_state.reset();
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

		if (playback_state.test(static_cast<size_t>(PlaybackMode::Pause)))
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
			playback_state.flip(static_cast<size_t>(PlaybackMode::Pause));
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

		if (playback_state.test(static_cast<size_t>(PlaybackMode::Play)))
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
