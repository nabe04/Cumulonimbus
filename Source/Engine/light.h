#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

class View;
namespace buffer
{
	template <class T>
	class ConstantBuffer;
}

class Light
{
public:
	struct LightData
	{
		DirectX::XMFLOAT4  position{ 10.0f,20.0f,-10.0f ,0 };	// 位置
		DirectX::XMFLOAT4  direction{ .0f,.0f,1.f,1.f };	// 方向
		DirectX::XMFLOAT4  ambient{ 1.f,1.f,1.f,1.f };	// アンビエント
		DirectX::XMFLOAT4  color{ 1.f,1.f,1.f,1.f };	// 色

		// 透視投影用
		DirectX::XMFLOAT4X4 perspective_projection{}; // ビュー行列
		DirectX::XMFLOAT4X4 perspective_view_projection{}; // ビュー・プロジェクション行列

		// 平行投影用
		float orthographic_view_width{ 200 }; // 幅
		float orthographic_view_height{ 200 };	// 高さ
		float orthographic_near_z{ 0 };	// 最近値
		float orthographic_far_z{ 100 };	// 最遠地
		DirectX::XMFLOAT4X4 orthographic_projection{};	// ビュー行列
		DirectX::XMFLOAT4X4 orthographic_view_projection{}; // ビュー・プロジェクション行列

	};

	explicit Light() {}
	explicit Light(ID3D11Device* device, const LightData& light_data);

	void Update(const View* view);

	[[nodiscard]] const DirectX::XMFLOAT4& GetLightDir() const { return direction; }
	void SetLightDir(const DirectX::XMFLOAT4& dir) { direction = dir; }
	[[nodiscard]] const DirectX::XMFLOAT4& GetAmbient() const { return ambient; }
	void SetAmbient(const DirectX::XMFLOAT4& ambient) { this->ambient = ambient; }
	[[nodiscard]] const DirectX::XMFLOAT4& GetColor() const { return color; }
	void SetColor(const DirectX::XMFLOAT4& color) { this->color = color; }

	void Position(const DirectX::XMFLOAT3& pos) { position = pos; }
	[[nodiscard]] const DirectX::XMFLOAT3& Position() const { return position; }

	[[nodiscard]] const DirectX::XMFLOAT4X4& LightViewProjectionMatrix() const { return light_view_projection_matrix; }

	[[nodiscard]] float GetOrthographicViewWidth() const { return data.orthographic_view_width; }
	[[nodiscard]] float GetOrthographicViewHeight() const { return data.orthographic_view_height; }
	[[nodiscard]] float GetOrthographicNearZ() const { return data.orthographic_near_z; }
	[[nodiscard]] float GetOrthographicFarZ() const { return data.orthographic_far_z; }

	[[noreturn]] void SetOrthographicViewWidth(const float width) { data.orthographic_view_width = width; }
	[[noreturn]] void SetOrthographicViewHeight(const float height) { data.orthographic_view_height = height; }
	[[noreturn]] void SetOrthographicNearZ(const float near_z) { data.orthographic_near_z = near_z; }
	[[noreturn]] void SetOrthographicFarZ(const float far_z) { data.orthographic_far_z = far_z; }

	void WriteImGui();

private:
	DirectX::XMFLOAT3  position{ 10.0f,20.0f,-10.0f };
	DirectX::XMFLOAT4  direction{ .0f,.0f,1.f,1.f };
	DirectX::XMFLOAT4  ambient{ 1.f,1.f,1.f,1.f };
	DirectX::XMFLOAT4  color{ 1.f,1.f,1.f,1.f };

	DirectX::SimpleMath::Vector3 view_right{ 1.f,.0f,.0f };
	DirectX::SimpleMath::Vector3 view_up{ .0f,1.f,.0f };
	DirectX::SimpleMath::Vector3 view_front{ .0f,.0f,1.f };

	DirectX::XMFLOAT4X4 light_view_matrix{};
	DirectX::XMFLOAT4X4 light_projection_matrix{};
	DirectX::XMFLOAT4X4 light_view_projection_matrix{};

	DirectX::XMFLOAT4X4 perspective_projection{};
	DirectX::XMFLOAT4X4 perspective_view_projection{};

	float orthographic_view_width{ 200 };
	float orthographic_view_height{ 200 };
	float orthographic_near_z{ 0 };
	float orthographic_far_z{ 100 };
	DirectX::XMFLOAT4X4 orthographic_projection{};
	DirectX::XMFLOAT4X4 orthographic_view_projection{};

	std::unique_ptr<buffer::ConstantBuffer<LightData>> cb_light{};
	LightData data{};

};

