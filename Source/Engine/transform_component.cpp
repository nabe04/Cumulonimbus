#include "transform_component.h"

#include <fstream>
#include <imgui.h>
#include <string>

#include <cereal/archives/json.hpp>
#include <cereal/types/bitset.hpp>

#include "arithmetic.h"
#include "cereal_helper.h"
#include "component_base.h"
#include "file_path_helper.h"
#include "locator.h"

using namespace DirectX;

namespace cumulonimbus::component
{
	using namespace DirectX::SimpleMath;

	TransformComponent::TransformComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent)
		: ComponentBase{ registry,ent}
	{
		cb_transform = std::make_shared<buffer::ConstantBuffer<TransformCB>>(locator::Locator::GetDx11Device()->device.Get());

		// Initialize XMFLOAT4x4
		CreateIdentity4x4(&scaling_matrix);
		CreateIdentity4x4(&rotation_matrix);
		CreateIdentity4x4(&translation_matrix);

		set_angle_bit_flg.reset();
	}

	void TransformComponent::NewFrame(const float delta_time)
	{
		SetOldPosition(GetPosition());
	}

	void TransformComponent::Update(const float delta_time)
	{
		NormalizeAngle();
		CreateWorldTransformMatrix();
	}

	void TransformComponent::RenderImGui()
	{
		if (ImGui::TreeNode("TransformComponent"))
		{
			ImGui::Text("Right X %f", model_right.x);
			ImGui::Text("Right Y %f", model_right.y);
			ImGui::Text("Right Z %f", model_right.z);

			ImGui::Text("Up X %f", model_up.x);
			ImGui::Text("Up Y %f", model_up.y);
			ImGui::Text("Up Z %f", model_up.z);

			ImGui::Text("Front X %f", model_front.x);
			ImGui::Text("Front Y %f", model_front.y);
			ImGui::Text("Front Z %f", model_front.z);

			ImGui::DragFloat3("Position", (float*)&position, 0.01f, -100.0f, 100.0f);
			ImGui::DragFloat3("Scale", (float*)&scale, 0.01f, -100.0f, 100.0f);
			ImGui::SliderFloat("Rotation X", &angle.x, -180.0f, 180.0f, "%.3f");
			ImGui::SliderFloat("Rotation Y", &angle.y, -180.0f, 180.0f, "%.3f");
			ImGui::SliderFloat("Rotation Z", &angle.z, -180.0f, 180.0f, "%.3f");

			ImGui::TreePop();
		}
	}

	void TransformComponent::SetTransformCB(const TransformCB transform) const
	{
		cb_transform->data = transform;
	}

	void TransformComponent::BindCBuffer(bool set_in_vs, bool set_in_ps) const
	{
		cb_transform->Activate(locator::Locator::GetDx11Device()->immediate_context.Get(), CBSlot_Transform, set_in_vs, set_in_ps);
	}

	void TransformComponent::UnbindCBuffer()
	{
		cb_transform->Deactivate(locator::Locator::GetDx11Device()->immediate_context.Get());
	}

	void TransformComponent::SetAndBindCBuffer(const TransformCB& transform, bool set_in_vs, bool set_in_ps) const
	{
		SetTransformCB(transform);
		BindCBuffer(set_in_vs, set_in_ps);
	}

	void TransformComponent::CreateScaling4x4()
	{
		// Scaling
		XMMATRIX s = XMMatrixScaling(scale.x, scale.y, scale.z);

		XMStoreFloat4x4(&scaling_matrix, s);
	}

	void TransformComponent::CreateRotation4x4()
	{
		// Rotation
		XMMATRIX r = XMMatrixIdentity();
		if (is_billboard)
		{
			return;
		}
		else if (is_quaternion)
		{
			//AdjustRotationFromAxis({ 0,1,0 }, XMConvertToRadians(1));
			r = XMMatrixRotationQuaternion(XMLoadFloat4(&rotation_quaternion));
			angle = arithmetic::QuaternionToEulerAngle(rotation_quaternion);
			angle.x = XMConvertToDegrees(angle.x);
			angle.y = XMConvertToDegrees(angle.y);
			angle.z = XMConvertToDegrees(angle.z);
		}
		else
		{
			r = XMMatrixRotationRollPitchYaw(XMConvertToRadians(angle.x), XMConvertToRadians(angle.y), XMConvertToRadians(angle.z));
		}

		XMStoreFloat4x4(&rotation_matrix, r);
	}

	void TransformComponent::CreateTranslation4x4()
	{
		// Parallel movement
		XMMATRIX t = XMMatrixTranslation(position.x, position.y, position.z);

		XMStoreFloat4x4(&translation_matrix, t);
	}

	void TransformComponent::CalcModelCoordinateAxis(const XMFLOAT4X4& orientation)
	{
		model_right = XMFLOAT3{ orientation._11,orientation._12,orientation._13 };
		model_up = XMFLOAT3{ orientation._21,orientation._22,orientation._23 };
		model_front = XMFLOAT3{ orientation._31,orientation._32,orientation._33 };
	}

	void TransformComponent::CreateWorldTransformMatrix()
	{
		CreateScaling4x4();
		CreateRotation4x4();
		CreateTranslation4x4();

		//-- Create world transform matrix --//
		// Scaling
		XMMATRIX s = XMLoadFloat4x4(&scaling_matrix);
		// Rotation
		XMMATRIX r = XMLoadFloat4x4(&rotation_matrix);
		// Parallel movement
		XMMATRIX t = XMLoadFloat4x4(&translation_matrix);

		// Matrix synthesis
		XMMATRIX world_matrix = s * r * t;
		XMStoreFloat4x4(&world_f4x4, world_matrix);

		model_right = XMFLOAT3{ rotation_matrix._11,rotation_matrix._12,rotation_matrix._13 };
		model_up	= XMFLOAT3{ rotation_matrix._21,rotation_matrix._22,rotation_matrix._23 };
		model_front = XMFLOAT3{ rotation_matrix._31,rotation_matrix._32,rotation_matrix._33 };

		prev_angle = angle;
	}

	void TransformComponent::NormalizeAngle()
	{
		// Angle X
		if (angle.x > 180.0f)
		{
			angle.x -= 360.0f;
		}
		else if (angle.x < -180.0f)
		{
			angle.x += 360.0f;
		}

		// Angle Y
		if (angle.y > 180.0f)
		{
			angle.y -= 360.0f;
		}
		else if (angle.y < -180.0f)
		{
			angle.y += 360.0f;
		}

		// Angle Z
		if (angle.z > 180.0f)
		{
			angle.z -= 360.0f;
		}
		else if (angle.z < -180.0f)
		{
			angle.z += 360.0f;
		}
	}

	void TransformComponent::CreateIdentity4x4(XMFLOAT4X4* convert)
	{
		XMMATRIX convert_matrix = XMMatrixIdentity();

		XMStoreFloat4x4(convert, convert_matrix);
	}

	void TransformComponent::GetBillboardRotation(const XMFLOAT3 billPos, const XMFLOAT3 targetPos)
	{
		if (!is_billboard)
			assert(!"Billboard passive !!");

		XMMATRIX rotatin_matrix = XMMatrixIdentity();
		FXMVECTOR bill_vec = XMVectorSet(billPos.x, billPos.y, billPos.z, 1);
		FXMVECTOR target_vec = XMVectorSet(targetPos.x, targetPos.y, targetPos.z, 1);
		rotatin_matrix = XMMatrixLookAtLH(bill_vec, target_vec, FXMVECTOR{ 0,1,0 });
		rotatin_matrix = XMMatrixInverse(nullptr, rotatin_matrix);

		XMStoreFloat4x4(&rotation_matrix, rotatin_matrix);
		//rotation_f4x4._41 = rotation_f4x4._42 = rotation_f4x4._43 = rotation_f4x4._44 = 0;
	}

	void TransformComponent::AdjustRotationFromAxis(const DirectX::SimpleMath::Vector3& axis, float angle)
	{
		SimpleMath::Quaternion q = SimpleMath::Quaternion::CreateFromAxisAngle(axis, angle);
		rotation_quaternion *= q;
	}


	XMMATRIX TransformComponent::GetRotationMatrix(XMFLOAT3 axis, float angle/* degree */)
	{
		XMVECTOR axis_vec = XMLoadFloat3(&axis);
		XMVECTOR calc_val = XMQuaternionRotationAxis(axis_vec, XMConvertToRadians(angle));

		return XMMatrixRotationQuaternion(calc_val);
	}

	void TransformComponent::Save(const std::string& file_path)
	{
		const std::string file_path_and_name = file_path + file_path_helper::GetTypeName<TransformComponent>() + file_path_helper::GetJsonExtension();
		std::ofstream ofs(file_path_and_name);
		cereal::JSONOutputArchive o_archive(ofs);
		o_archive(*this);
	}

	void TransformComponent::Load(const std::string& file_path_and_name)
	{
		{
			std::ifstream ifs(file_path_and_name);
			cereal::JSONInputArchive i_archive(ifs);
			i_archive(*this);
		}
	}
}
