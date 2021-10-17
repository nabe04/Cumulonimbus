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
		void SetPosition(const DirectX::SimpleMath::Vector3& pos);
		void SetPositionX(float x);
		void SetPositionY(float y);
		void SetPositionZ(float z);
		void AdjustPosition(const DirectX::SimpleMath::Vector3& adjust_val);
		void AdjustPositionX(float x);
		void AdjustPositionY(float y);
		void AdjustPositionZ(float z);

		[[nodiscard]]
		const DirectX::SimpleMath::Vector3& GetPosition()   const
		{
			return local_position;
		}
		[[nodiscard]]
		const DirectX::SimpleMath::Vector3& GetOldPosition() const
		{
			return local_prev_position;
		}
		[[nodiscard]]
		DirectX::SimpleMath::Vector3 GetDeltaPosition() const
		{
			return local_position - local_prev_position;
		}

		//-- Scale --//
		void SetScale(const DirectX::SimpleMath::Vector3& scale);
		void SetScale(float scale);
		void SetScaleX(float x);
		void SetScaleY(float y);
		void SetScaleZ(float z);
		void AdjustScale(const DirectX::SimpleMath::Vector3& adjust_val);
		void AdjustScale(float adjust_val);
		void AdjustScaleX(float x);
		void AdjustScaleY(float y);
		void AdjustScaleZ(float z);
		[[nodiscard]]
		const DirectX::SimpleMath::Vector3& GetScale() const
		{
			return local_scale;
		}
		[[nodiscard]]
		DirectX::SimpleMath::Vector3 GetDeltaScale() const
		{
			return local_scale - local_prev_scale;
		}

		//-- Angle --//
		void SetWorldRotation(const DirectX::SimpleMath::Vector3& angle);
		void SetWorldRotation(float angle);
		void SetWorldRotation_X(float x);
		void SetWorldRotation_Y(float y);
		void SetWorldRotation_Z(float z);
		void AdjustWorldRotation(const DirectX::SimpleMath::Vector3& adjust_val);
		void AdjustWorldRotation(float adjust_val);
		void AdjustWorldRotation_X(float x);
		void AdjustWorldRotation_Y(float y);
		void AdjustWorldRotation_Z(float z);
		[[nodiscard]]
		auto& GetWorldRotation() { return local_angle; }
		[[nodiscard]]
		const auto& GetWorldRotation() const { return local_angle; }
		[[nodiscard]]
		DirectX::SimpleMath::Vector3 GetDeltaAngle() const
		{
			return local_angle - local_prev_angle;
		}

		void NormalizeAngle();
		void CreateIdentity4x4(DirectX::SimpleMath::Matrix& convert);

		//-- World 4x4 matrix --//
		void SetWorld4x4(const DirectX::SimpleMath::Matrix& mat4x4);
		[[nodiscard]]
		const DirectX::SimpleMath::Matrix& GetWorld4x4();

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

		DirectX::SimpleMath::Vector3  local_position	  { 0.0f,0.0f,0.0f };
		DirectX::SimpleMath::Vector3  local_scale		  { 1.0f,1.0f,1.0f };
		DirectX::SimpleMath::Vector3  local_angle		  { 0.0f,0.0f,0.0f };
		DirectX::SimpleMath::Vector3  local_prev_position {};
		DirectX::SimpleMath::Vector3  local_prev_scale	  {};
		DirectX::SimpleMath::Vector3  local_prev_angle	  {};

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

		bool is_quaternion{ false };

		void CreateScaling4x4();
		void CreateRotation4x4();
		void CreateTranslation4x4();
		void CalcModelCoordinateAxis(const DirectX::XMFLOAT4X4& orientation);	// モデルの座標軸計算

		DirectX::XMMATRIX GetRotationMatrix(DirectX::XMFLOAT3 axis, float local_angle/* degree */);
	};
} // cumulonimbus::component