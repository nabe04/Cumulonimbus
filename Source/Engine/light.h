#pragma once

#include <array>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

class View;

class Light
{
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

	float orthographic_view_width{200};
	float orthographic_view_height{200};
	float orthographic_near_z{0};
	float orthographic_far_z{100};
	DirectX::XMFLOAT4X4 orthographic_projection{};
	DirectX::XMFLOAT4X4 orthographic_view_projection{};

public:
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

	[[nodiscard]] float GetOrthographicViewWidth() const { return orthographic_view_width; }
	[[nodiscard]] float GetOrthographicViewHeight() const { return orthographic_view_height; }
	[[nodiscard]] float GetOrthographicNearZ() const { return orthographic_near_z; }
	[[nodiscard]] float GetOrthographicFarZ() const { return orthographic_far_z; }

	[[noreturn]] void SetOrthographicViewWidth(const float width) { orthographic_view_width = width; }
	[[noreturn]] void SetOrthographicViewHeight(const float height) { orthographic_view_height = height; }
	[[noreturn]] void SetOrthographicNearZ(const float near_z) { orthographic_near_z = near_z; }
	[[noreturn]] void SetOrthographicFarZ(const float far_z) { orthographic_far_z = far_z; }

	void WriteImGui();
};

