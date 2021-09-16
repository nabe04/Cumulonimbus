#include "cum_imgui_helper.h"

namespace cumulonimbus::helper::imgui
{
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