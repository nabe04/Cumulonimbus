#include "transform_component.h"

#include <fstream>
#include <imgui.h>

#include "arithmetic.h"
#include "component_base.h"
#include "locator.h"
#include "cum_imgui_helper.h"
#include "ecs.h"
#include "scene.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::TransformComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::TransformComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::TransformComponent, 0);

namespace cumulonimbus::component
{
	using namespace DirectX::SimpleMath;

	//template <class Archive>
	//void TransformComponent::serialize(Archive&& archive)
	//{
	//	archive(
	//		cereal::base_class<ComponentBase>(this),
	//		CEREAL_NVP(local_position),
	//		CEREAL_NVP(local_prev_pos),
	//		CEREAL_NVP(local_scale),
	//		CEREAL_NVP(local_angle),
	//		CEREAL_NVP(local_prev_angle),

	//		CEREAL_NVP(world_f4x4),
	//		CEREAL_NVP(scaling_matrix),
	//		CEREAL_NVP(rotation_matrix),
	//		CEREAL_NVP(translation_matrix),

	//		CEREAL_NVP(model_right),
	//		CEREAL_NVP(model_front),
	//		CEREAL_NVP(model_up),
	//		CEREAL_NVP(orientation),

	//		// Quaternion
	//		CEREAL_NVP(rotation_quaternion),

	//		CEREAL_NVP(is_quaternion)

	//	);
	//}

	template <class Archive>
	void TransformComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<ComponentBase>(this),
			CEREAL_NVP(local_position),
			CEREAL_NVP(local_prev_pos),
			CEREAL_NVP(local_scale),
			CEREAL_NVP(local_angle),
			CEREAL_NVP(local_prev_angle),

			CEREAL_NVP(world_f4x4),
			CEREAL_NVP(scaling_matrix),
			CEREAL_NVP(rotation_matrix),
			CEREAL_NVP(translation_matrix),

			CEREAL_NVP(model_right),
			CEREAL_NVP(model_front),
			CEREAL_NVP(model_up),
			CEREAL_NVP(orientation),

			// Quaternion
			CEREAL_NVP(rotation_quaternion),

			CEREAL_NVP(is_quaternion)

		);
	}

	template <class Archive>
	void TransformComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<ComponentBase>(this),
			CEREAL_NVP(local_position),
			CEREAL_NVP(local_prev_pos),
			CEREAL_NVP(local_scale),
			CEREAL_NVP(local_angle),
			CEREAL_NVP(local_prev_angle),

			CEREAL_NVP(world_f4x4),
			CEREAL_NVP(scaling_matrix),
			CEREAL_NVP(rotation_matrix),
			CEREAL_NVP(translation_matrix),

			CEREAL_NVP(model_right),
			CEREAL_NVP(model_front),
			CEREAL_NVP(model_up),
			CEREAL_NVP(orientation),

			// Quaternion
			CEREAL_NVP(rotation_quaternion),

			CEREAL_NVP(is_quaternion)

		);
	}

	TransformComponent::TransformComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent)
		: ComponentBase{ registry,ent}
	{
		cb_transform = std::make_unique<buffer::ConstantBuffer<TransformCB>>(locator::Locator::GetDx11Device()->device.Get());

		// Initialize XMFLOAT4x4
		CreateIdentity4x4(scaling_matrix);
		CreateIdentity4x4(rotation_matrix);
		CreateIdentity4x4(translation_matrix);
	}

	TransformComponent::TransformComponent(ecs::Registry* registry, const mapping::rename_type::Entity ent, const TransformComponent& copy_comp)
		:ComponentBase{ registry,ent }
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	TransformComponent::TransformComponent(const TransformComponent& other)
		:ComponentBase{other},
		 local_position{other.GetPosition()},
		 local_prev_pos{other.GetOldPosition()},
		 local_scale{other.GetScale()},
		 local_angle{other.GetWorldRotation()},
		 world_f4x4{other.GetWorld4x4()},
		 scaling_matrix{other.GetScalingMat()},
		 rotation_matrix{other.GetRotationMat()},
		 translation_matrix{other.GetTranslationMat()},
		 orientation{DirectX::SimpleMath::Matrix::Identity},
		 rotation_quaternion{other.GetRotationQuaternion()},
		 rotation_result_quaternion{other.GetRotationResultQuaternion()},
		 is_quaternion{other.IsQuaternion()}
	{
		if (cb_transform)
			cb_transform.reset();

		cb_transform = std::make_unique<buffer::ConstantBuffer<TransformCB>>(locator::Locator::GetDx11Device()->device.Get());
	}

	TransformComponent& TransformComponent::operator=(const TransformComponent& other)
	{
		if(this == &other)
		{
			return *this;
		}

		local_position					= other.GetPosition();
		local_prev_pos					= other.GetOldPosition();
		local_scale						= other.GetScale();
		local_angle						= other.GetWorldRotation();
		world_f4x4					= other.GetWorld4x4();
		scaling_matrix				= other.GetScalingMat();
		rotation_matrix				= other.GetRotationMat();
		translation_matrix			= other.GetTranslationMat();
		orientation					= DirectX::SimpleMath::Matrix::Identity;
		rotation_quaternion			= other.GetRotationQuaternion();
		rotation_result_quaternion	= other.GetRotationResultQuaternion();
		is_quaternion				= other.IsQuaternion();

		if (cb_transform)
			cb_transform.reset();
		cb_transform = std::make_unique<buffer::ConstantBuffer<TransformCB>>(locator::Locator::GetDx11Device()->device.Get());

		return *this;
	}

	void TransformComponent::PreCommonUpdate(float dt)
	{
		NormalizeAngle();
		CreateWorldTransformMatrix();
	}

	void TransformComponent::CommonUpdate(float dt)
	{

	}

	void TransformComponent::PostCommonUpdate(float dt)
	{
		local_prev_pos = local_position;
	}

	void TransformComponent::SceneUpdate(float dt)
	{

	}

	void TransformComponent::PreGameUpdate(const float dt)
	{

	}

	void TransformComponent::GameUpdate(const float dt)
	{

	}

	void TransformComponent::RenderImGui()
	{
		if (ImGui::CollapsingHeader("TransformComponent", ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto& camera = GetRegistry()->GetScene()->GetEditorManager()->GetSceneView().GetSceneViewCamera();

			DirectX::SimpleMath::Matrix world_mat{};
			DirectX::SimpleMath::Matrix view_mat = camera.GetCamera().GetViewMat();
			DirectX::SimpleMath::Matrix proj_mat = camera.GetCamera().GetProjectionMat();

			//ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, window_width, window_height);
			IMGUI_LEFT_LABEL(ImGui::DragFloat3, "Position", reinterpret_cast<float*>(&local_position), 0.01f, -10000.0f, 10000.0f);
			IMGUI_LEFT_LABEL(ImGui::DragFloat3, "Scale   ", reinterpret_cast<float*>(&local_scale)	 , 0.01f, -10000.0f, 10000.0f);
			IMGUI_LEFT_LABEL(ImGui::DragFloat3, "Rotation", &local_angle.x	 , 0.01f, -180.0f  , 180.0f);

			ImGui::Text("Right X %f", model_right.x);
			ImGui::Text("Right Y %f", model_right.y);
			ImGui::Text("Right Z %f", model_right.z);

			ImGui::Text("Up X %f", model_up.x);
			ImGui::Text("Up Y %f", model_up.y);
			ImGui::Text("Up Z %f", model_up.z);

			ImGui::Text("Front X %f", model_front.x);
			ImGui::Text("Front Y %f", model_front.y);
			ImGui::Text("Front Z %f", model_front.z);
		}
	}

	void TransformComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);

		cb_transform = std::make_unique<buffer::ConstantBuffer<TransformCB>>(locator::Locator::GetDx11Device()->device.Get());

		// Initialize XMFLOAT4x4
		CreateIdentity4x4(scaling_matrix);
		CreateIdentity4x4(rotation_matrix);
		CreateIdentity4x4(translation_matrix);
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
		DirectX::XMMATRIX s = DirectX::XMMatrixScaling(local_scale.x, local_scale.y, local_scale.z);

		XMStoreFloat4x4(&scaling_matrix, s);
		// Constant Bufferのセット
		cb_transform->data.scaling_matrix = scaling_matrix;
	}

	void TransformComponent::CreateRotation4x4()
	{


		// Rotation
		DirectX::XMMATRIX r = DirectX::XMMatrixIdentity();
		if (is_quaternion)
		{
			//AdjustRotationFromAxis({ 0,1,0 }, XMConvertToRadians(1));
			rotation_quaternion.Normalize();
			r = DirectX::XMMatrixRotationQuaternion(XMLoadFloat4(&rotation_quaternion));
			local_angle   = arithmetic::QuaternionToEulerAngle(rotation_quaternion);
			local_angle.x = DirectX::XMConvertToDegrees(local_angle.x);
			local_angle.y = DirectX::XMConvertToDegrees(local_angle.y);
			local_angle.z = DirectX::XMConvertToDegrees(local_angle.z);
		}
		else
		{
			r = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(local_angle.x), DirectX::XMConvertToRadians(local_angle.y), DirectX::XMConvertToRadians(local_angle.z));
		}

		XMStoreFloat4x4(&rotation_matrix, r);
		//DirectX::XMMatrix
		//DirectX::S
		//rotation_matrix.
		// Constant Bufferのセット
		cb_transform->data.rotation_matrix = rotation_matrix;
	}

	void TransformComponent::CreateTranslation4x4()
	{
		// Parallel movement
		DirectX::XMMATRIX t = DirectX::XMMatrixTranslation(local_position.x, local_position.y, local_position.z);

		XMStoreFloat4x4(&translation_matrix, t);
		// Constant Bufferのセット
		cb_transform->data.translation_matrix = translation_matrix;
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
		const DirectX::XMMATRIX s = XMLoadFloat4x4(&scaling_matrix);
		// Rotation
		const DirectX::XMMATRIX r = XMLoadFloat4x4(&rotation_matrix);
		// Parallel movement
		const DirectX::XMMATRIX t = XMLoadFloat4x4(&translation_matrix);

		// Matrix synthesis
		const DirectX::XMMATRIX matrix = s * r * t;
		XMStoreFloat4x4(&world_f4x4, matrix);
		// Constant Bufferのセット
		cb_transform->data.transform_matrix = world_f4x4;

		model_right = DirectX::XMFLOAT3{ rotation_matrix._11,rotation_matrix._12,rotation_matrix._13 };
		model_up	= DirectX::XMFLOAT3{ rotation_matrix._21,rotation_matrix._22,rotation_matrix._23 };
		model_front = DirectX::XMFLOAT3{ rotation_matrix._31,rotation_matrix._32,rotation_matrix._33 };

		local_prev_angle = local_angle;
	}

	void TransformComponent::NormalizeAngle()
	{
		// Angle X
		if (local_angle.x > 180.0f)
		{
			local_angle.x -= 360.0f;
		}
		else if (local_angle.x < -180.0f)
		{
			local_angle.x += 360.0f;
		}

		// Angle Y
		if (local_angle.y > 180.0f)
		{
			local_angle.y -= 360.0f;
		}
		else if (local_angle.y < -180.0f)
		{
			local_angle.y += 360.0f;
		}

		// Angle Z
		if (local_angle.z > 180.0f)
		{
			local_angle.z -= 360.0f;
		}
		else if (local_angle.z < -180.0f)
		{
			local_angle.z += 360.0f;
		}
	}

	void TransformComponent::CreateIdentity4x4(DirectX::SimpleMath::Matrix& convert)
	{
		convert = DirectX::SimpleMath::Matrix::Identity;
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

	void TransformComponent::AdjustRotationFromAxis(const DirectX::SimpleMath::Vector3& axis, const float local_angle)
	{
		const Quaternion q = Quaternion::CreateFromAxisAngle(axis, local_angle);
		rotation_quaternion *= q;
	}

	DirectX::XMMATRIX TransformComponent::GetRotationMatrix(DirectX::XMFLOAT3 axis, float local_angle/* degree */)
	{
		const DirectX::XMVECTOR axis_vec = XMLoadFloat3(&axis);
		const DirectX::XMVECTOR calc_val = DirectX::XMQuaternionRotationAxis(axis_vec, DirectX::XMConvertToRadians(local_angle));

		return DirectX::XMMatrixRotationQuaternion(calc_val);
	}
} // cumulonimbus::component
