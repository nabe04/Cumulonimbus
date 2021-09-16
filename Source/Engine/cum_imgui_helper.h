#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <imgui.h>

#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), ImGui::SameLine(), func("##" label, __VA_ARGS__))

namespace cumulonimbus::helper::imgui
{
	/**
	 * @brief : ImGui::Image�̊ȈՔ�
	 *        (shader resource view �� �摜�̑傫�����w�肷��̂�)
	 */
	inline void Image(
		ID3D11ShaderResourceView*    srv,
		DirectX::SimpleMath::Vector2 image_size = {100,100})
	{
		ImGui::Image((void*)srv, ImVec2(image_size.x, image_size.y));
	}

	/**
	 * @brief :ImGui::Combo��std::string�Ή���
	 * @param label : Combo��
	 * @param current_item : ���ݑI������Ă���A�C�e����
	 * @param items : Combo���ɕ\�����ꂽ�����O�̔z��
	 * @return : true -> current_item���ύX���ꂽ�Ƃ�
	 * @return : false -> ����ȊO
	 */
	bool Combo(
		const std::string& label,
		std::string& current_item,
		const std::vector<std::string>& items);

	/**
	 * @brief :	ImGui::DragFloat3�̃e�L�X�g�ʒu�����ɂ������̕\��(X,Y,Z)
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