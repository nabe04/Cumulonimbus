#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>
#include "constant_buffer.h"

class View;

class Light
{
public:
	struct LightData
	{
		DirectX::XMFLOAT4  position{ 10.0f,20.0f,-10.0f ,0 };	// 位置
		DirectX::XMFLOAT4  direction{ .0f,-1.0f,1.f,1.f };	// 方向
		DirectX::XMFLOAT4  ambient{ 1.f,1.f,1.f,1.f };	// アンビエント
		DirectX::XMFLOAT4  color{ 1.f,1.f,1.f,1.f };	// 色

		DirectX::XMFLOAT4X4 view_matrix{};

		// 透視投影用
		DirectX::XMFLOAT4X4 perspective_projection_matrix{}; //プロジェクション行列
		DirectX::XMFLOAT4X4 perspective_view_projection_matrix{}; // ビュー・プロジェクション行列

		// 平行投影用
		float orthographic_view_width{ 5000 }; // 幅
		float orthographic_view_height{ 5000 };	// 高さ
		float orthographic_near_z{ 0 };	// 最近値
		float orthographic_far_z{ 1000 };	// 最遠地
		DirectX::XMFLOAT4X4 orthographic_projection_matrix{};	// プロジェクション行列
		DirectX::XMFLOAT4X4 orthographic_view_projection_matrix{}; // ビュー・プロジェクション行列
	};

	explicit Light(ID3D11Device* device, const LightData& light_data);

	void Update(const View* view);
	void ActivateCBuffer(ID3D11DeviceContext* const immediate_context, const bool set_in_vs, const bool set_in_ps) const;
	void DeactivateCBuffer(ID3D11DeviceContext* const immediate_context) const;

	[[nodiscard]] const DirectX::XMFLOAT4& GetPosition() const { return  cb_light->data.position; }
	[[nodiscard]] const DirectX::XMFLOAT4& GetLightDir() const { return cb_light->data.direction; }
	[[nodiscard]] const DirectX::XMFLOAT4& GetAmbient() const { return  cb_light->data.ambient; }
	[[nodiscard]] const DirectX::XMFLOAT4& GetColor() const { return  cb_light->data.color; }
	[[nodiscard]] float GetOrthographicViewWidth() const { return cb_light->data.orthographic_view_width; }
	[[nodiscard]] float GetOrthographicViewHeight() const { return cb_light->data.orthographic_view_height; }
	[[nodiscard]] float GetOrthographicNearZ() const { return cb_light->data.orthographic_near_z; }
	[[nodiscard]] float GetOrthographicFarZ() const { return cb_light->data.orthographic_far_z; }


	void SetPosition(const DirectX::XMFLOAT4& pos) const { cb_light->data.position = pos; }
	void SetLightDir(const DirectX::XMFLOAT4& dir) const { cb_light->data.direction = dir; }
	void SetColor(const DirectX::XMFLOAT4& color) const { cb_light->data.color = color; }
	void SetAmbient(const DirectX::XMFLOAT4& ambient) const { cb_light->data.ambient = ambient; }
	void SetOrthographicViewWidth(const float width) const { cb_light->data.orthographic_view_width = width; }
	void SetOrthographicViewHeight(const float height) const { cb_light->data.orthographic_view_height = height; }
	void SetOrthographicNearZ(const float near_z) const { cb_light->data.orthographic_near_z = near_z; }
	void SetOrthographicFarZ(const float far_z) const { cb_light->data.orthographic_far_z = far_z; }

	[[nodiscard]] const LightData& GetData() const { return cb_light->data; }
	void SetData(const LightData& light_data) const { cb_light->data = light_data; }

	void WriteImGui() const;

private:
	std::unique_ptr<buffer::ConstantBuffer<LightData>> cb_light{};

	const u_int cb_slot = 2;

	DirectX::SimpleMath::Vector3 view_right{ 1.f,.0f,.0f };
	DirectX::SimpleMath::Vector3 view_up{ .0f,1.f,.0f };
	DirectX::SimpleMath::Vector3 view_front{ .0f,.0f,1.f };
};

