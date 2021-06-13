#pragma once

#include <bitset>
#include <vector>
#include <string>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include <cereal/cereal.hpp>

#include "ecs.h"
#include "component_base.h"
#include "state_machine.h"

class CameraOperationComponent : public Component
{
public:
	enum class CameraState : int
	{
		DefaultTargetCamera = 0,
		LockOnCamera,
		ResetCamera,

		Correction_Free_LockOn,
		Correction_LockOn_Free,

		End,
	};

private:
	std::bitset<static_cast<int>(CameraState::End)> license;
	StateMachine<CameraState, void, const float> camera_state;

	EntityTag my_target_tag;	// エンティティ判別用タグ
	EntityTag other_target_tag;

	DirectX::SimpleMath::Vector3 adjust_target_position;	// カメラ位置(調整値)
	float default_camera_length;	// カメラ距離(デフォルト)
	float default_camera_height;	//カメラ高さ(デフォルト)
	float adjust_camera_length;
	float lock_on_cancel_length = 1;
	float camera_radius = 5;
	float moving_restriction_length = 400;
	bool  is_restriction = false;

	float correction_timer = 1;
	float current_timer = 0;

	DirectX::SimpleMath::Vector2 camera_angle;
	DirectX::SimpleMath::Vector2 ajust_camera_angle;
	DirectX::SimpleMath::Vector2 camera_angle_min;
	DirectX::SimpleMath::Vector2 camera_angle_max;

	DirectX::SimpleMath::Vector3 ajust_camera_position;

	DirectX::SimpleMath::Vector3 correction_vec;
	bool is_operation = true;

	DirectX::SimpleMath::Vector3 camera_position;
	DirectX::SimpleMath::Vector3 prev_camera_position;
	DirectX::SimpleMath::Vector2 camera_speed_gamepad{ 1.0f,1.0f };
	DirectX::SimpleMath::Vector2 camera_speed_mouse{ 0.1f,0.1f };

	DirectX::SimpleMath::Vector3 delta_position{};

	// For ImGui
	std::vector<std::string> state_name;
	float lockon_length = 0;

private:
	void TargetTagCamera(const float delta_time);
	void FreeCamera(const float delta_time);
	void LockOnCamera(const float delta_time);
	void ResetCamera(const float delta_time);
	void Correction_Free_LockOn(const float delta_time);
	void Correction_LockOn_Free(const float delta_time);
	bool MovingRestriction(DirectX::SimpleMath::Vector3 position);

	void CalcCameraAngle();

public:
	CameraOperationComponent(Entity* entity, EntityTag tag, DirectX::XMFLOAT3 camera_pos, float default_camera_length = 50);

	void NewFrame(const float delta_time) override;
	void Update(const float delta_time) override;
	void RenderImGui() override;

	void SetLisence(CameraState bit) { license.set(static_cast<int>(bit)); }
	void ResetLisence(CameraState bit) { license.reset(static_cast<int>(bit)); }

	void AjustCameraTargetPosition(const DirectX::SimpleMath::Vector3& pos) { adjust_target_position = pos; }
	DirectX::SimpleMath::Vector3 const AjustCameraTargetPosition() { return adjust_target_position; }
	void DefaultCameraLength(float length) { default_camera_length = length; }
	float const DefaultCameraLength() { return default_camera_length; }

	void AjustCameraAngle(const DirectX::SimpleMath::Vector2& angle) { camera_angle += angle; }
	DirectX::SimpleMath::Vector2 const AjustCameraAngle() { return ajust_camera_angle; }

	void AjustCameraPosition(const DirectX::SimpleMath::Vector3& position) { ajust_camera_position = position; }
	DirectX::SimpleMath::Vector3 const AjustCameraPositon() { return ajust_camera_position; }

	DirectX::SimpleMath::Vector2 CameraAngleMin() { return camera_angle_min; }
	void CameraAngleMin(DirectX::SimpleMath::Vector2 min) { camera_angle_min = min; }
	DirectX::SimpleMath::Vector2 CameraAngleMax() { return camera_angle_max; }
	void CameraAngleMax(DirectX::SimpleMath::Vector2 max) { camera_angle_max = max; }

	void CameraSpeed_GamePad(const DirectX::SimpleMath::Vector2& speed) { camera_speed_gamepad = speed; }
	DirectX::SimpleMath::Vector2 const CameraSpeed_GamePad() { return camera_speed_gamepad; }
	void CameraSpeed_Mouse(const DirectX::SimpleMath::Vector2& speed) { camera_speed_mouse = speed; }
	DirectX::SimpleMath::Vector2 const CameraSpeed_Mouse() { return camera_speed_mouse; }

	void  LockOnCancelLength(const float length) { lock_on_cancel_length = length; }
	float LockOnCancelLength() const { return lock_on_cancel_length; }

	void CorrectionTimer(float timer) { correction_timer = timer; }
	float CorrectionTimer() const { return correction_timer; }

	void TargetTag(EntityTag tag) { other_target_tag = tag; }
	EntityTag TargetTag() { return other_target_tag; }
};

//namespace cumulonimbus::component
//{
//	//TODO : Adjustの綴りミス,[[nodiscard]]の追加、コメント
//	class CameraOperationComponent : public ComponentBase
//	{
//	public:
//		enum class CameraState : int
//		{
//			DefaultTargetCamera = 0,
//			LockOnCamera,
//			ResetCamera,
//
//			Correction_Free_LockOn,
//			Correction_LockOn_Free,
//
//			End,
//		};
//
//		explicit CameraOperationComponent(ecs::Registry* const registry, const ecs::Entity ent,
//		                         EntityTag tag, DirectX::XMFLOAT3 camera_pos, float default_camera_length = 50);
//		explicit CameraOperationComponent() = default;
//		~CameraOperationComponent() override = default;
//
//		void NewFrame(const float delta_time) override;
//		void Update(const float delta_time) override;
//		void RenderImGui() override;
//
//		void Save(const std::string& file_path) override {}
//		void Load(const std::string& file_path_and_name) override {}
//
//		void SetLisence(CameraState bit) { license.set(static_cast<int>(bit)); }
//		void ResetLisence(CameraState bit) { license.reset(static_cast<int>(bit)); }
//
//		void AjustCameraTargetPosition(const DirectX::SimpleMath::Vector3& pos) { adjust_target_position = pos; }
//		DirectX::SimpleMath::Vector3 const AjustCameraTargetPosition() { return adjust_target_position; }
//		void DefaultCameraLength(float length) { default_camera_length = length; }
//		float const DefaultCameraLength() { return default_camera_length; }
//
//		void AjustCameraAngle(const DirectX::SimpleMath::Vector2& angle) { camera_angle += angle; }
//		DirectX::SimpleMath::Vector2 const AjustCameraAngle() { return ajust_camera_angle; }
//
//		void AjustCameraPosition(const DirectX::SimpleMath::Vector3& position) { ajust_camera_position = position; }
//		DirectX::SimpleMath::Vector3 const AjustCameraPositon() { return ajust_camera_position; }
//
//		DirectX::SimpleMath::Vector2 CameraAngleMin() { return camera_angle_min; }
//		void CameraAngleMin(DirectX::SimpleMath::Vector2 min) { camera_angle_min = min; }
//		DirectX::SimpleMath::Vector2 CameraAngleMax() { return camera_angle_max; }
//		void CameraAngleMax(DirectX::SimpleMath::Vector2 max) { camera_angle_max = max; }
//
//		void CameraSpeed_GamePad(const DirectX::SimpleMath::Vector2& speed) { camera_speed_gamepad = speed; }
//		DirectX::SimpleMath::Vector2 const CameraSpeed_GamePad() { return camera_speed_gamepad; }
//		void CameraSpeed_Mouse(const DirectX::SimpleMath::Vector2& speed) { camera_speed_mouse = speed; }
//		DirectX::SimpleMath::Vector2 const CameraSpeed_Mouse() { return camera_speed_mouse; }
//
//		void  LockOnCancelLength(const float length) { lock_on_cancel_length = length; }
//		float LockOnCancelLength() const { return lock_on_cancel_length; }
//
//		void CorrectionTimer(float timer) { correction_timer = timer; }
//		float CorrectionTimer() const { return correction_timer; }
//
//		void TargetTag(EntityTag tag) { other_target_tag = tag; }
//		EntityTag TargetTag() { return other_target_tag; }
//
//	private:
//		std::bitset<static_cast<int>(CameraState::End)> license;
//		StateMachine<CameraState, void, const float> camera_state;
//
//		EntityTag my_target_tag;	// エンティティ判別用タグ
//		EntityTag other_target_tag;
//
//		DirectX::SimpleMath::Vector3 adjust_target_position{};	// カメラ位置(調整値)
//		float default_camera_length{};	// カメラ距離(デフォルト)
//		float default_camera_height{};	//カメラ高さ(デフォルト)
//		float adjust_camera_length{};
//		float lock_on_cancel_length{1};
//		float camera_radius{5};
//		float moving_restriction_length{400};
//		bool  is_restriction{false};
//
//		float correction_timer{1};
//		float current_timer{0};
//
//		DirectX::SimpleMath::Vector2 camera_angle{};
//		DirectX::SimpleMath::Vector2 ajust_camera_angle{};
//		DirectX::SimpleMath::Vector2 camera_angle_min{};
//		DirectX::SimpleMath::Vector2 camera_angle_max{};
//
//		DirectX::SimpleMath::Vector3 ajust_camera_position{};
//
//		DirectX::SimpleMath::Vector3 correction_vec{};
//		bool is_operation = true;
//
//		DirectX::SimpleMath::Vector3 camera_position{};
//		DirectX::SimpleMath::Vector3 prev_camera_position{};
//		DirectX::SimpleMath::Vector2 camera_speed_gamepad{ 1.0f,1.0f };
//		DirectX::SimpleMath::Vector2 camera_speed_mouse{ 0.1f,0.1f };
//
//		DirectX::SimpleMath::Vector3 delta_position{};
//
//		// For ImGui
//		std::vector<std::string> state_name{};
//		float lockon_length{0};
//
//
//		void TargetTagCamera(const float delta_time);
//		void FreeCamera(const float delta_time);
//		void LockOnCamera(const float delta_time);
//		void ResetCamera(const float delta_time);
//		void Correction_Free_LockOn(const float delta_time);
//		void Correction_LockOn_Free(const float delta_time);
//		bool MovingRestriction(DirectX::SimpleMath::Vector3 position);
//
//		void CalcCameraAngle();
//	};
//}
//
//CEREAL_REGISTER_TYPE(cumulonimbus::component::CameraOperationComponent);
//CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::CameraOperationComponent)
