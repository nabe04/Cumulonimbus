#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <imgui.h>

#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), ImGui::SameLine(), func("##" label, __VA_ARGS__))

namespace cumulonimbus::helper::imgui
{
	/**
	 * @brief : ImGui::Imageの簡易版
	 *        (shader resource view と 画像の大きさを指定するのみ)
	 */
	inline void Image(
		ID3D11ShaderResourceView*    srv,
		DirectX::SimpleMath::Vector2 image_size = {100,100})
	{
		ImGui::Image((void*)srv, ImVec2(image_size.x, image_size.y));
	}

	/**
	 * @brief :ImGui::Comboのstd::string対応版
	 * @param label : Combo名
	 * @param current_item : 現在選択されているアイテム名
	 * @param items : Combo内に表示されたい名前の配列
	 * @return : true -> current_itemが変更されたとき
	 * @return : false -> それ以外
	 */
	bool Combo(
		const std::string& label,
		std::string& current_item,
		const std::vector<std::string>& items);

	/**
	 * @brief :	ImGui::DragFloat3のテキスト位置を左にし成分の表示(X,Y,Z)
	 */
	void DragFloat3(
		const std::string& label,
		DirectX::SimpleMath::Vector3& v,
		float v_speed = 1.f,
		float v_min = 0.f,
		float v_max = 1.f,
		const char* format = "%.2f",
		ImGuiSliderFlags flags = 0);

} // cumulonimbus::helper::imgui