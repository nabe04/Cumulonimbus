#pragma once

#include <bitset>
#include <vector>
#include <string>

#include <d3d11.h>

#include <DirectXMath.h>
#include <SimpleMath.h>

#include "ecs.h"
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
	std::bitset<static_cast<int>(CameraState::End)> lisence;
	StateMachine<CameraState,void ,const float> camera_state;

	EntityTag my_target_tag; 	// �����_�ɐݒ肷��^�O(����)
	EntityTag ohter_target_tag;	// �����_�ɐݒ肷��^�O(�ϓ�)

	DirectX::SimpleMath::Vector3 ajust_target_position;	// �J�����̒����_�������ʒu�̒����l
	float default_camera_length;	// �f�t�H���g�̒����_����J�����܂ł̈ʒu�̑傫��
	float default_camera_height;	// �f�t�H���g�̃J�����̍���
	float ajust_camera_length;		// �J�����̋��������p(�Ǎۂ̒����Ȃǂ̎g�p)
	float lockon_cancel_lenght = 1;	// ���b�N�I�����Ɏ����Ƒ���ŃJ�����̒����_���}�ɕς��̂�h�����߂̋���
	float camera_radius = 5;		// �ړ������Ɏg���Ƃ��̃J�������g�̋��̑傫��
	float moving_restriction_length = 400; // �J�����̈ړ������p�ϐ�
	bool  is_restriction = false;

	float correction_timer = 1;	// �J�����̕⊮�����������̎���
	float current_timer = 0;

	DirectX::SimpleMath::Vector2 camera_angle;
	DirectX::SimpleMath::Vector2 ajust_camera_angle;	 // �J���������̒����l
	DirectX::SimpleMath::Vector2 camera_angle_min;		 // �J�����p�x(�ŏ��l)
	DirectX::SimpleMath::Vector2 camera_angle_max;		 // �J�����p�x(�ő�l)

	DirectX::SimpleMath::Vector3 ajust_camera_position;	 // �J�����ʒu�̒����l

	DirectX::SimpleMath::Vector3 correction_vec;
	bool is_operation = true;	// �J���������K�p���邩

	DirectX::SimpleMath::Vector3 camera_position;
	DirectX::SimpleMath::Vector3 prev_camera_position;
	DirectX::SimpleMath::Vector2 camera_speed_gamepad{1.0f,1.0f};
	DirectX::SimpleMath::Vector2 camera_speed_mouse{ 0.1f,0.1f };

	DirectX::SimpleMath::Vector3 delta_position{};

	// For Imgui
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
	CameraOperationComponent(Entity* entity, EntityTag tag ,DirectX::XMFLOAT3 camera_pos, float default_camera_length = 50);

	void NewFrame(const float delta_time) override;
	void Update(const float delta_time) override;
	void RenderImGui() override;

	void SetLisence(CameraState bit)   { lisence.set(static_cast<int>(bit)); }
	void ResetLisence(CameraState bit) { lisence.reset(static_cast<int>(bit)); }

	void AjustCameraTargetPosition(const DirectX::SimpleMath::Vector3& pos) { ajust_target_position = pos; }
	DirectX::SimpleMath::Vector3 const AjustCameraTargetPosition() { return ajust_target_position; }
	void DefaultCameraLength(float length) { default_camera_length = length; }
	float const DefaultCameraLength() { return default_camera_length; }

	void AjustCameraAngle(const DirectX::SimpleMath::Vector2& angle) { camera_angle += angle; }
	DirectX::SimpleMath::Vector2 const AjustCameraAngle() { return ajust_camera_angle; }

	void AjustCameraPosition(const DirectX::SimpleMath::Vector3& position) { ajust_camera_position = position; }
	DirectX::SimpleMath::Vector3 const AjustCameraPositon() {	return ajust_camera_position;}

	DirectX::SimpleMath::Vector2 CameraAngleMin() { return camera_angle_min; }
	void CameraAngleMin(DirectX::SimpleMath::Vector2 min) { camera_angle_min = min; }
	DirectX::SimpleMath::Vector2 CameraAngleMax() { return camera_angle_max; }
	void CameraAngleMax(DirectX::SimpleMath::Vector2 max) { camera_angle_max = max; }

	void CameraSpeed_GamePad(const DirectX::SimpleMath::Vector2& speed) { camera_speed_gamepad = speed; }
	DirectX::SimpleMath::Vector2 const CameraSpeed_GamePad() { return camera_speed_gamepad; }
	void CameraSpeed_Mouse(const DirectX::SimpleMath::Vector2& speed) { camera_speed_mouse = speed; }
	DirectX::SimpleMath::Vector2 const CameraSpeed_Mouse() { return camera_speed_mouse; }

	void  LockOnCancelLength(const float length) { lockon_cancel_lenght = length; }
	float LockOnCancelLength() const { return lockon_cancel_lenght; }

	void CorrectionTimer(float timer) { correction_timer = timer; }
	float CorrectionTimer() const { return correction_timer; }

	void TargetTag(EntityTag tag) { ohter_target_tag = tag; }
	EntityTag TargetTag() { return ohter_target_tag; }
};