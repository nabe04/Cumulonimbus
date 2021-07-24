#pragma once
#include <bitset>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>

#include "component_base.h"
#include "constant_buffer.h"
#include "shader_interop_renderer.h"
#include "rename_type_mapping.h"

namespace cumulonimbus::component
{
	class TransformComponent :public ComponentBase
	{
	public:
		explicit TransformComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		TransformComponent() = default; // for cereal
		~TransformComponent() override = default;

		void NewFrame(float delta_time)override;
		void Update(float delta_time) override;
		void RenderImGui() override;

		void Save(const std::string& file_path) override;
		void Load(const std::string& file_path_and_name) override;

		void SetTransformCB(const TransformCB transform) const;
		void BindCBuffer(bool set_in_vs = true, bool set_in_ps = true) const;
		void UnbindCBuffer();
		void SetAndBindCBuffer(const TransformCB& transform, bool set_in_vs = true, bool set_in_ps = true) const;

		//-- Creating a World Transformation Matrix --//
		void CreateWorldTransformMatrix();

		//-- Position --//
		void SetPositionX(const float x) { position.x = x; }
		void SetPositionY(const float y) { position.y = y; }
		void SetPositionZ(const float z) { position.z = z; }
		void SetPosition(const DirectX::XMFLOAT3& pos) { position = pos; }
		void AdjustPositionX(const float x) { position.x += x; }
		void AdjustPositionY(const float y) { position.y += y; }
		void AdjustPositionZ(const float z) { position.z += z; }
		void AdjustPosition(const DirectX::XMFLOAT3& adjust_val)
		{
			position.x += adjust_val.x;
			position.y += adjust_val.y;
			position.z += adjust_val.z;
		}
		[[nodiscard]] auto& GetPosition()const	   { return position; }
		void SetOldPosition(DirectX::XMFLOAT3 pos) { prev_pos = pos; }
		[[nodiscard]] auto GetOldPosition() const  { return prev_pos; }
		[[nodiscard]] auto DeltaPosition() const
		{
			const DirectX::XMFLOAT3 delta_pos{ position.x - prev_pos.x ,position.y - prev_pos.y,position.z - prev_pos.z };
			return delta_pos;
		}


		//-- Scale --//
		void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
		void SetScale(const float scale) { this->scale.x = this->scale.y = this->scale.z = scale; }
		void SetScaleX(const float x) { scale.x = x; }
		void SetScaleY(const float y) { scale.y = y; }
		void SetScaleZ(const float z) { scale.z = z; }
		void AdjustScale(const DirectX::XMFLOAT3& scale) { this->scale.x += scale.x; this->scale.y += scale.y; this->scale.z += scale.z; }
		void AdjustScale(const float scale) { this->scale.x += scale; this->scale.y += scale; this->scale.z += scale; }
		void AdjustScaleX(const float x) { scale.x += x; }
		void AdjustScaleY(const float y) { scale.y += y; }
		void AdjustScaleZ(const float z) { scale.z += z; }
		[[nodiscard]] const auto& GetScale() const { return scale; }

		//-- Angle --//
		void SetWorldRotation(const DirectX::XMFLOAT3& angle) { this->angle = angle; }
		void SetWorldRotation(const float angle) { this->angle.x = this->angle.y = this->angle.z = angle; }
		void SetWorldRotation_X(const float x) { angle.x = x; set_angle_bit_flg.set(Bit::X); }
		void SetWorldRotation_Y(const float y) { angle.y = y; set_angle_bit_flg.set(Bit::Y); }
		void SetWorldRotation_Z(const float z) { angle.z = z; set_angle_bit_flg.set(Bit::Z); }
		void AdjustWorldRotation(const DirectX::XMFLOAT3& angle) { this->angle.x += angle.x; this->angle.y += angle.y; this->angle.z += angle.z; }
		void AdjustWorldRotation(const float angle) { this->angle.x += angle; this->angle.y += angle; this->angle.z += angle; }
		void AdjustWorldRotation_X(const float x) { angle.x += x; }
		void AdjustWorldRotation_Y(const float y) { angle.y += y; }
		void AdjustWorldRotation_Z(const float z) { angle.z += z; }
		auto& GetWorldRotation() { return angle; }
		DirectX::XMFLOAT3 GetDeltaAngle() const
		{
			return DirectX::XMFLOAT3{ angle.x - prev_angle.x,angle.y - prev_angle.y,angle.z - prev_angle.z };
		}

		void NormalizeAngle();
		void CreateIdentity4x4(DirectX::XMFLOAT4X4* convert);

		//-- World 4x4 matrix --//
		void SetWorld4x4(DirectX::XMFLOAT4X4& mat4x4) { world_f4x4 = mat4x4; }
		const auto& GetWorld4x4() const { return world_f4x4; }

		// Billboard
		void ActiveBillboard() { is_billboard = true; }
		void PassiveBillboard() { is_billboard = false; }

		// Quaternion
		void ActiveQuaternion() { is_quaternion = true; }
		void PassiveQuaternion() { is_quaternion = false; }
		void SetQuaternionAxis(const DirectX::XMFLOAT3& axis) { this->axis = axis; }
		void AdjustLocalRotation_X(float angle_x);
		void AdjustLocalRotation_Y(float angle_y);
		void AdjustLocalRotation_Z(float angle_z);
		void CalcLocalRotation_X();
		void CalcLocalRotation_Y();
		void CalcLocalRotation_Z();


		[[nodiscard]] const DirectX::SimpleMath::Matrix& GetRotationMat()	 const { return rotation_matrix; }
		[[nodiscard]] const DirectX::SimpleMath::Matrix& GetScalingMat()	 const { return scaling_matrix; }
		[[nodiscard]] const DirectX::SimpleMath::Matrix& GetTransformMat()   const { return translation_matrix; }

		/*
		* brief : Front Billboard
		* billPos   : Location of Billboard Control BillboardTriangleComponent
		* targetPos : Point of view for in a billboard
		*/
		void GetBillboardRotation(const DirectX::XMFLOAT3 billPos, const DirectX::XMFLOAT3 targetPos);

		[[nodiscard]] const DirectX::SimpleMath::Vector3& GetModelRight()const { return model_right; }
		[[nodiscard]] const DirectX::SimpleMath::Vector3& GetModelUp() const { return model_up; }
		[[nodiscard]] const DirectX::SimpleMath::Vector3& GetModelFront()const { return model_front; }

		/*
		 * brief : cereal
		 *         実装を"transform_component.cpp"で書いている
		 */
		template<class  Archive>
		void serialize(Archive&& archive)
		{
			archive(
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

	private:
		std::shared_ptr<buffer::ConstantBuffer<TransformCB>> cb_transform;

		DirectX::SimpleMath::Vector3  position{ 0.0f,0.0f,0.0f };
		DirectX::SimpleMath::Vector3  prev_pos{ 0.0f,0.0f,0.0f };
		DirectX::SimpleMath::Vector3  scale{ 1.0f,1.0f,1.0f };
		DirectX::SimpleMath::Vector3  angle{ 0.0f,0.0f,0.0f };
		DirectX::SimpleMath::Vector3  prev_angle{};
		DirectX::SimpleMath::Matrix   world_f4x4 =
		{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};

		DirectX::SimpleMath::Matrix scaling_matrix;
		DirectX::SimpleMath::Matrix rotation_matrix;
		DirectX::SimpleMath::Matrix translation_matrix;


		/* TODO: constメンバ変数にしたいが、
		 * Entityの削除の際のstd::iter_swap関数でエラーが出るため非constにしている*/
		DirectX::SimpleMath::Vector3 right	= { 1.0f,0.0f,0.0f };
		DirectX::SimpleMath::Vector3 up		= { 0.0f,1.0f,0.0f };
		DirectX::SimpleMath::Vector3 front	= { 0.0f,0.0f,1.0f };

		// Orientation vector
		DirectX::SimpleMath::Vector3 model_right = { 1.0f,0.0f,0.0f };
		DirectX::SimpleMath::Vector3 model_up	 = { 0.0f,1.0f,0.0f };
		DirectX::SimpleMath::Vector3 model_front = { 0.0f,0.0f,1.0f };

		DirectX::SimpleMath::Matrix orientation =
		{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};

		// Quaternion
		DirectX::SimpleMath::Vector3 axis{ 0,1,0 };
		DirectX::SimpleMath::Vector4 local_quaternion{ 0,0,0,1 };
		DirectX::SimpleMath::Vector4 world_quaternion{ 0,0,0,1 };

		enum Bit
		{
			X,
			Y,
			Z,
		};
		std::bitset<3> set_angle_bit_flg;

		bool is_billboard = false;
		bool is_quaternion = false;

		void CreateScaling4x4();
		void CreateRotation4x4();
		void CreateTranslation4x4();
		void CalcModelCoordinateAxis(const DirectX::XMFLOAT4X4& orientation);	// モデルの座標軸計算
		void CalcWorldRotationAngle();

		DirectX::XMMATRIX GetRotationMatrix(DirectX::XMFLOAT3 axis, float angle/* degree */);
		DirectX::XMVECTOR GetRotationVec(DirectX::XMFLOAT3 axis, float angle/* degree */);
	};
} // cumulonimbus::component

CEREAL_REGISTER_TYPE(cumulonimbus::component::TransformComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::TransformComponent)

