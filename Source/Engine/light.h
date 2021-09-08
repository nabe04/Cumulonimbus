#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>
#include "constant_buffer.h"
#include "shader_interop_renderer.h"

class Camera;

namespace cumulonimbus::component
{
	class CameraComponent;
} // cumulonimbus::component

class Light
{
public:
	explicit Light(ID3D11Device* device);
	explicit Light() = default;

	template<class Archive>
	void serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(cb_light),
			CEREAL_NVP(view_right),
			CEREAL_NVP(view_up),
			CEREAL_NVP(view_front)
		);
	}

	void Update(const cumulonimbus::component::CameraComponent* camera);
	void BindCBuffer(bool set_in_vs = true, bool set_in_ps = true) const;
	void UnbindCBuffer() const;

	[[nodiscard]] const DirectX::XMFLOAT3& GetPosition() const { return  cb_light->data.light_position; }
	[[nodiscard]] const DirectX::XMFLOAT3& GetLightDir() const { return cb_light->data.light_direction; }
	[[nodiscard]] const DirectX::XMFLOAT3& GetColor()	 const { return  cb_light->data.light_color; }
	[[nodiscard]] float GetOrthographicViewWidth()		 const { return cb_light->data.orthographic_view_width; }
	[[nodiscard]] float GetOrthographicViewHeight()		 const { return cb_light->data.orthographic_view_height; }
	[[nodiscard]] float GetOrthographicNearZ()			 const { return cb_light->data.orthographic_near_z; }
	[[nodiscard]] float GetOrthographicFarZ()			 const { return cb_light->data.orthographic_far_z; }


	void SetPosition(const DirectX::XMFLOAT3& pos) const { cb_light->data.light_position = pos; }
	void SetLightDir(const DirectX::XMFLOAT3& dir) const { cb_light->data.light_direction = dir; }
	void SetColor(const DirectX::XMFLOAT3& color)  const { cb_light->data.light_color = color; }

	void SetOrthographicViewWidth(const float width)	const { cb_light->data.orthographic_view_width = width; }
	void SetOrthographicViewHeight(const float height)	const { cb_light->data.orthographic_view_height = height; }
	void SetOrthographicNearZ(const float near_z)		const { cb_light->data.orthographic_near_z = near_z; }
	void SetOrthographicFarZ(const float far_z)			const { cb_light->data.orthographic_far_z = far_z; }

	[[nodiscard]] const LightCB& GetData()  const { return cb_light->data; }
	void SetData(const LightCB& light_data) const { cb_light->data = light_data; }

	void Load() const;
	void WriteImGui() const;

private:
	std::unique_ptr<buffer::ConstantBuffer<LightCB>> cb_light{};

	DirectX::SimpleMath::Vector3 view_right{ 1.f,.0f,.0f };
	DirectX::SimpleMath::Vector3 view_up{ .0f,1.f,.0f };
	DirectX::SimpleMath::Vector3 view_front{ .0f,.0f,1.f };
};

