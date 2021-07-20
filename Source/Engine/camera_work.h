#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "view.h"
#include "rename_type_mapping.h"

namespace cumulonimbus::ecs
{
	class Registry;
} // cumulonimbus::ecs

class CameraWork final
{
public:
	CameraWork(cumulonimbus::ecs::Registry* registry);
	~CameraWork() = default;

	void Update(float dt);
	void RenderImGui();

#pragma region
	/*
	 * brief    : �J�����̈ʒu�A�����_�A�A�b�v�x�N�g���̃Z�b�g
	 */
	void SetCameraInfo(
		const DirectX::SimpleMath::Vector3& eye_position /* �ʒu */,
		const DirectX::SimpleMath::Vector3& target       /* �����_ */,
		const DirectX::SimpleMath::Vector3& up_vec       /* �A�b�v�x�N�g��*/);
	void SetCameraInfo(const View& v);

	void SetPosition(const DirectX::SimpleMath::Vector3& eye_position);
	void SetTargetVec(const DirectX::SimpleMath::Vector3& target);
	void SetCameraUp(const DirectX::SimpleMath::Vector3& up);
	void SetCameraUpRightFrontVector(const DirectX::SimpleMath::Vector3& up,
									 const DirectX::SimpleMath::Vector3& right,
									 const DirectX::SimpleMath::Vector3& front);

	void SetCameraSpeed(const DirectX::SimpleMath::Vector2& speed) { this->camera_velocity = speed; }

	[[nodiscard]] const DirectX::SimpleMath::Vector3& GetPosition() const { return eye_position; }
	[[nodiscard]] const DirectX::SimpleMath::Vector3& GetFocusPosition() const { return focus_position; }
	[[nodiscard]] const DirectX::SimpleMath::Vector3& GetCameraFront() const { return front_vec; }
#pragma endregion Setter & Getter

private:
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

	void CalcCameraDirectionalVector();
	void CalcCameraAngle();
};
