#pragma once

#include <array>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

class View;

namespace light
{
	//struct SpotLight
	//{
	//	int index;
	//	float range;		// Range of light
	//	float type;			// Valid or invalid
	//	float near_area;
	//	float far_area;
	//	//float dumy0;
	//	//float dumy1;
	//	//float dumy2;
	//	XMFLOAT4 position;
	//	XMFLOAT4 color;
	//	XMFLOAT4 direction;
	//};
}
class Light
{
private:
	DirectX::XMFLOAT3  position  = { 10.0f,20.0f,-10.0f };
	DirectX::XMFLOAT4  direction = { .0f,.0f,1.f,1.f };
	DirectX::XMFLOAT4  ambient	 = { 1.f,1.f,1.f,1.f };
	DirectX::XMFLOAT4  color	 = { 1.f,1.f,1.f,1.f };

	DirectX::SimpleMath::Vector3 view_right{ 1.f,.0f,.0f };
	DirectX::SimpleMath::Vector3 view_up{ .0f,1.f,.0f };
	DirectX::SimpleMath::Vector3 view_front{ .0f,.0f,1.f };

	DirectX::XMFLOAT4X4 light_view_matrix;
	DirectX::XMFLOAT4X4 light_projection_matrix;
	DirectX::XMFLOAT4X4 light_view_projection_matrix;


public:
	void Update(const View* view);

	const DirectX::XMFLOAT4& GetLightDir() { return direction; }
	void SetLightDir(const DirectX::XMFLOAT4& dir) { direction = dir; }
	const DirectX::XMFLOAT4& GetAmbient() { return ambient; }
	void SetAmbient(const DirectX::XMFLOAT4& ambient) { this->ambient = ambient; }
	const DirectX::XMFLOAT4& GetColor() { return color; }
	void SetColor(const DirectX::XMFLOAT4& color) { this->color = color; }

	void Position(const DirectX::XMFLOAT3& pos) { position = pos; }
	const DirectX::XMFLOAT3& Position() { return position; }

	const DirectX::XMFLOAT4X4& LightViewProjectionMatrix() { return light_view_projection_matrix; }

	void WriteImGui();
};

