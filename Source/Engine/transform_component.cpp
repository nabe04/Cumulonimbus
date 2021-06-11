#include "transform_component.h"

#include <fstream>
#include <string>
#include <imgui.h>

#include <cereal/archives/json.hpp>

#include "transform.h"
#include "arithmetic.h"
#include "component_base.h"
#include "component_list.h"

using namespace DirectX;

TransformComponent::TransformComponent(Entity* entity, const XMFLOAT3& pos)
	: Component{ entity }
{
	component_name = "TransformComponent";

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

//template <class Archive>
//void TransformComponent::serialize(Archive&& archive)
//{
//	archive(
//		cereal::make_nvp("Component Name", component_name),
//		CEREAL_NVP(position),
//		CEREAL_NVP(prev_pos),
//		CEREAL_NVP(scale),
//		CEREAL_NVP(angle),
//		CEREAL_NVP(prev_angle),
//
//		CEREAL_NVP(world_f4x4),
//		CEREAL_NVP(scaling_matrix),
//		CEREAL_NVP(rotation_matrix),
//		CEREAL_NVP(translation_matrix),
//
//		CEREAL_NVP(model_right),
//		CEREAL_NVP(model_front),
//		CEREAL_NVP(model_up),
//		CEREAL_NVP(orientation),
//
//		// Quaternion
//		CEREAL_NVP(axis),
//		CEREAL_NVP(local_quaternion),
//		CEREAL_NVP(world_quaternion),
//
//		CEREAL_NVP(set_angle_bit_flg),
//		CEREAL_NVP(is_billboard),
//		CEREAL_NVP(is_quaternion)
//
//	);
//}


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
		CalcLocalRotation_X();
		CalcLocalRotation_Y();
		CalcLocalRotation_Z();
		r = XMMatrixRotationQuaternion(XMLoadFloat4(&local_quaternion));
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

void TransformComponent::CalcWorldRotationAngle()
{
	{// Caluculate angle x
		XMFLOAT3 compare_up = arithmetic::CalcUpVec(right, model_right);
		float angle = arithmetic::CalcAngleFromTwoVec(right, model_right);
		XMVECTOR dot_val = XMVector3Dot(XMLoadFloat3(&compare_up), XMLoadFloat3(&up));

		this->angle.x = XMVectorGetX(dot_val) >= 0 ? angle : -angle;
	}

	{// Caluculate angle y
		XMFLOAT3 compare_up = arithmetic::CalcUpVec(up, model_up);
		float angle = arithmetic::CalcAngleFromTwoVec(up, model_up);
		XMVECTOR dot_val = XMVector3Dot(XMLoadFloat3(&compare_up), XMLoadFloat3(&right));

		this->angle.y = XMVectorGetY(dot_val) >= 0 ? angle : -angle;
	}

	{// Caluculate angle z
		XMFLOAT3 compare_up = arithmetic::CalcUpVec(front, model_front);
		float angle = arithmetic::CalcAngleFromTwoVec(front, model_front);
		XMVECTOR dot_val = XMVector3Dot(XMLoadFloat3(&compare_up), XMLoadFloat3(&up));

		this->angle.z = XMVectorGetZ(dot_val) >= 0 ? angle : -angle;
	}
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
	model_up = XMFLOAT3{ rotation_matrix._21,rotation_matrix._22,rotation_matrix._23 };
	model_front = XMFLOAT3{ rotation_matrix._31,rotation_matrix._32,rotation_matrix._33 };


	prev_angle = angle;

	//-- Calculate eulerian angle --//
	//CalcWorldRotationAngle();
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

void TransformComponent::AdjustLocalRotation_X(float angle_x)
{
	angle.x += angle_x;
}

void TransformComponent::AdjustLocalRotation_Y(float angle_y)
{
	angle.y += angle_y;
}

void TransformComponent::AdjustLocalRotation_Z(float angle_z)
{
	angle.z += angle_z;
}

void TransformComponent::CalcLocalRotation_X()
{
	XMFLOAT3 delta_angle = GetDeltaAngle();
	XMVECTOR right_vec = XMLoadFloat3(&model_right);
	XMVECTOR store_vec = XMLoadFloat4(&local_quaternion);
	XMVECTOR calc_val;
	//if (set_angle_bit_flg.test(Bit::X))
	//{
	//	float rotation_val = angle.x - prev_angle.x;
	//	XMVECTOR calc_val = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90),angle.y,angle.z);
	//	store_vec = XMQuaternionSlerp(store_vec, calc_val, 1.0f);
	//	//calc_val = XMQuaternionRotationAxis(XMLoadFloat3(&right), XMConvertToRadians(rotation_val));
	//	set_angle_bit_flg.reset(Bit::X);
	//}
	//else
	{
		calc_val = XMQuaternionRotationAxis(right_vec, XMConvertToRadians(delta_angle.x));
		store_vec = XMQuaternionMultiply(store_vec, calc_val);
	}
	XMStoreFloat4(&local_quaternion, store_vec);
}

void TransformComponent::CalcLocalRotation_Y()
{
	XMFLOAT3 delta_angle = GetDeltaAngle();
	XMVECTOR up_vec = XMLoadFloat3(&model_up);
	XMVECTOR calc_val;
	//if (set_angle_bit_flg.test(Bit::Y))
	//{
	//	float rotation_val = angle.y - prev_angle.y;
	//	calc_val = XMQuaternionRotationAxis(up_vec, XMConvertToRadians(rotation_val));
	//	set_angle_bit_flg.reset(Bit::Y);
	//}
	//else
	//{
	calc_val = XMQuaternionRotationAxis(up_vec, XMConvertToRadians(delta_angle.y));
	XMVECTOR store_vec = XMLoadFloat4(&local_quaternion);
	store_vec = XMQuaternionMultiply(store_vec, calc_val);
	XMStoreFloat4(&local_quaternion, store_vec);
}

void TransformComponent::CalcLocalRotation_Z()
{
	XMFLOAT3 delta_angle = GetDeltaAngle();
	XMVECTOR front_vec = XMLoadFloat3(&model_front);
	XMVECTOR calc_val = XMQuaternionRotationAxis(front_vec, XMConvertToRadians(delta_angle.z));
	XMVECTOR store_vec = XMLoadFloat4(&local_quaternion);

	store_vec = XMQuaternionMultiply(store_vec, calc_val);
	XMStoreFloat4(&local_quaternion, store_vec);
}

XMMATRIX TransformComponent::GetRotationMatrix(XMFLOAT3 axis, float angle/* degree */)
{
	XMVECTOR axis_vec = XMLoadFloat3(&axis);
	XMVECTOR calc_val = XMQuaternionRotationAxis(axis_vec, XMConvertToRadians(angle));

	return XMMatrixRotationQuaternion(calc_val);
}

XMVECTOR TransformComponent::GetRotationVec(XMFLOAT3 axis, float angle/* degree */)
{
	XMVECTOR axis_vec = XMLoadFloat3(&axis);
	XMVECTOR calc_val = XMQuaternionRotationAxis(axis_vec, XMConvertToRadians(angle));
	XMVECTOR store_vec = XMLoadFloat4(&local_quaternion);

	//store_vec *= calc_val;
	//XMStoreFloat3(&quaternion_f4, store_vec);

	return store_vec;
}

//template <class Archive>
//void TransformComponent::serialize(Archive&& archive)


void TransformComponent::Save(std::string file_path)
{
	const std::string file_path_and_name = file_path + component_name + ".json";
	std::ofstream ofs(file_path_and_name);
	cereal::JSONOutputArchive o_archive(ofs);
	o_archive(
		cereal::base_class<Component>(this),
		CEREAL_NVP(component_name),
		CEREAL_NVP(position),
		CEREAL_NVP(prev_pos),
		CEREAL_NVP(scale),
		CEREAL_NVP(angle),
		CEREAL_NVP(prev_angle),

		CEREAL_NVP(world_f4x4),
		CEREAL_NVP(scaling_matrix),
		CEREAL_NVP(rotation_matrix),
		CEREAL_NVP(translation_matrix),

		CEREAL_NVP(model_right),
		CEREAL_NVP(model_front),
		CEREAL_NVP(model_up),
		CEREAL_NVP(orientation),

		// Quaternion
		CEREAL_NVP(axis),
		CEREAL_NVP(local_quaternion),
		CEREAL_NVP(world_quaternion),

		CEREAL_NVP(set_angle_bit_flg),
		CEREAL_NVP(is_billboard),
		CEREAL_NVP(is_quaternion)
	);
}

void TransformComponent::Load(Entity* entity, std::string file_path_and_name)
{
	{
		std::ifstream ifs(file_path_and_name);
		cereal::JSONInputArchive i_archive(ifs);
		i_archive(
			cereal::base_class<Component>(this),
			CEREAL_NVP(component_name),
			CEREAL_NVP(position),
			CEREAL_NVP(prev_pos),
			CEREAL_NVP(scale),
			CEREAL_NVP(angle),
			CEREAL_NVP(prev_angle),

			CEREAL_NVP(world_f4x4),
			CEREAL_NVP(scaling_matrix),
			CEREAL_NVP(rotation_matrix),
			CEREAL_NVP(translation_matrix),

			CEREAL_NVP(model_right),
			CEREAL_NVP(model_front),
			CEREAL_NVP(model_up),
			CEREAL_NVP(orientation),

			// Quaternion
			CEREAL_NVP(axis),
			CEREAL_NVP(local_quaternion),
			CEREAL_NVP(world_quaternion),

			CEREAL_NVP(set_angle_bit_flg),
			CEREAL_NVP(is_billboard),
			CEREAL_NVP(is_quaternion)
		);

		this->entity = entity;
	}
}

namespace cumulonimbus::component
{
	using namespace DirectX::SimpleMath;

	TransformComponent::TransformComponent(ecs::Registry* const registry, const ecs::Entity ent,
										   const Vector3& pos, const Vector3& scale, const Vector3& angle)
		: ComponentBase{ registry,ent}
		,position{ pos }
		,scale{ scale }
		,angle{ angle }
	{
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
			CalcLocalRotation_X();
			CalcLocalRotation_Y();
			CalcLocalRotation_Z();
			r = XMMatrixRotationQuaternion(XMLoadFloat4(&local_quaternion));
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

	void TransformComponent::CalcWorldRotationAngle()
	{
		{// Caluculate angle x
			XMFLOAT3 compare_up = arithmetic::CalcUpVec(right, model_right);
			float angle = arithmetic::CalcAngleFromTwoVec(right, model_right);
			XMVECTOR dot_val = XMVector3Dot(XMLoadFloat3(&compare_up), XMLoadFloat3(&up));

			this->angle.x = XMVectorGetX(dot_val) >= 0 ? angle : -angle;
		}

		{// Caluculate angle y
			XMFLOAT3 compare_up = arithmetic::CalcUpVec(up, model_up);
			float angle = arithmetic::CalcAngleFromTwoVec(up, model_up);
			XMVECTOR dot_val = XMVector3Dot(XMLoadFloat3(&compare_up), XMLoadFloat3(&right));

			this->angle.y = XMVectorGetY(dot_val) >= 0 ? angle : -angle;
		}

		{// Caluculate angle z
			XMFLOAT3 compare_up = arithmetic::CalcUpVec(front, model_front);
			float angle = arithmetic::CalcAngleFromTwoVec(front, model_front);
			XMVECTOR dot_val = XMVector3Dot(XMLoadFloat3(&compare_up), XMLoadFloat3(&up));

			this->angle.z = XMVectorGetZ(dot_val) >= 0 ? angle : -angle;
		}
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
		model_up = XMFLOAT3{ rotation_matrix._21,rotation_matrix._22,rotation_matrix._23 };
		model_front = XMFLOAT3{ rotation_matrix._31,rotation_matrix._32,rotation_matrix._33 };


		prev_angle = angle;

		//-- Calculate eulerian angle --//
		//CalcWorldRotationAngle();
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

	void TransformComponent::AdjustLocalRotation_X(float angle_x)
	{
		angle.x += angle_x;
	}

	void TransformComponent::AdjustLocalRotation_Y(float angle_y)
	{
		angle.y += angle_y;
	}

	void TransformComponent::AdjustLocalRotation_Z(float angle_z)
	{
		angle.z += angle_z;
	}

	void TransformComponent::CalcLocalRotation_X()
	{
		XMFLOAT3 delta_angle = GetDeltaAngle();
		XMVECTOR right_vec = XMLoadFloat3(&model_right);
		XMVECTOR store_vec = XMLoadFloat4(&local_quaternion);
		XMVECTOR calc_val;
		//if (set_angle_bit_flg.test(Bit::X))
		//{
		//	float rotation_val = angle.x - prev_angle.x;
		//	XMVECTOR calc_val = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90),angle.y,angle.z);
		//	store_vec = XMQuaternionSlerp(store_vec, calc_val, 1.0f);
		//	//calc_val = XMQuaternionRotationAxis(XMLoadFloat3(&right), XMConvertToRadians(rotation_val));
		//	set_angle_bit_flg.reset(Bit::X);
		//}
		//else
		{
			calc_val = XMQuaternionRotationAxis(right_vec, XMConvertToRadians(delta_angle.x));
			store_vec = XMQuaternionMultiply(store_vec, calc_val);
		}
		XMStoreFloat4(&local_quaternion, store_vec);
	}

	void TransformComponent::CalcLocalRotation_Y()
	{
		XMFLOAT3 delta_angle = GetDeltaAngle();
		XMVECTOR up_vec = XMLoadFloat3(&model_up);
		XMVECTOR calc_val;
		//if (set_angle_bit_flg.test(Bit::Y))
		//{
		//	float rotation_val = angle.y - prev_angle.y;
		//	calc_val = XMQuaternionRotationAxis(up_vec, XMConvertToRadians(rotation_val));
		//	set_angle_bit_flg.reset(Bit::Y);
		//}
		//else
		//{
		calc_val = XMQuaternionRotationAxis(up_vec, XMConvertToRadians(delta_angle.y));
		XMVECTOR store_vec = XMLoadFloat4(&local_quaternion);
		store_vec = XMQuaternionMultiply(store_vec, calc_val);
		XMStoreFloat4(&local_quaternion, store_vec);
	}

	void TransformComponent::CalcLocalRotation_Z()
	{
		XMFLOAT3 delta_angle = GetDeltaAngle();
		XMVECTOR front_vec = XMLoadFloat3(&model_front);
		XMVECTOR calc_val = XMQuaternionRotationAxis(front_vec, XMConvertToRadians(delta_angle.z));
		XMVECTOR store_vec = XMLoadFloat4(&local_quaternion);

		store_vec = XMQuaternionMultiply(store_vec, calc_val);
		XMStoreFloat4(&local_quaternion, store_vec);
	}

	XMMATRIX TransformComponent::GetRotationMatrix(XMFLOAT3 axis, float angle/* degree */)
	{
		XMVECTOR axis_vec = XMLoadFloat3(&axis);
		XMVECTOR calc_val = XMQuaternionRotationAxis(axis_vec, XMConvertToRadians(angle));

		return XMMatrixRotationQuaternion(calc_val);
	}

	XMVECTOR TransformComponent::GetRotationVec(XMFLOAT3 axis, float angle/* degree */)
	{
		XMVECTOR axis_vec = XMLoadFloat3(&axis);
		XMVECTOR calc_val = XMQuaternionRotationAxis(axis_vec, XMConvertToRadians(angle));
		XMVECTOR store_vec = XMLoadFloat4(&local_quaternion);

		//store_vec *= calc_val;
		//XMStoreFloat3(&quaternion_f4, store_vec);

		return store_vec;
	}

	//template <class Archive>
	//void TransformComponent::serialize(Archive&& archive)


	void TransformComponent::Save(const std::string& file_path)
	{
		const std::string file_path_and_name = file_path + typeid(TransformComponent).name() + ".json";
		std::ofstream ofs(file_path_and_name);
		cereal::JSONOutputArchive o_archive(ofs);
		o_archive(
			cereal::base_class<ComponentBase>(this),
			CEREAL_NVP(position),
			CEREAL_NVP(prev_pos),
			CEREAL_NVP(scale),
			CEREAL_NVP(angle),
			CEREAL_NVP(prev_angle),

			CEREAL_NVP(world_f4x4),
			CEREAL_NVP(scaling_matrix),
			CEREAL_NVP(rotation_matrix),
			CEREAL_NVP(translation_matrix),

			CEREAL_NVP(model_right),
			CEREAL_NVP(model_front),
			CEREAL_NVP(model_up),
			CEREAL_NVP(orientation),

			// Quaternion
			CEREAL_NVP(axis),
			CEREAL_NVP(local_quaternion),
			CEREAL_NVP(world_quaternion),

			CEREAL_NVP(set_angle_bit_flg),
			CEREAL_NVP(is_billboard),
			CEREAL_NVP(is_quaternion)
		);
	}

	void TransformComponent::Load(const std::string& file_path_and_name)
	{
		{
			std::ifstream ifs(file_path_and_name);
			cereal::JSONInputArchive i_archive(ifs);
			i_archive(
				cereal::base_class<ComponentBase>(this),
				CEREAL_NVP(position),
				CEREAL_NVP(prev_pos),
				CEREAL_NVP(scale),
				CEREAL_NVP(angle),
				CEREAL_NVP(prev_angle),

				CEREAL_NVP(world_f4x4),
				CEREAL_NVP(scaling_matrix),
				CEREAL_NVP(rotation_matrix),
				CEREAL_NVP(translation_matrix),

				CEREAL_NVP(model_right),
				CEREAL_NVP(model_front),
				CEREAL_NVP(model_up),
				CEREAL_NVP(orientation),

				// Quaternion
				CEREAL_NVP(axis),
				CEREAL_NVP(local_quaternion),
				CEREAL_NVP(world_quaternion),

				CEREAL_NVP(set_angle_bit_flg),
				CEREAL_NVP(is_billboard),
				CEREAL_NVP(is_quaternion)
			);
		}
	}
}
