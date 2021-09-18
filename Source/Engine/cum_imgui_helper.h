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
        bool pressed{false};    // true -> ボタンが押された瞬間
        bool hovered{ false };  // true -> ボタン上にカーソルがある場合
        bool held{ false };     // true -> ボタン押下中
    };

	/**
	 * @brief : ImGui::Imageの簡易版
	 *        (shader resource view と 画像の大きさを指定するのみ)
	 */
	void Image(
		ID3D11ShaderResourceView* srv,
		const DirectX::SimpleMath::Vector2& image_size = { 100,100 });

	/**
	 * @brief : Textureクラスから画像を表示
	 */
	void Image(
		asset::Texture& texture,
		const DirectX::SimpleMath::Vector2& image_size = { 100,100 });

	/**
	 * @breif : アセットシートのテクスチャIDから画像を表示
	 */
	void Image(
		const mapping::rename_type::UUID& texture_id,
		const DirectX::SimpleMath::Vector2& image_size = { 100,100 });

	/**
	 * @brief : テキスト & 画像付きボタンの作成
	 * @remark : 参考サイト -> https://github.com/ocornut/imgui/issues/1096
	 * @remark : 画像の下にテキストが表示される
	 * @remark : ボタンの状態などはbutton_stateで保持する
	 * @param srv : 画像の持つShader Resource View(後に ImGui::ImTexIDに変換される)
	 * @param label : ボタン名
	 * @param button_state : ボタン状態
	 * @param image_size : 画像サイズ
	 * @param uv0 : 画像左上UV
	 * @param uv1 : 画像右下UV
	 * @param frame_padding : フレーム余白
	 * @param bg_col : 背景色
	 * @param tint_col : 画像色
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
	 * @brief : テキスト & 画像付きボタンの作成
	 * @remark : 参考サイト -> https://github.com/ocornut/imgui/issues/1096
	 * @remark : 画像の下にテキストが表示される
	 * @remark : ボタンの状態などはbutton_stateで保持する
	 * @param texture : Textureクラス(後に ImGui::ImTexIDに変換される)
	 * @param label : ボタン名
	 * @param button_state : ボタン状態
	 * @param image_size : 画像サイズ
	 * @param uv0 : 画像左上UV
	 * @param uv1 : 画像右下UV
	 * @param frame_padding : フレーム余白
	 * @param bg_col : 背景色
	 * @param tint_col : 画像色
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
	 * @brief : テキスト & 画像付きボタンの作成
	 * @remark : 参考サイト -> https://github.com/ocornut/imgui/issues/1096
	 * @remark : 画像の下にテキストが表示される
	 * @remark : ボタンの状態などはbutton_stateで保持する
	 * @param texture_id : アセットシートに登録されているテクスチャID
	 * @param label : ボタン名
	 * @param button_state : ボタン状態
	 * @param image_size : 画像サイズ
	 * @param uv0 : 画像左上UV
	 * @param uv1 : 画像右下UV
	 * @param frame_padding : フレーム余白
	 * @param bg_col : 背景色
	 * @param tint_col : 画像色
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