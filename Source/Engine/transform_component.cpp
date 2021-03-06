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
#include "model_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::TransformComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::TransformComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::TransformComponent, 1)

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
			CEREAL_NVP(local_rotation),
			CEREAL_NVP(local_prev_rotation),
			CEREAL_NVP(local_scale),
			CEREAL_NVP(local_prev_scale),

			CEREAL_NVP(world_matrix),
			CEREAL_NVP(scaling_matrix),
			CEREAL_NVP(rotation_matrix),
			CEREAL_NVP(translation_matrix),

			CEREAL_NVP(model_right),
			CEREAL_NVP(model_front),
			CEREAL_NVP(model_up),
			CEREAL_NVP(orientation),

			// Quaternion
			CEREAL_NVP(rotation_before),
			CEREAL_NVP(rotation_after)
		);


		if (version == 1)
		{
			archive(
				CEREAL_NVP(model_euler_angle)
			);
		}
	}

	template <class Archive>
	void TransformComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<ComponentBase>(this),
			CEREAL_NVP(local_position),
			CEREAL_NVP(local_prev_position),
			CEREAL_NVP(local_rotation),
			CEREAL_NVP(local_prev_rotation),
			CEREAL_NVP(local_scale),
			CEREAL_NVP(local_prev_scale),

			CEREAL_NVP(world_matrix),
			CEREAL_NVP(scaling_matrix),
			CEREAL_NVP(rotation_matrix),
			CEREAL_NVP(translation_matrix),

			CEREAL_NVP(model_right),
			CEREAL_NVP(model_front),
			CEREAL_NVP(model_up),
			CEREAL_NVP(orientation),

			// Quaternion
			CEREAL_NVP(rotation_before),
			CEREAL_NVP(rotation_after),

			CEREAL_NVP(model_euler_angle)
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
		 local_rotation{other.local_rotation},
		 local_scale{ other.local_scale },
		 local_prev_position{other.local_prev_position},
		 local_prev_rotation{other.local_prev_rotation},
		 local_prev_scale{other.local_prev_scale},
		 world_matrix{ other.world_matrix },
		 scaling_matrix{other.scaling_matrix},
		 rotation_matrix{ other.rotation_matrix },
		 translation_matrix{other.translation_matrix},
		 model_right{ DirectX::SimpleMath::Vector3{ 1.0f,0.0f,0.0f } },
		 model_up{ DirectX::SimpleMath::Vector3{ 0.0f,1.0f,0.0f } },
		 model_front{ DirectX::SimpleMath::Vector3{ 0.0f,0.0f,1.0f } },
		 orientation{DirectX::SimpleMath::Matrix::Identity},
		 model_euler_angle{other.model_euler_angle},
		 model_prev_euler_angle{other.model_prev_euler_angle},
		 rotation_before{other.rotation_before },
		 rotation_after{other.rotation_after },
		 dirty_flg{other.dirty_flg}
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

		ComponentBase::operator=(other);
		local_position				= other.local_position;
		local_rotation				= other.local_rotation;
		local_scale					= other.local_scale;
		local_prev_position			= other.local_prev_position;
		local_prev_rotation			= other.local_prev_rotation;
		local_prev_scale			= other.local_prev_scale;
		world_matrix				= other.world_matrix;
		scaling_matrix				= other.scaling_matrix;
		rotation_matrix				= other.rotation_matrix;
		translation_matrix			= other.translation_matrix;
		model_right					= DirectX::SimpleMath::Vector3{ 1.0f,0.0f,0.0f };
		model_up					= DirectX::SimpleMath::Vector3{ 0.0f,1.0f,0.0f };
		model_front					= DirectX::SimpleMath::Vector3{ 0.0f,0.0f,1.0f };
		orientation					= DirectX::SimpleMath::Matrix::Identity;
		model_euler_angle			= other.model_euler_angle;
		model_prev_euler_angle		= other.model_prev_euler_angle;
		rotation_before				= other.rotation_before;
		rotation_after				= other.rotation_after;
		dirty_flg					= other.dirty_flg;

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
		local_prev_position		= local_position;
		local_prev_scale		= local_scale;
		local_prev_rotation		= local_rotation;
	    model_prev_euler_angle  = model_euler_angle;
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
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			IMGUI_LEFT_LABEL(ImGui::DragFloat3, "Position", reinterpret_cast<float*>(&local_position), 0.01f, -10000.0f, 10000.0f);
			IMGUI_LEFT_LABEL(ImGui::DragFloat3, "Scale   ", reinterpret_cast<float*>(&local_scale)	 , 0.01f, -10000.0f, 10000.0f);
			IMGUI_LEFT_LABEL(ImGui::DragFloat3, "Rotation", reinterpret_cast<float*>(&model_euler_angle) , 0.01f, -180.0f  , 180.0f);

			ImGui::Text("Right X %f", model_right.x);
			ImGui::Text("Right Y %f", model_right.y);
			ImGui::Text("Right Z %f", model_right.z);

			ImGui::Text("Up X %f", model_up.x);
			ImGui::Text("Up Y %f", model_up.y);
			ImGui::Text("Up Z %f", model_up.z);

			ImGui::Text("Front X %f", model_front.x);
			ImGui::Text("Front Y %f", model_front.y);
			ImGui::Text("Front Z %f", model_front.z);

			if(model_euler_angle != model_prev_euler_angle)
			{
				SetEulerAngles(model_euler_angle);
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
		cb_transform->SetData(transform);
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

	void TransformComponent::CreateScalingMatrix()
	{
		// Scaling
		DirectX::XMMATRIX s = DirectX::XMMatrixScaling(local_scale.x, local_scale.y, local_scale.z);

		XMStoreFloat4x4(&scaling_matrix, s);
		// Constant Buffer???Z?b?g
		cb_transform->GetData().scaling_matrix = scaling_matrix;
	}

	void TransformComponent::CreateRotationMatrix()
	{
		// Rotation
		DirectX::XMMATRIX r = DirectX::XMMatrixIdentity();
		r = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&local_rotation));
		XMStoreFloat4x4(&rotation_matrix, r);
		// Constant Buffer???Z?b?g
		cb_transform->GetData().rotation_matrix = rotation_matrix;
	}

	void TransformComponent::CreateTranslationMatrix()
	{
		// Parallel movement
		DirectX::XMMATRIX t = DirectX::XMMatrixTranslation(local_position.x, local_position.y, local_position.z);

		XMStoreFloat4x4(&translation_matrix, t);
		// Constant Buffer???Z?b?g
		cb_transform->GetData().translation_matrix = translation_matrix;
	}

	void TransformComponent::CalcModelCoordinateAxis(const DirectX::XMFLOAT4X4& orientation)
	{
		model_right = DirectX::XMFLOAT3{ orientation._11,orientation._12,orientation._13 };
		model_up	= DirectX::XMFLOAT3{ orientation._21,orientation._22,orientation._23 };
		model_front = DirectX::XMFLOAT3{ orientation._31,orientation._32,orientation._33 };
	}

	void TransformComponent::CreateWorldTransformMatrix()
	{
		CreateScalingMatrix();
		CreateRotationMatrix();
		CreateTranslationMatrix();

		//-- Create world transform matrix --//
		// Scaling
		const DirectX::XMMATRIX s = XMLoadFloat4x4(&scaling_matrix);
		// Rotation
		const DirectX::XMMATRIX r = XMLoadFloat4x4(&rotation_matrix);
		// Translation
		const DirectX::XMMATRIX t = XMLoadFloat4x4(&translation_matrix);

		// Matrix synthesis
		const DirectX::XMMATRIX matrix = s * r * t;
		XMStoreFloat4x4(&local_matrix, matrix);

		model_right = DirectX::XMFLOAT3{ rotation_matrix._11,rotation_matrix._12,rotation_matrix._13 };
		model_up	= DirectX::XMFLOAT3{ rotation_matrix._21,rotation_matrix._22,rotation_matrix._23 };
		model_front = DirectX::XMFLOAT3{ rotation_matrix._31,rotation_matrix._32,rotation_matrix._33 };
	}

	void TransformComponent::SetDirtyFlg(const DirtyFlg flg)
	{
		dirty_flg |= flg;
	}

	void TransformComponent::NormalizeAngle()
	{
		//// Angle X
		//if (local_angle.x > 180.0f)
		//{
		//	local_angle.x -= 360.0f;
		//}
		//else if (local_angle.x < -180.0f)
		//{
		//	local_angle.x += 360.0f;
		//}

		//// Angle Y
		//if (local_angle.y > 180.0f)
		//{
		//	local_angle.y -= 360.0f;
		//}
		//else if (local_angle.y < -180.0f)
		//{
		//	local_angle.y += 360.0f;
		//}

		//// Angle Z
		//if (local_angle.z > 180.0f)
		//{
		//	local_angle.z -= 360.0f;
		//}
		//else if (local_angle.z < -180.0f)
		//{
		//	local_angle.z += 360.0f;
		//}
	}

	const DirectX::SimpleMath::Matrix& TransformComponent::GetWorldMatrix()
	{
		auto& hierarchy_comp = GetRegistry()->GetComponent<HierarchyComponent>(GetEntity());
		if (dirty_flg | DirtyFlg::Local_Transform_Changed)
		{
			NormalizeAngle();
			CreateWorldTransformMatrix();

			if (!hierarchy_comp.GetParentEntity().empty())
			{
				if (auto* model_comp = GetRegistry()->TryGetComponent<ModelComponent>(hierarchy_comp.GetParentEntity());
					model_comp)
				{
					if (const auto& [node_index, node_name] = hierarchy_comp.GetParentNodeData();
						node_index >= 0 &&
						model_comp->GetNodeIndexFromName(node_name) >= 0)
					{// ?e???{?[?????u???A?^?b?`?????????????A?^?b?`?????????????u?????q?????u???Z?o????
						const auto& parent_bone_matrix = model_comp->GetNodes().at(node_index).world_transform;
						world_matrix = local_matrix * parent_bone_matrix;
					}
					else
					{// ?e???{?[?????u???A?^?b?`?????????????A?????????????????s?????????e???g?????X?t?H?[???s?????????u???Z?o????
						const auto& parent_matrix = GetRegistry()->GetComponent<TransformComponent>(hierarchy_comp.GetParentEntity()).GetWorldMatrix();
						world_matrix = local_matrix * parent_matrix;
					}
				}
				else
				{
					const auto& parent_matrix = GetRegistry()->GetComponent<TransformComponent>(hierarchy_comp.GetParentEntity()).GetWorldMatrix();
					world_matrix = local_matrix * parent_matrix;
				}
			}
			else
			{// ?e???????????????????????[???h?g?????X?t?H?[?????g?p
				world_matrix = local_matrix;
			}
			dirty_flg &= ~DirtyFlg::Local_Transform_Changed;
		}

		if(dirty_flg | DirtyFlg::Animation_Changed)
		{
			NormalizeAngle();
			CreateWorldTransformMatrix();

			if(auto* parent_model_comp = GetRegistry()->TryGetComponent<ModelComponent>(hierarchy_comp.GetParentEntity());
			   parent_model_comp)
			{
				if (const auto& [node_index, node_name] = hierarchy_comp.GetParentNodeData();
					node_index >= 0 &&
					parent_model_comp->GetNodeIndexFromName(node_name) >= 0)
				{// ?e???{?[?????u???A?^?b?`?????????????A?^?b?`?????????????u?????q?????u???Z?o????
					const auto& parent_matrix = parent_model_comp->GetNodes().at(node_index).world_transform;
					world_matrix = local_matrix * parent_matrix;
				}
			}
			dirty_flg &= ~DirtyFlg::Animation_Changed;
		}

		return world_matrix;
	}

	void TransformComponent::CreateIdentity4x4(DirectX::SimpleMath::Matrix& convert)
	{
		convert = DirectX::SimpleMath::Matrix::Identity;
	}

	void TransformComponent::SetPosition(const DirectX::SimpleMath::Vector3& pos)
	{
		local_position = pos;
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	void TransformComponent::SetPosition_X(const float x)
	{
		local_position.x = x;
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	void TransformComponent::SetPosition_Y(float y)
	{
		local_position.y = y;
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	void TransformComponent::SetPosition_Z(float z)
	{
		local_position.z = z;
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	void TransformComponent::AdjustPosition(const DirectX::SimpleMath::Vector3& adjust_val)
	{
		local_position += adjust_val;
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	void TransformComponent::AdjustPosition_X(float x)
	{
		local_position.x += x;
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	void TransformComponent::AdjustPosition_Y(float y)
	{
		local_position.y += y;
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	void TransformComponent::AdjustPosition_Z(float z)
	{
		local_position.z += z;
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	void TransformComponent::SetScale(const DirectX::SimpleMath::Vector3& scale)
	{
		local_scale = scale;
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	void TransformComponent::SetScale(const float scale)
	{
		local_scale.x = local_scale.y = local_scale.z = scale;
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	void TransformComponent::SetScale_X(const float x)
	{
		local_scale.x = x;
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	void TransformComponent::SetScale_Y(const float y)
	{
		local_scale.y = y;
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	void TransformComponent::SetScale_Z(const float z)
	{
		local_scale.z = z;
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	void TransformComponent::AdjustScale(const DirectX::SimpleMath::Vector3& adjust_val)
	{
		local_scale += adjust_val;
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	void TransformComponent::AdjustScale(const float adjust_val)
	{
		local_scale.x += adjust_val;
		local_scale.y += adjust_val;
		local_scale.z += adjust_val;
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	void TransformComponent::AdjustScale_X(float x)
	{
		local_scale.x += x;
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	void TransformComponent::AdjustScale_Y(float y)
	{
		local_scale.y += y;
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	void TransformComponent::AdjustScale_Z(float z)
	{
		local_scale.z += z;
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	void TransformComponent::SetRotation(const DirectX::SimpleMath::Quaternion& q)
	{
		local_rotation = q;
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	void TransformComponent::SetRotation(float x, float y, float z, float w)
	{
		const DirectX::SimpleMath::Quaternion q = { x,y,z,w };
		local_rotation = q;
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	void TransformComponent::SetEulerAngle_X(const float x)
	{
		const DirectX::SimpleMath::Vector3 r = GetEulerAngles();
		SetEulerAngles(x, r.y, r.z);
	}

	void TransformComponent::SetEulerAngle_Y(const float y)
	{
		const DirectX::SimpleMath::Vector3 r = GetEulerAngles();
		SetEulerAngles(r.x, y, r.z);
	}

	void TransformComponent::SetEulerAngle_Z(const float z)
	{
		const DirectX::SimpleMath::Vector3 r = GetEulerAngles();
		SetEulerAngles(r.x, r.y, z);
	}

	void TransformComponent::SetEulerAngles(const DirectX::SimpleMath::Vector3& angle)
	{
		model_euler_angle = angle;
		DirectX::SimpleMath::Quaternion q{};
		q = q.CreateFromYawPitchRoll(DirectX::XMConvertToRadians(angle.y),
		                             DirectX::XMConvertToRadians(angle.x),
		                             DirectX::XMConvertToRadians(angle.z));
		SetRotation(q);
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	void TransformComponent::SetEulerAngles(const float x, const float y, const float z)
	{
		model_euler_angle = DirectX::SimpleMath::Vector3{ x,y,z };
		DirectX::SimpleMath::Quaternion q{};
		q = q.CreateFromYawPitchRoll(DirectX::XMConvertToRadians(y),
									 DirectX::XMConvertToRadians(x),
									 DirectX::XMConvertToRadians(z));
		SetRotation(q);
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	void TransformComponent::AdjustEulerAngles(const DirectX::SimpleMath::Vector3& adjust_val)
	{
		const DirectX::SimpleMath::Vector3 r = arithmetic::ConvertQuaternionToEuler(local_rotation);
		SetEulerAngles(r + adjust_val);
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	void TransformComponent::AdjustEulerAngles(float adjust_x, float adjust_y, float adjust_z)
	{
		const DirectX::SimpleMath::Vector3 r = arithmetic::ConvertQuaternionToEuler(local_rotation);
		SetEulerAngles(r.x + adjust_x, r.y + adjust_y, r.z + adjust_z);
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	const DirectX::SimpleMath::Quaternion& TransformComponent::GetRotation() const
	{
		return local_rotation;
	}

	DirectX::SimpleMath::Vector3 TransformComponent::GetEulerAngles() const
	{
		return arithmetic::ConvertQuaternionToEuler(local_rotation);
	}

	DirectX::SimpleMath::Vector3 TransformComponent::GetDeltaEulerAngles() const
	{
		return { arithmetic::ConvertQuaternionToEuler(local_rotation) - arithmetic::ConvertQuaternionToEuler(local_prev_rotation) };
	}


	void TransformComponent::SetWorldMatrix(const DirectX::SimpleMath::Matrix& mat)
	{
		world_matrix = mat;
		DirectX::SimpleMath::Vector3    t{};
		DirectX::SimpleMath::Quaternion r{};
		DirectX::SimpleMath::Vector3    s{};

		if(auto& hierarchy_comp = GetRegistry()->GetComponent<HierarchyComponent>(GetEntity());
		   !hierarchy_comp.GetParentEntity().empty())
		{
			if (auto* model_comp = GetRegistry()->TryGetComponent<ModelComponent>(hierarchy_comp.GetParentEntity());
				model_comp)
			{
				if (const auto& [node_index, node_name] = hierarchy_comp.GetParentNodeData();
					node_index >= 0 &&
					model_comp->GetNodeIndexFromName(node_name) >= 0)
				{// ?e???{?[?????u???A?^?b?`?????????????A?^?b?`?????????????u?????q?????u???Z?o????
					const auto& parent_bone_matrix = model_comp->GetNodes().at(node_index).world_transform;
					local_matrix = world_matrix * parent_bone_matrix.Invert();
				}
				else
				{// ?e???{?[?????u???A?^?b?`?????????????A?????????????????s?????????e???g?????X?t?H?[???s?????????u???Z?o????
					const auto& parent_matrix = GetRegistry()->GetComponent<TransformComponent>(hierarchy_comp.GetParentEntity()).GetWorldMatrix();
					local_matrix = world_matrix * parent_matrix.Invert();
				}
			}
			else
			{
				const auto& parent_matrix = GetRegistry()->GetComponent<TransformComponent>(hierarchy_comp.GetParentEntity()).GetWorldMatrix();
				local_matrix = world_matrix * parent_matrix.Invert();
			}

			if (arithmetic::DecomposeMatrix(t, r, s, local_matrix))
			{
				SetPosition(t);
				SetScale(s);
				SetRotation(r);
				const DirectX::SimpleMath::Vector3 euler = arithmetic::ConvertQuaternionToEuler(r);
				SetEulerAngles(euler);
			}
		}
		else
		{
			local_matrix = world_matrix;
			if (arithmetic::DecomposeMatrix(t, r, s, world_matrix))
			{
				SetPosition(t);
				SetScale(s);
				SetRotation(r);
				const DirectX::SimpleMath::Vector3 euler = arithmetic::ConvertQuaternionToEuler(r);
				SetEulerAngles(euler);
			}
		}
	}

	void TransformComponent::SetLocalMatrix(const DirectX::SimpleMath::Matrix& mat)
	{
		local_matrix = mat;
		//dirty_flg |= DirtyFlg::Local_Transform_Changed;
		DirectX::SimpleMath::Vector3    t{};
		DirectX::SimpleMath::Quaternion r{};
		DirectX::SimpleMath::Vector3    s{};

		if (arithmetic::DecomposeMatrix(t, r, s, local_matrix))
		{
			SetPosition(t);
			SetScale(s);
			SetRotation(r);
			const DirectX::SimpleMath::Vector3 euler = arithmetic::ConvertQuaternionToEuler(r);
			SetEulerAngles(euler);
			//dirty_flg |= DirtyFlg::Local_Transform_Changed;
		}
	}


	void TransformComponent::SetQuaternionSlerp(const DirectX::SimpleMath::Quaternion& q1, const DirectX::SimpleMath::Quaternion& q2)
	{
		rotation_before = q1;
		rotation_after  = q2;
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	void TransformComponent::QuaternionSlerp(const float t)
	{
		local_rotation = Quaternion::Slerp(rotation_before, rotation_after, t);
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	void TransformComponent::AdjustRotationFromAxis(const DirectX::SimpleMath::Vector3& axis, const float local_angle)
	{
		const Quaternion q = Quaternion::CreateFromAxisAngle(axis, local_angle);
		local_rotation *= q;
		// ?_?[?e?B?t???O???Z?b?g
		dirty_flg |= DirtyFlg::Local_Transform_Changed;
	}

	DirectX::XMMATRIX TransformComponent::GetRotationMatrix(DirectX::XMFLOAT3 axis, float local_angle/* degree */)
	{
		const DirectX::XMVECTOR axis_vec = XMLoadFloat3(&axis);
		const DirectX::XMVECTOR calc_val = DirectX::XMQuaternionRotationAxis(axis_vec, DirectX::XMConvertToRadians(local_angle));

		return DirectX::XMMatrixRotationQuaternion(calc_val);
	}
} // cumulonimbus::component
