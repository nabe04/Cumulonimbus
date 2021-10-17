#include "transform_component.h"

#include <fstream>
#include <imgui.h>

#include "arithmetic.h"
#include "component_base.h"
#include "locator.h"
#include "cum_imgui_helper.h"
#include "ecs.h"
#include "scene.h"
// components
#include "hierarchy_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::TransformComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::TransformComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::TransformComponent, 0);

namespace cumulonimbus::component
{
	using namespace DirectX::SimpleMath;

	template <class Archive>
	void TransformComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<ComponentBase>(this),
			CEREAL_NVP(local_position),
			CEREAL_NVP(local_prev_position),
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
			CEREAL_NVP(local_prev_position),
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
		 local_position{other.local_position},
		 local_prev_position{other.local_prev_position},
		 local_scale{other.local_scale},
		 local_angle{other.local_angle},
		 world_f4x4{ other.world_f4x4 },
		 scaling_matrix{other.scaling_matrix},
		 rotation_matrix{ other.rotation_matrix },
		 translation_matrix{other.translation_matrix},
		 orientation{DirectX::SimpleMath::Matrix::Identity},
		 rotation_quaternion{other.rotation_quaternion},
		 rotation_result_quaternion{other.rotation_result_quaternion},
		 is_quaternion{other.is_quaternion}
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

		local_position				= other.local_position;
		local_prev_position			= other.local_prev_position;
		local_scale					= other.local_scale;
		local_angle					= other.local_angle;
		world_f4x4					= other.world_f4x4;
		scaling_matrix				= other.scaling_matrix;
		rotation_matrix				= other.rotation_matrix;
		translation_matrix			= other.translation_matrix;
		orientation					= DirectX::SimpleMath::Matrix::Identity;
		rotation_quaternion			= other.rotation_quaternion;
		rotation_result_quaternion	= other.rotation_result_quaternion;
		is_quaternion				= other.is_quaternion;

		if (cb_transform)
			cb_transform.reset();
		cb_transform = std::make_unique<buffer::ConstantBuffer<TransformCB>>(locator::Locator::GetDx11Device()->device.Get());

		return *this;
	}

	void TransformComponent::PreCommonUpdate(float dt)
	{

	}

	void TransformComponent::CommonUpdate(float dt)
	{
		//if(GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).GetIsDirty())
		//{
		//	NormalizeAngle();
		//	CreateWorldTransformMatrix();
		//}
	}

	void TransformComponent::PostCommonUpdate(float dt)
	{
		local_prev_position	 = local_position;
		local_prev_scale	 = local_scale;
		local_prev_angle	 = local_angle;
	}

	void TransformComponent::SceneUpdate(float dt)
	{
		//using namespace DirectX::SimpleMath;
		//// 90,-60,30
		//const auto& rotation_x = Matrix::CreateRotationX(DirectX::XMConvertToRadians(-129));
		//const auto& rotation_y = Matrix::CreateRotationY(DirectX::XMConvertToRadians(35));
		//const auto& rotation_z = Matrix::CreateRotationZ(DirectX::XMConvertToRadians(150));
		//const auto& srt_mat = Matrix::CreateScale(1, 1, 1) * rotation_z * rotation_y * rotation_x * Matrix::CreateTranslation(0, 0, 0);
		//const Matrix& mat = { Matrix::Identity };
		////DirectX::XMMatrixRotationRollPitchYawFromVector()

		//const auto& m = mat * srt_mat;

		//Vector3 v_after = { m._11,m._12,m._13 };
		//Vector3 v_before{ 1,0,0 };
		//ImGui::TreeNodeEx("", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow);
		//arithmetic::AxisFlags flg{ arithmetic::Axis_X | arithmetic::Axis_Y };
		//auto result_xyz = arithmetic::CalcAngleFromMatrix(m, arithmetic::UseAllAxis);
		//auto result_xy = arithmetic::CalcAngleFromMatrix(m, arithmetic::Axis_X | arithmetic::Axis_Y);
		//auto result_yz = arithmetic::CalcAngleFromMatrix(m, arithmetic::Axis_Z | arithmetic::Axis_Y);

		//float d = v_after.Dot(v_before);
		//float cos = acosf(d);
		//float angle = DirectX::XMConvertToDegrees(cos);
		//int a;
		//a = 0;
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

			// Transformに動きがあればダーティフラグをセット
			if(GetDeltaPosition() != DirectX::SimpleMath::Vector3{})
			{
				GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
			}
			if(GetDeltaScale() != DirectX::SimpleMath::Vector3{})
			{
				GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
			}
			if(GetDeltaAngle() != DirectX::SimpleMath::Vector3{})
			{
				GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
			}
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

	const DirectX::SimpleMath::Matrix& TransformComponent::GetWorld4x4()
	{
		if(auto& hierarchy_comp = GetRegistry()->GetComponent<HierarchyComponent>(GetEntity());
			hierarchy_comp.GetIsDirty())
		{
			NormalizeAngle();
			CreateWorldTransformMatrix();

			if (!hierarchy_comp.GetParentEntity().empty())
			{
				const auto& parent_ent = GetRegistry()->GetComponent<TransformComponent>(hierarchy_comp.GetParentEntity()).GetWorld4x4();
				world_f4x4 = parent_ent * world_f4x4;
			}

			hierarchy_comp.DeactivateDirtyFlg();
		}

		return world_f4x4;
	}


	void TransformComponent::CreateIdentity4x4(DirectX::SimpleMath::Matrix& convert)
	{
		convert = DirectX::SimpleMath::Matrix::Identity;
	}

	void TransformComponent::SetPosition(const DirectX::SimpleMath::Vector3& pos)
	{
		local_position = pos;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::SetPositionX(const float x)
	{
		local_position.x = x;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::SetPositionY(float y)
	{
		local_position.y = y;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::SetPositionZ(float z)
	{
		local_position.z = z;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::AdjustPosition(const DirectX::SimpleMath::Vector3& adjust_val)
	{
		local_position += adjust_val;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::AdjustPositionX(float x)
	{
		local_position.x += x;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::AdjustPositionY(float y)
	{
		local_position.y += y;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::AdjustPositionZ(float z)
	{
		local_position.z += z;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::SetScale(const DirectX::SimpleMath::Vector3& scale)
	{
		local_scale = scale;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::SetScale(const float scale)
	{
		local_scale.x = local_scale.y = local_scale.z = scale;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::SetScaleX(const float x)
	{
		local_scale.x = x;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::SetScaleY(const float y)
	{
		local_scale.y = y;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::SetScaleZ(const float z)
	{
		local_scale.z = z;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::AdjustScale(const DirectX::SimpleMath::Vector3& adjust_val)
	{
		local_scale += adjust_val;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::AdjustScale(const float adjust_val)
	{
		local_scale.x += adjust_val;
		local_scale.y += adjust_val;
		local_scale.z += adjust_val;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::AdjustScaleX(float x)
	{
		local_scale.x += x;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::AdjustScaleY(float y)
	{
		local_scale.y += y;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::AdjustScaleZ(float z)
	{
		local_scale.z += z;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::SetWorldRotation(const DirectX::SimpleMath::Vector3& angle)
	{
		local_angle = angle;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::SetWorldRotation(const float angle)
	{
		local_angle.x = local_angle.y = local_angle.z = angle;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::SetWorldRotation_X(const float x)
	{
		local_angle.x = x;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::SetWorldRotation_Y(const float y)
	{
		local_angle.y = y;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::SetWorldRotation_Z(const float z)
	{
		local_angle.z = z;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::AdjustWorldRotation(const DirectX::SimpleMath::Vector3& adjust_val)
	{
		local_angle += adjust_val;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::AdjustWorldRotation(const float adjust_val)
	{
		local_angle.x += adjust_val;
		local_angle.y += adjust_val;
		local_angle.z += adjust_val;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::AdjustWorldRotation_X(const float x)
	{
		local_angle.x += x;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::AdjustWorldRotation_Y(const float y)
	{
		local_angle.y += y;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::AdjustWorldRotation_Z(const float z)
	{
		local_angle.z += z;
		// ダーティフラグをセット
		GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
	}

	void TransformComponent::SetWorld4x4(const DirectX::SimpleMath::Matrix& mat4x4)
	{
		NormalizeAngle();
		CreateWorldTransformMatrix();

		world_f4x4 = mat4x4;
		// Constant Bufferのセット
		cb_transform->data.transform_matrix = world_f4x4;

		//// ダーティフラグをセット
		//GetRegistry()->GetComponent<HierarchyComponent>(GetEntity()).ActivateDirtyFlg();
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
