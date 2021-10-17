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
		void SetPosition_X(float x);
		void SetPosition_Y(float y);
		void SetPosition_Z(float z);
		void AdjustPosition(const DirectX::SimpleMath::Vector3& adjust_val);
		void AdjustPosition_X(float x);
		void AdjustPosition_Y(float y);
		void AdjustPosition_Z(float z);

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
		void SetScale_X(float x);
		void SetScale_Y(float y);
		void SetScale_Z(float z);
		void AdjustScale(const DirectX::SimpleMath::Vector3& adjust_val);
		void AdjustScale(float adjust_val);
		void AdjustScale_X(float x);
		void AdjustScale_Y(float y);
		void AdjustScale_Z(float z);
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

		//-- Rotation --//

		void SetRotation(const DirectX::SimpleMath::Quaternion& q);
		void SetRotation(float x, float y, float z, float w);
		void SetEulerAngles(const DirectX::SimpleMath::Vector3& angle);
		void SetEulerAngles(float x, float y, float z);
		void SetEulerAngle_X(float x);
		void SetEulerAngle_Y(float y);
		void SetEulerAngle_Z(float z);
		void AdjustEulerAngles(const DirectX::SimpleMath::Vector3& adjust_val);
		void AdjustEulerAngles(float adjust_x, float adjust_y, float adjust_z);
		[[nodiscard]]
		const DirectX::SimpleMath::Quaternion& GetRotation() const;
		[[nodiscard]]
		DirectX::SimpleMath::Vector3 GetEulerAngles() const;
		[[nodiscard]]
		DirectX::SimpleMath::Vector3 GetDeltaEulerAngles() const;

		//void SetWorldRotation(const DirectX::SimpleMath::Vector3& angle);
		//void SetWorldRotation(float angle);
		//void SetWorldRotation_X(float x);
		//void SetWorldRotation_Y(float y);
		//void SetWorldRotation_Z(float z);
		//void AdjustWorldRotation(const DirectX::SimpleMath::Vector3& adjust_val);
		//void AdjustWorldRotation(float adjust_val);
		//void AdjustWorldRotation_X(float x);
		//void AdjustWorldRotation_Y(float y);
		//void AdjustWorldRotation_Z(float z);
		//[[nodiscard]]
		//auto& GetWorldRotation() { return local_angle; }
		//[[nodiscard]]
		//const auto& GetWorldRotation() const { return local_angle; }
		//[[nodiscard]]
		//DirectX::SimpleMath::Vector3 GetDeltaAngle() const
		//{
		//	return local_angle - local_prev_angle;
		//}

		void NormalizeAngle();
		void CreateIdentity4x4(DirectX::SimpleMath::Matrix& convert);

		//-- World 4x4 matrix --//
		void SetWorldMatrix(const DirectX::SimpleMath::Matrix& mat);
		[[nodiscard]]
		const DirectX::SimpleMath::Matrix& GetWorldMatrix();

		// Quaternion
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

		/**
		 * @brief	    : モデルの任意軸回転(クォータニオン)
		 * @param axis	: 回転軸(正規化済み)
		 * @param local_angle : ラジアン角
		 */
		void AdjustRotationFromAxis(const DirectX::SimpleMath::Vector3& axis, float local_angle);

		[[nodiscard]]
		const DirectX::SimpleMath::Matrix& GetRotationMatrix()	 const { return rotation_matrix; }
		[[nodiscard]]
		const DirectX::SimpleMath::Matrix& GetScalingMatrix()	 const { return scaling_matrix; }
		[[nodiscard]]
		const DirectX::SimpleMath::Matrix& GetTranslationMatrix()   const { return translation_matrix; }

		[[nodiscard]]
		const DirectX::SimpleMath::Vector3& GetModelRight()const { return model_right; }
		[[nodiscard]]
		const DirectX::SimpleMath::Vector3& GetModelUp()   const { return model_up; }
		[[nodiscard]]
		const DirectX::SimpleMath::Vector3& GetModelFront()const { return model_front; }

	private:
		std::unique_ptr<buffer::ConstantBuffer<TransformCB>> cb_transform{};

		DirectX::SimpleMath::Vector3	 local_position		  { 0.0f,0.0f,0.0f };
		DirectX::SimpleMath::Quaternion  local_rotation		  { 0.0f,0.0f,0.0f,1.0f };
		DirectX::SimpleMath::Vector3	 local_scale		  { 1.0f,1.0f,1.0f };
		DirectX::SimpleMath::Vector3	 local_prev_position  {};
		DirectX::SimpleMath::Quaternion	 local_prev_rotation  {};
		DirectX::SimpleMath::Vector3	 local_prev_scale	  {};

		DirectX::SimpleMath::Matrix world_matrix		{ DirectX::SimpleMath::Matrix::Identity };
		DirectX::SimpleMath::Matrix scaling_matrix		{ DirectX::SimpleMath::Matrix::Identity };
		DirectX::SimpleMath::Matrix rotation_matrix		{ DirectX::SimpleMath::Matrix::Identity };
		DirectX::SimpleMath::Matrix translation_matrix	{ DirectX::SimpleMath::Matrix::Identity };

		//const DirectX::SimpleMath::Vector3 right { 1.0f,0.0f,0.0f };
		//const DirectX::SimpleMath::Vector3 up	 { 0.0f,1.0f,0.0f };
		//const DirectX::SimpleMath::Vector3 front { 0.0f,0.0f,1.0f };

		// Orientation vector
		DirectX::SimpleMath::Vector3 model_right { 1.0f,0.0f,0.0f };
		DirectX::SimpleMath::Vector3 model_up	 { 0.0f,1.0f,0.0f };
		DirectX::SimpleMath::Vector3 model_front { 0.0f,0.0f,1.0f };

		DirectX::SimpleMath::Matrix orientation{ DirectX::SimpleMath::Matrix::Identity };

		// その他の回転パラメータ
		DirectX::SimpleMath::Vector3 model_euler_angle{};
		DirectX::SimpleMath::Vector3 model_prev_euler_angle{};
		DirectX::SimpleMath::Quaternion rotation_before{};	// Slerp時の変形前のクォータニオン値
		DirectX::SimpleMath::Quaternion rotation_after{};	// Slerp時の変形後のクォータニオン値

		void CreateScalingMatrix();
		void CreateRotationMatrix();
		void CreateTranslationMatrix();
		void CalcModelCoordinateAxis(const DirectX::XMFLOAT4X4& orientation);	// モデルの座標軸計算

		DirectX::XMMATRIX GetRotationMatrix(DirectX::XMFLOAT3 axis, float local_angle/* degree */);
	};
} // cumulonimbus::component