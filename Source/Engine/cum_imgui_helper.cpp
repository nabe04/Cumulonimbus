#include "cum_imgui_helper.h"

#include <imgui_internal.h>

#include "locator.h"
#include "texture.h"
#include "texture_loader.h"

namespace cumulonimbus::helper::imgui
{
	void ImageButtonWithText(
		ID3D11ShaderResourceView* srv	,const char* label,
		IMButtonState& button_state		,const ImVec2& image_size,
		const ImVec2& uv0				,const ImVec2& uv1,
		const int frame_padding,
		const ImVec4& bg_col			,const ImVec4& tint_col)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return;

		ImVec2 size = image_size;
		if (size.x <= 0 && size.y <= 0) { size.x = size.y = ImGui::GetTextLineHeightWithSpacing(); }
		else {
			if (size.x <= 0)          size.x = size.y;
			else if (size.y <= 0)     size.y = size.x;
			size.x *= window->FontWindowScale * ImGui::GetIO().FontGlobalScale;
			size.y *= window->FontWindowScale * ImGui::GetIO().FontGlobalScale;
		}

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		const ImGuiID id = window->GetID(label);
		const ImVec2 text_size = ImGui::CalcTextSize(label, NULL, true);
		const bool has_text = text_size.x > 0;

		const float inner_spacing = has_text ? ((frame_padding >= 0) ? (float)frame_padding : (style.ItemInnerSpacing.x)) : 0.f;
		const ImVec2 padding = (frame_padding >= 0) ? ImVec2((float)frame_padding, (float)frame_padding) : style.FramePadding;
		//const ImVec2 total_size_without_padding(size.x + inner_spacing + text_size.x, size.y > text_size.y ? size.y : text_size.y); // default
		const ImVec2 total_size_without_padding(size.x > text_size.x ? size.x : size.x, size.y + inner_spacing + text_size.y);
		const ImRect bb(window->DC.CursorPos, ImVec2{ window->DC.CursorPos.x + total_size_without_padding.x + padding.x * 2,window->DC.CursorPos.y + total_size_without_padding.y + padding.y });
		ImVec2 start(0, 0);
		start.x = window->DC.CursorPos.x + padding.x;
		start.y = window->DC.CursorPos.y + padding.y;
		if (size.y < text_size.y) start.y += (text_size.y - size.y) * .5f;
		const ImRect image_bb(start, ImVec2{ start.x + size.x,start.y + size.y });
		start.x = window->DC.CursorPos.x + padding.x;
		start.y = window->DC.CursorPos.y + padding.y;
		//start.x += size.x + inner_spacing; if (size.y > text_size.y) start.y += (size.y - text_size.y) * .5f; // default
		start.y += (size.y + inner_spacing);
		if (size.x > text_size.x)
			start.x += (size.x - text_size.x) * .5f;
		ImGui::ItemSize(bb);
		if (!ImGui::ItemAdd(bb, id))
			return;

		button_state.pressed = ImGui::ButtonBehavior(bb, id, &button_state.hovered, &button_state.held);

		// Render
		const ImU32 col = ImGui::GetColorU32((button_state.hovered && button_state.held) ? ImGuiCol_ButtonActive : button_state.hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
		ImGui::RenderFrame(bb.Min, bb.Max, col, true, ImClamp((float)ImMin(padding.x, padding.y), 0.0f, style.FrameRounding));
		if (bg_col.w > 0.0f)
			window->DrawList->AddRectFilled(image_bb.Min, image_bb.Max, ImGui::GetColorU32(bg_col));

		window->DrawList->AddImage((void*)srv, image_bb.Min, image_bb.Max, uv0, uv1, ImGui::GetColorU32(tint_col));

		if (text_size.x > 0)
		{
			const ImVec2 p0 = ImGui::GetItemRectMin();
			const ImVec2 p1 = ImGui::GetItemRectMax();
			ImGui::PushClipRect(p0, p1, false);
			ImGui::RenderText(start, label);
			ImGui::PopClipRect();
		}
	}

	void ImageButtonWithText(
		asset::Texture& texture		, const char* label,
		IMButtonState& button_state	, const ImVec2& image_size,
		const ImVec2& uv0			, const ImVec2& uv1,
		int frame_padding,
		const ImVec4& bg_col		, const ImVec4& tint_col)
	{
		ImageButtonWithText(texture.GetShaderResourceView(), label, button_state, image_size, uv0, uv1, frame_padding, bg_col, tint_col);
	}

	void ImageButtonWithText(
		const mapping::rename_type::UUID& texture_id,
		const char* label		, IMButtonState& button_state,
		const ImVec2& image_size, const ImVec2& uv0,
		const ImVec2& uv1		, int frame_padding,
		const ImVec4& bg_col	, const ImVec4& tint_col)
	{
		asset::Texture& texture = locator::Locator::GetAssetManager()->GetLoader<asset::TextureLoader>()->GetTexture(texture_id);
		ImageButtonWithText(texture, label, button_state, image_size, uv0, uv1, frame_padding, bg_col, tint_col);
	}


	bool Combo(
		const std::string& label,
		std::string& current_item,
		const std::vector<std::string>& items)
	{
		bool changed = false;

		if (ImGui::BeginCombo(label.c_str(), current_item.c_str()))
		{
			for (int n = 0; n < items.size(); ++n)
			{
				bool is_selected = (current_item == items[n]);
				if (ImGui::Selectable(items[n].c_str(), is_selected))
				{
					current_item = items[n];
					changed = true;
				}
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		return changed;
	}

	void DragFloat3(
		const std::string& label,
		DirectX::SimpleMath::Vector3& v,
		const float v_speed,
		const float v_min,
		const float v_max,
		const char* format,
		const ImGuiSliderFlags flags)
	{
		ImGui::PushItemWidth(110.f); // パラメータ調整入力の表示域の調整
		ImGui::Text(label.c_str());
		ImGui::SameLine(0,20.f);
		IMGUI_LEFT_LABEL(ImGui::DragFloat, "X", &v.x, v_speed, v_min, v_max, format, flags);
		ImGui::SameLine();
		IMGUI_LEFT_LABEL(ImGui::DragFloat, "Y", &v.y, v_speed, v_min, v_max, format, flags);
		ImGui::SameLine();
		IMGUI_LEFT_LABEL(ImGui::DragFloat, "Z", &v.z, v_speed, v_min, v_max, format, flags);

		ImGui::PushItemWidth(0); // サイズを元に戻す
	}

} // cumulonimbus::helper::imgui