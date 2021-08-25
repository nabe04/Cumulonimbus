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

			ImGui::DragFloat3("Position", (float*)&position, 0.01f, -10000.0f, 10000.0f);
			ImGui::DragFloat3("Scale", (float*)&scale, 0.01f, -1000.0f, 1000.0f);
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
		DirectX::XMMATRIX s = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

		XMStoreFloat4x4(&scaling_matrix, s);
	}

	void TransformComponent::CreateRotation4x4()
	{
		// Rotation
		DirectX::XMMATRIX r = DirectX::XMMatrixIdentity();
		if (is_billboard)
		{
			return;
		}
		else if (is_quaternion)
		{
			//AdjustRotationFromAxis({ 0,1,0 }, XMConvertToRadians(1));
			r = DirectX::XMMatrixRotationQuaternion(XMLoadFloat4(&rotation_quaternion));
			angle   = arithmetic::QuaternionToEulerAngle(rotation_quaternion);
			angle.x = DirectX::XMConvertToDegrees(angle.x);
			angle.y = DirectX::XMConvertToDegrees(angle.y);
			angle.z = DirectX::XMConvertToDegrees(angle.z);
		}
		else
		{
			r = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(angle.x), DirectX::XMConvertToRadians(angle.y), DirectX::XMConvertToRadians(angle.z));
		}

		XMStoreFloat4x4(&rotation_matrix, r);
	}

	void TransformComponent::CreateTranslation4x4()
	{
		// Parallel movement
		DirectX::XMMATRIX t = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

		XMStoreFloat4x4(&translation_matrix, t);
	}

	void TransformComponent::CalcModelCoordinateAxis(const DirectX::XMFLOAT4X4& orientation)
	{
		model_right = DirectX::XMFLOAT3{ orientation._11,orientation._12,orientation._13 };
		model_up	= DirectX::XMFLOAT3{ orientation._21,orientation._22,orientation._23 };
		model_front = DirectX::XMFLOAT3{ orientation._31,orientation._32,orientation._33 };
	}

	void TransformComponent::CreateWorldTransformMatrix()
	{
		CreateScaling4x4();
		CreateRotation4x4();
		CreateTranslation4x4();

		//-- Create world transform matrix --//
		// Scaling
		DirectX::XMMATRIX s = XMLoadFloat4x4(&scaling_matrix);
		// Rotation
		DirectX::XMMATRIX r = XMLoadFloat4x4(&rotation_matrix);
		// Parallel movement
		DirectX::XMMATRIX t = XMLoadFloat4x4(&translation_matrix);

		// Matrix synthesis
		DirectX::XMMATRIX world_matrix = s * r * t;
		XMStoreFloat4x4(&world_f4x4, world_matrix);

		model_right = DirectX::XMFLOAT3{ rotation_matrix._11,rotation_matrix._12,rotation_matrix._13 };
		model_up	= DirectX::XMFLOAT3{ rotation_matrix._21,rotation_matrix._22,rotation_matrix._23 };
		model_front = DirectX::XMFLOAT3{ rotation_matrix._31,rotation_matrix._32,rotation_matrix._33 };

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

	void TransformComponent::CreateIdentity4x4(DirectX::XMFLOAT4X4* convert)
	{
		DirectX::XMMATRIX convert_matrix = DirectX::XMMatrixIdentity();

		XMStoreFloat4x4(convert, convert_matrix);
	}

	void TransformComponent::GetBillboardRotation(const DirectX::XMFLOAT3 billPos, const DirectX::XMFLOAT3 targetPos)
	{
		if (!is_billboard)
			assert(!"Billboard passive !!");

		DirectX::XMMATRIX rotatin_matrix = DirectX::XMMatrixIdentity();
		DirectX::FXMVECTOR bill_vec = DirectX::XMVectorSet(billPos.x, billPos.y, billPos.z, 1);
		DirectX::FXMVECTOR target_vec = DirectX::XMVectorSet(targetPos.x, targetPos.y, targetPos.z, 1);
		rotatin_matrix = DirectX::XMMatrixLookAtLH(bill_vec, target_vec, DirectX::FXMVECTOR{ 0,1,0 });
		rotatin_matrix = XMMatrixInverse(nullptr, rotatin_matrix);

		XMStoreFloat4x4(&rotation_matrix, rotatin_matrix);
		//rotation_f4x4._41 = rotation_f4x4._42 = rotation_f4x4._43 = rotation_f4x4._44 = 0;
	}

	void TransformComponent::SetQuaternionSlerp(const DirectX::SimpleMath::Quaternion& q1, const DirectX::SimpleMath::Quaternion& q2)
	{
		rotation_prev_quaternion	= q1;
		rotation_result_quaternion	= q2;
	}

	void TransformComponent::QuaternionSlerp(const float t)
	{
		rotation_quaternion = Quaternion::Slerp(rotation_prev_quaternion, rotation_result_quaternion, t);
	}

	void TransformComponent::AdjustRotationFromAxis(const DirectX::SimpleMath::Vector3& axis, const float angle)
	{
		const Quaternion q = Quaternion::CreateFromAxisAngle(axis, angle);
		rotation_quaternion *= q;
	}

	DirectX::XMMATRIX TransformComponent::GetRotationMatrix(DirectX::XMFLOAT3 axis, float angle/* degree */)
	{
		const DirectX::XMVECTOR axis_vec = XMLoadFloat3(&axis);
		const DirectX::XMVECTOR calc_val = DirectX::XMQuaternionRotationAxis(axis_vec, DirectX::XMConvertToRadians(angle));

		return DirectX::XMMatrixRotationQuaternion(calc_val);
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
