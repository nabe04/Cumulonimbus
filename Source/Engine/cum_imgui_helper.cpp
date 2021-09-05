#include "cum_imgui_helper.h"

namespace cumulonimbus::helper::imgui
{
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