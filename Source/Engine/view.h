#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "constant_buffer.h"
#include "shader_interop_renderer.h"
#include "rename_type_mapping.h"

class CameraWork;

namespace cumulonimbus::ecs
{
	class Registry;
} // cumulonimbus::ecs

class View final
{
public:
	explicit View(cumulonimbus::ecs::Registry* registry);
	~View();

	void BindCBuffer(bool set_in_vs = true, bool set_in_ps = true) const;
	void UnbindCBuffer() const;

	void Update(float dt);
	void WriteImGui();

	/**
	 * @brief						: �I�u�W�F�N�g�J�������g�p����ۂ�
	 *								  �Ώۃ��f���̃G���e�B�e�B���w��
	 * @param ent					: �Ώۃ��f���̃G���e�B�e�B
	 * @param switch_object_camera	: �J�����������I�u�W�F�N�g�p�ɂ��邩(true : �I�u�W�F�N�g�p�J�������g�p)
	 */
	void AttachObject(cumulonimbus::mapping::rename_type::Entity ent, bool switch_object_camera = true);

	/**
	 * @brief				: �I�u�W�F�N�g�p�J�����̃p�����[�^�̏�����
	 *						  �J�����̏����ʒu���I�u�W�F�N�g�̌���ɃZ�b�g
	 *						  (�I�u�W�F�N�g�̃t�����g�x�N�g�� * -1) * camera_length
	 * @param camera_length : �I�u�W�F�N�g�ƃJ�����Ԃ̒���(�f�t�H���g == 50)
	 */
	void InitializeObjectCameraParam(float camera_length = 50.0f);

	void SetViewInfo(
		DirectX::SimpleMath::Vector3 eye_position,
		DirectX::SimpleMath::Vector3 target,
		DirectX::SimpleMath::Vector3 up) const;
	void SetProjection(float fov, float aspect, float min, float max);
	void SetOrtho(float width, float height, float min, float max);
	void SetFocusPosition(const DirectX::SimpleMath::Vector3& target) const { cb_camera->data.camera_at= target; }

	void SetEyePosition(const DirectX::SimpleMath::Vector3& pos) const  { cb_camera->data.camera_position = pos; }
	void SetTargetVec(const DirectX::SimpleMath::Vector3& target);
	void SetCameraUp(const DirectX::SimpleMath::Vector3& up);
	void SetCameraUpRightFrontVector(
		const DirectX::SimpleMath::Vector3& up,
		const DirectX::SimpleMath::Vector3& right,
		const DirectX::SimpleMath::Vector3& front);

	void SetCameraSpeed(const DirectX::SimpleMath::Vector2& speed) { this->camera_velocity = speed; }
	
	[[nodiscard]] DirectX::SimpleMath::Matrix GetView()			const   { return cb_camera->data.camera_view_matrix; }
	[[nodiscard]] DirectX::SimpleMath::Matrix GetProjection()	const	{ return cb_camera->data.camera_view_projection_matrix; }

	[[nodiscard]] DirectX::SimpleMath::Vector3 GetEyePosition()   const { return cb_camera->data.camera_position; }
	[[nodiscard]] DirectX::SimpleMath::Vector3 GetFocusPosition() const { return cb_camera->data.camera_at; }

	// �J�����̌����擾
	[[nodiscard]]DirectX::SimpleMath::Vector3 GetCameraRight() const { return cb_camera->data.camera_right; }
	[[nodiscard]]DirectX::SimpleMath::Vector3 GetCameraUp()    const { return cb_camera->data.camera_up; }
	[[nodiscard]]DirectX::SimpleMath::Vector3 GetCameraFront() const { return cb_camera->data.camera_front; }

	void SetCameraRight(const DirectX::SimpleMath::Vector3& right) const { cb_camera->data.camera_right = right; }
	void SetCameraUp(const DirectX::SimpleMath::Vector3& up)	   const { cb_camera->data.camera_up = up; }
	void SetCameraFront(const DirectX::SimpleMath::Vector3& front) const { cb_camera->data.camera_front = front; }
	void SetFov(const float fov) const { cb_camera->data.camera_fov = fov; }

	[[nodiscard]] float GetFov()    const { return cb_camera->data.camera_fov; }
	[[nodiscard]] float GetAspect() const { return cb_camera->data.camera_aspect; }
	[[nodiscard]] float GetNearZ()  const { return cb_camera->data.camera_near_z; }
	[[nodiscard]] float GetFarZ()   const { return cb_camera->data.camera_far_z; }

	//[[nodiscard]] CameraWork* GetCameraWork() const { return camera_work.get(); }

	void IsDebugCamera(const bool flg) { is_debug_camera = flg; }
	[[nodiscard]] bool IsDebugCamera() const { return is_debug_camera; }

private:
	//std::unique_ptr<CameraWork> camera_work{};
	std::unique_ptr<buffer::ConstantBuffer<CameraCB>> cb_camera;

	cumulonimbus::ecs::Registry* registry{ nullptr };

	// �J�����̍ő�p�x(���) (y,z�͖���`)
	const DirectX::SimpleMath::Vector3 max_camera_angle{ 85.0f,0,0 };

	DirectX::SimpleMath::Vector3 eye_position{ 0.0f,0.0f,0.0f };
	DirectX::SimpleMath::Vector3 focus_position{ 0.0f,0.0f,1.0f };

	DirectX::SimpleMath::Vector3 up_vec{ 0.0f,1.0f,0.0f };		// �J������Up�x�N�g�������ۂɃZ�b�g����Ƃ��Ɏg�p����x�N�g��
	DirectX::SimpleMath::Vector3 right_vec{ 1.0f,0.0f,0.0f };
	DirectX::SimpleMath::Vector3 front_vec{ focus_position.x - eye_position.x,
											focus_position.y - eye_position.y,
											focus_position.z - eye_position.z };

	DirectX::SimpleMath::Vector3 current_camera_up{ .0f,1.f,.0f };		// �J�����̂��ꂼ��̃x�N�g�������s�ɂȂ�悤�ɕ␳�����x�N�g��(�v�Z�ȂǂɎg�p)
	DirectX::SimpleMath::Vector3 camera_right{ 1.0f,0.0f,0.0f };

	DirectX::SimpleMath::Matrix view_f4x4{ DirectX::SimpleMath::Matrix::Identity };
	DirectX::SimpleMath::Matrix projection_f4x4{ DirectX::SimpleMath::Matrix::Identity };

	DirectX::SimpleMath::Vector3 camera_angle{}; // �J�����̊p�x(Degree),����n�̍��W������̊p�x

	//-- for debug --//
	DirectX::SimpleMath::Vector2 camera_velocity{ 3.f,3.f };

	//-- �J�����ƃI�u�W�F�N�g�̃A�^�b�`�p�ϐ� --//
	cumulonimbus::mapping::rename_type::Entity attach_entity;	// �A�^�b�`����I�u�W�F�N�g�̃G���e�B�e�B
	bool is_use_camera_for_object = false; // �I�u�W�F�N�g�A�^�b�`�p�t���O(true : �I�u�W�F�N�g�ɃA�^�b�`����Ă���)
	float camera_length; // �I�u�W�F�N�g�ƃJ�����̒���

	DirectX::SimpleMath::Vector3 view_up_vec{ 0.0f,1.0f,0.0f };
	bool  is_perspective  = true;
	bool  is_debug_camera = false;

	//-- Camera controlled activate mouse eye_position --//
	DirectX::SimpleMath::Vector2 cur_mouse_pos{ 0.0f,0.0f };		// Current mouse eye_position
	DirectX::SimpleMath::Vector2 prev_mouse_pos{ 0.0f,0.0f };		// Mouse eye_position one frame ago

	/**
	 * @brier : ���s�x�N�g��(right,up,front)�̎Z�o
	 */
	void CalcCameraOrthogonalVector();

	/**
	 * @brief : �I�u�W�F�N�g�A�^�b�`���̍X�V�֐�
	 *			is_use_camera_for_object == true���Ɏ��s�����
	 */
	void UpdateObjectCamera(float dt);

	/**
	 * @brief : ��I�u�W�F�N�g�A�^�b�`���̍X�V�֐�
	 *			is_use_camera_for_object == false���Ɏ��s�����
	 */
	void UpdateDefaultCamera(float dt);

	/**
	 * @brief   : ���݂̃J�����̐��K���s�x�N�g�����Z�o
	 */
	void CalcCameraDirectionalVector();

	/**
	 * @brief : ������W������ɂ��ẴJ�����̊p�x(Degree)�v�Z
	 */
	void CalcCameraAngle();

	/**
	 * @brief			: �J�������[�N���̓���
	 * @param velocity	: �J�����X�s�[�h(�f�o�b�N�̐ݒ�ŕύX�\)
	 */
	void Pan(float velocity);								// �J�����̍��E�̌X��(�ʒu�͌Œ�)
	void Tilt(float velocity);								// �J�����̏㉺�̌X��(�ʒu�͌Œ�)
	void DollyInOut(float velocity);						// �J�����̑O��̓���(�����͌Œ�)
	void Track(float velocity,
		const DirectX::SimpleMath::Vector3& axis /*���*/);								// �J�����̍��E�ړ�(�����͌Œ�)
	void Crane(float velocity,
		const DirectX::SimpleMath::Vector3& axis /*���*/);	// �J�����̏㉺�ړ�(�����͌Œ�)
};


