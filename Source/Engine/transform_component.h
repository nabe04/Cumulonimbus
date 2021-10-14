#pragma once
#include <bitset>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "component_base.h"
#include "constant_buffer.h"
#include "shader_interop_renderer.h"
#include "rename_type_mapping.h"

namespace cumulonimbus::component
{
	class TransformComponent final :public ComponentBase
	{
	public:
		explicit TransformComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit TransformComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const TransformComponent& copy_comp); // プレファブからの追加用
		explicit TransformComponent()
			:ComponentBase{}
		{
			component_tag = mapping::component_tag::ComponentTag::Mesh;
		}; // for cereal
		TransformComponent(const TransformComponent& other);
		TransformComponent& operator=(const TransformComponent& other);

		~TransformComponent() override = default;

		void PreCommonUpdate(float dt) override;
		void CommonUpdate(float dt) override;
		void PostCommonUpdate(float dt) override;

		void SceneUpdate(float dt) override;

		void PreGameUpdate(float dt) override;
		void GameUpdate(float dt) override;
		void RenderImGui() override;

		void Load(ecs::Registry* registry) override;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		void SetTransformCB(const TransformCB transform) const;
		void BindCBuffer(bool set_in_vs = true, bool set_in_ps = true) const;
		void UnbindCBuffer();
		void SetAndBindCBuffer(const TransformCB& transform, bool set_in_vs = true, bool set_in_ps = true) const;

		//-- Creating a World Transformation Matrix --//
		void CreateWorldTransformMatrix();

		//-- Position --//
		void SetPosition(const DirectX::SimpleMath::Vector3& pos) { local_position = pos; }
		void SetPositionX(const float x) { local_position.x = x; }
		void SetPositionY(const float y) { local_position.y = y; }
		void SetPositionZ(const float z) { local_position.z = z; }
		void AdjustPositionX(const float x) { local_position.x += x; }
		void AdjustPositionY(const float y) { local_position.y += y; }
		void AdjustPositionZ(const float z) { local_position.z += z; }
		void AdjustPosition(const DirectX::SimpleMath::Vector3& adjust_val)
		{
			local_position.x += adjust_val.x;
			local_position.y += adjust_val.y;
			local_position.z += adjust_val.z;
		}
		[[nodiscard]]
		const DirectX::SimpleMath::Vector3& GetPosition()   const
		{
			return local_position;
		}
		[[nodiscard]]
		const DirectX::SimpleMath::Vector3& GetOldPosition() const
		{
			return local_prev_pos;
		}
		[[nodiscard]]
		const DirectX::SimpleMath::Vector3& DeltaPosition() const
		{
			const DirectX::SimpleMath::Vector3 delta_pos{ local_position.x - local_prev_pos.x ,local_position.y - local_prev_pos.y,local_position.z - local_prev_pos.z };
			return delta_pos;
		}

		//-- Scale --//
		void SetScale(const DirectX::SimpleMath::Vector3& local_scale)
		{
			this->local_scale = local_scale;
		}
		void SetScale(const float local_scale)
		{
			this->local_scale.x = this->local_scale.y = this->local_scale.z = local_scale;
		}
		void SetScaleX(const float x) { local_scale.x = x; }
		void SetScaleY(const float y) { local_scale.y = y; }
		void SetScaleZ(const float z) { local_scale.z = z; }
		void AdjustScale(const DirectX::XMFLOAT3& local_scale) { this->local_scale.x += local_scale.x; this->local_scale.y += local_scale.y; this->local_scale.z += local_scale.z; }
		void AdjustScale(const float local_scale) { this->local_scale.x += local_scale; this->local_scale.y += local_scale; this->local_scale.z += local_scale; }
		void AdjustScaleX(const float x) { local_scale.x += x; }
		void AdjustScaleY(const float y) { local_scale.y += y; }
		void AdjustScaleZ(const float z) { local_scale.z += z; }
		[[nodiscard]]
		const DirectX::SimpleMath::Vector3& GetScale() const
		{
			return local_scale;
		}

		//-- Angle --//
		void SetWorldRotation(const DirectX::XMFLOAT3& local_angle) { this->local_angle = local_angle; }
		void SetWorldRotation(const float local_angle) { this->local_angle.x = this->local_angle.y = this->local_angle.z = local_angle; }
		void SetWorldRotation_X(const float x) { local_angle.x = x;}
		void SetWorldRotation_Y(const float y) { local_angle.y = y;}
		void SetWorldRotation_Z(const float z) { local_angle.z = z;}
		void AdjustWorldRotation(const DirectX::XMFLOAT3& local_angle) { this->local_angle.x += local_angle.x; this->local_angle.y += local_angle.y; this->local_angle.z += local_angle.z; }
		void AdjustWorldRotation(const float local_angle) { this->local_angle.x += local_angle; this->local_angle.y += local_angle; this->local_angle.z += local_angle; }
		void AdjustWorldRotation_X(const float x) { local_angle.x += x; }
		void AdjustWorldRotation_Y(const float y) { local_angle.y += y; }
		void AdjustWorldRotation_Z(const float z) { local_angle.z += z; }
		[[nodiscard]]
		auto& GetWorldRotation() { return local_angle; }
		[[nodiscard]]
		const auto& GetWorldRotation() const { return local_angle; }
		[[nodiscard]]
		DirectX::XMFLOAT3 GetDeltaAngle() const
		{
			return DirectX::XMFLOAT3{ local_angle.x - local_prev_angle.x,local_angle.y - local_prev_angle.y,local_angle.z - local_prev_angle.z };
		}

		void NormalizeAngle();
		void CreateIdentity4x4(DirectX::SimpleMath::Matrix& convert);

		//-- World 4x4 matrix --//
		void SetWorld4x4(const DirectX::SimpleMath::Matrix& mat4x4) { world_f4x4 = mat4x4; }
		[[nodiscard]]
		const DirectX::SimpleMath::Matrix& GetWorld4x4() const { return world_f4x4; }

		// Quaternion
		[[nodiscard]]
		bool IsQuaternion() const { return is_quaternion; }
		void ActiveQuaternion()  { is_quaternion = true; }
		void PassiveQuaternion() { is_quaternion = false; }
		/**
		 * @brief : 球面線形補間で使用する値の設定
		 * @param q1 : 補完前のベクトル
		 * @param q2 : 補完後のベクトル
		 */
		void SetQuaternionSlerp(const DirectX::SimpleMath::Quaternion& q1, const DirectX::SimpleMath::Quaternion& q2);
		/**
		 * @brief   : "rotation_prev_quaternion"と"rotation_result_quaternion"間の補完
		 * @param t : 補間制御係数(t == 0 : rotation_prev_quaternion、 t == 1 : rotation_result_quaternion)
		 */
		void QuaternionSlerp(float t);
		void SetRotationQuaternion(const DirectX::SimpleMath::Quaternion& q)		{ rotation_quaternion = q; }
		void SetRotationResultQuaternion(const DirectX::SimpleMath::Quaternion& q)	{ rotation_result_quaternion = q; }
		[[nodiscard]]
		const auto& GetRotationQuaternion()		const { return rotation_quaternion; }
		[[nodiscard]]
		const auto& GetRotationResultQuaternion() const { return rotation_result_quaternion; }

		/**
		 * @brief	    : モデルの任意軸回転(クォータニオン)
		 * @param axis	: 回転軸(正規化済み)
		 * @param local_angle : ラジアン角
		 */
		void AdjustRotationFromAxis(const DirectX::SimpleMath::Vector3& axis, float local_angle);

		[[nodiscard]]
		const DirectX::SimpleMath::Matrix& GetRotationMat()	 const { return rotation_matrix; }
		[[nodiscard]]
		const DirectX::SimpleMath::Matrix& GetScalingMat()	 const { return scaling_matrix; }
		[[nodiscard]]
		const DirectX::SimpleMath::Matrix& GetTranslationMat()   const { return translation_matrix; }

		[[nodiscard]]
		const DirectX::SimpleMath::Vector3& GetModelRight()const { return model_right; }
		[[nodiscard]]
		const DirectX::SimpleMath::Vector3& GetModelUp()   const { return model_up; }
		[[nodiscard]]
		const DirectX::SimpleMath::Vector3& GetModelFront()const { return model_front; }

	private:
		std::unique_ptr<buffer::ConstantBuffer<TransformCB>> cb_transform{};

		DirectX::SimpleMath::Vector3  local_position	{ 0.0f,0.0f,0.0f };
		DirectX::SimpleMath::Vector3  local_prev_pos	{ 0.0f,0.0f,0.0f };
		DirectX::SimpleMath::Vector3  local_scale		{ 1.0f,1.0f,1.0f };
		DirectX::SimpleMath::Vector3  local_angle		{ 0.0f,0.0f,0.0f };
		DirectX::SimpleMath::Vector3  local_prev_angle	{};

		DirectX::SimpleMath::Matrix world_f4x4			{ DirectX::SimpleMath::Matrix::Identity };
		DirectX::SimpleMath::Matrix scaling_matrix		{ DirectX::SimpleMath::Matrix::Identity };
		DirectX::SimpleMath::Matrix rotation_matrix		{ DirectX::SimpleMath::Matrix::Identity };
		DirectX::SimpleMath::Matrix translation_matrix	{ DirectX::SimpleMath::Matrix::Identity };

		const DirectX::SimpleMath::Vector3 right { 1.0f,0.0f,0.0f };
		const DirectX::SimpleMath::Vector3 up	 { 0.0f,1.0f,0.0f };
		const DirectX::SimpleMath::Vector3 front { 0.0f,0.0f,1.0f };

		// Orientation vector
		DirectX::SimpleMath::Vector3 model_right { 1.0f,0.0f,0.0f };
		DirectX::SimpleMath::Vector3 model_up	 { 0.0f,1.0f,0.0f };
		DirectX::SimpleMath::Vector3 model_front { 0.0f,0.0f,1.0f };

		DirectX::SimpleMath::Matrix orientation{ DirectX::SimpleMath::Matrix::Identity };

		// Quaternion
		DirectX::SimpleMath::Quaternion rotation_quaternion{};
		DirectX::SimpleMath::Quaternion rotation_prev_quaternion{};		// Slerp時の変形前のクォータニオン値
		DirectX::SimpleMath::Quaternion rotation_result_quaternion{};	// Slerp時の変形後のクォータニオン値

		bool is_quaternion = false;

		void CreateScaling4x4();
		void CreateRotation4x4();
		void CreateTranslation4x4();
		void CalcModelCoordinateAxis(const DirectX::XMFLOAT4X4& orientation);	// モデルの座標軸計算

		DirectX::XMMATRIX GetRotationMatrix(DirectX::XMFLOAT3 axis, float local_angle/* degree */);
	};
} // cumulonimbus::component