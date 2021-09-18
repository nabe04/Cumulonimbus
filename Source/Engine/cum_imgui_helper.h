#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include <imgui.h>

#include "rename_type_mapping.h"
#include "texture.h"

#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), ImGui::SameLine(), func("##" label, __VA_ARGS__))

namespace cumulonimbus::helper::imgui
{
	/**
	 *
	 */
    struct IMButtonState
    {
        bool pressed{false};    // true -> �{�^���������ꂽ�u��
        bool hovered{ false };  // true -> �{�^����ɃJ�[�\��������ꍇ
        bool held{ false };     // true -> �{�^��������
    };

	/**
	 * @brief : ImGui::Image�̊ȈՔ�
	 *        (shader resource view �� �摜�̑傫�����w�肷��̂�)
	 */
	void Image(
		ID3D11ShaderResourceView* srv,
		const DirectX::SimpleMath::Vector2& image_size = { 100,100 });

	/**
	 * @brief : Texture�N���X����摜��\��
	 */
	void Image(
		asset::Texture& texture,
		const DirectX::SimpleMath::Vector2& image_size = { 100,100 });

	/**
	 * @breif : �A�Z�b�g�V�[�g�̃e�N�X�`��ID����摜��\��
	 */
	void Image(
		const mapping::rename_type::UUID& texture_id,
		const DirectX::SimpleMath::Vector2& image_size = { 100,100 });

	/**
	 * @brief : �e�L�X�g & �摜�t���{�^���̍쐬
	 * @remark : �Q�l�T�C�g -> https://github.com/ocornut/imgui/issues/1096
	 * @remark : �摜�̉��Ƀe�L�X�g���\�������
	 * @remark : �{�^���̏�ԂȂǂ�button_state�ŕێ�����
	 * @param srv : �摜�̎���Shader Resource View(��� ImGui::ImTexID�ɕϊ������)
	 * @param label : �{�^����
	 * @param button_state : �{�^�����
	 * @param image_size : �摜�T�C�Y
	 * @param uv0 : �摜����UV
	 * @param uv1 : �摜�E��UV
	 * @param frame_padding : �t���[���]��
	 * @param bg_col : �w�i�F
	 * @param tint_col : �摜�F
	 */
	void ImageButtonWithText(
		ID3D11ShaderResourceView* srv,
		const char* label,
		IMButtonState& button_state,
		const ImVec2& image_size = ImVec2(0, 0),
		const ImVec2& uv0 = ImVec2(0, 0),
		const ImVec2& uv1 = ImVec2(1, 1),
		int frame_padding = -1,
		const ImVec4& bg_col = ImVec4(0, 0, 0, 0),
		const ImVec4& tint_col = ImVec4(1, 1, 1, 1));

	/**
	 * @brief : �e�L�X�g & �摜�t���{�^���̍쐬
	 * @remark : �Q�l�T�C�g -> https://github.com/ocornut/imgui/issues/1096
	 * @remark : �摜�̉��Ƀe�L�X�g���\�������
	 * @remark : �{�^���̏�ԂȂǂ�button_state�ŕێ�����
	 * @param texture : Texture�N���X(��� ImGui::ImTexID�ɕϊ������)
	 * @param label : �{�^����
	 * @param button_state : �{�^�����
	 * @param image_size : �摜�T�C�Y
	 * @param uv0 : �摜����UV
	 * @param uv1 : �摜�E��UV
	 * @param frame_padding : �t���[���]��
	 * @param bg_col : �w�i�F
	 * @param tint_col : �摜�F
	 */
	void ImageButtonWithText(
		asset::Texture& texture,
		const char* label,
		IMButtonState& button_state,
		const ImVec2& image_size = ImVec2(0, 0),
		const ImVec2& uv0 = ImVec2(0, 0),
		const ImVec2& uv1 = ImVec2(1, 1),
		int frame_padding = -1,
		const ImVec4& bg_col = ImVec4(0, 0, 0, 0),
		const ImVec4& tint_col = ImVec4(1, 1, 1, 1));

	/**
	 * @brief : �e�L�X�g & �摜�t���{�^���̍쐬
	 * @remark : �Q�l�T�C�g -> https://github.com/ocornut/imgui/issues/1096
	 * @remark : �摜�̉��Ƀe�L�X�g���\�������
	 * @remark : �{�^���̏�ԂȂǂ�button_state�ŕێ�����
	 * @param texture_id : �A�Z�b�g�V�[�g�ɓo�^����Ă���e�N�X�`��ID
	 * @param label : �{�^����
	 * @param button_state : �{�^�����
	 * @param image_size : �摜�T�C�Y
	 * @param uv0 : �摜����UV
	 * @param uv1 : �摜�E��UV
	 * @param frame_padding : �t���[���]��
	 * @param bg_col : �w�i�F
	 * @param tint_col : �摜�F
	 */
	void ImageButtonWithText(
		const mapping::rename_type::UUID& texture_id,
		const char* label,
		IMButtonState& button_state,
		const ImVec2& image_size = ImVec2(0, 0),
		const ImVec2& uv0 = ImVec2(0, 0),
		const ImVec2& uv1 = ImVec2(1, 1),
		int frame_padding = -1,
		const ImVec4& bg_col = ImVec4(0, 0, 0, 0),
		const ImVec4& tint_col = ImVec4(1, 1, 1, 1));


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