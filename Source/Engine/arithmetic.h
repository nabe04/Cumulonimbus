#pragma once
#include <string>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

namespace arithmetic
{
	/**
	 * @brief : �����̒�`
	 */
	struct Line
	{
		DirectX::SimpleMath::Vector3 point;	// ������̔C�ӂ̓_
		DirectX::SimpleMath::Vector3 vec;	// �����̕����x�N�g��
	};

	/**
	 * @brief : �E�B���h�E�䗦�����ɃE�B���h�E�T�C�Y���v�Z
	 * @remark : width�̃p�����[�^��D��ɃT�C�Y���v�Z����
	 * @remark : width����Z�o���ꂽ�v�Z��̃E�B���h�E�c����
	 *			 PC�̏c��ʂ̃T�C�Y�����傫�������ꍇheight
	 *			 �̃p�����[�^����E�B���h�E�T�C�Y���Z�o����
	 * @param aspect_ratio : �E�B���h�E�䗦
	 * @param width : �v�Z�O�̃E�B���h�E����
	 * @param height : �v�Z�O�̃E�B���h�E�c��
	 * @return : aspect_ratio�Ɋ�Â��čČv�Z���ꂽ�E�B���h�E�T�C�Y
	 */
	[[nodiscard]]
	DirectX::SimpleMath::Vector2 CalcWindowSize(
		const DirectX::XMUINT2& aspect_ratio,
		int width, int height);

	/**
	 * @brief  : �ۂߌ덷���l���������l�֐�
	 * @return : true -> ���l
	 */
	[[nodiscard]]
	bool IsEqual(float l_val, float r_val);

	/**
	 * @brief : [min,max]�͈̔͂܂�n���N�����v
	 */
	[[nodiscard]]
	float Clamp(float n, float min, float max);

	/**
	 * @brief : �X�N���[�����W����NDC���W�ւ̕ϊ�
	 * @param screen_pos : �X�N���[���ʒu(Z�l�͖��������)
	 * @param window_width : �E�B���h�E��
	 * @param window_height : �E�B���h�E����
	 * @return : NDC��ԏ�̍��W
	 */
	[[nodiscard]]
	DirectX::SimpleMath::Vector2 ConvertScreenToNDC(const DirectX::SimpleMath::Vector3& screen_pos,
													float window_width, float window_height);
	/**
	 * @brief : �X�N���[�����W����NDC���W�ւ̕ϊ�
	 * @param screen_pos : �X�N���[���ʒu(Z�l�͖��������)
	 * @param window_width : �E�B���h�E��
	 * @param window_height : �E�B���h�E����
	 * @return : NDC��ԏ�̍��W
	 */
	[[nodiscard]]
	DirectX::SimpleMath::Vector2 ConvertScreenToNDC(const DirectX::SimpleMath::Vector2& screen_pos,
													float window_width, float window_height);

	/**
	 * @brief : �X�N���[�����W���烏�[���h���W�ւ̕ϊ�
	 * @param screen_pos : �X�N���[���ʒu(z�l���܂�)
	 * @param view_mat : �r���[�s��
	 * @param projection_mat : �v���W�F�N�V�����s��
	 * @return :
	 */
	[[nodiscard]]
	DirectX::SimpleMath::Vector3 ConvertScreenToWorld(DirectX::SimpleMath::Vector3 screen_pos,
													  const DirectX::SimpleMath::Matrix& view_mat,
													  const DirectX::SimpleMath::Matrix& projection_mat);

	[[nodiscard]]
	DirectX::XMFLOAT2 CalcVecFromTwoPositions(DirectX::XMFLOAT2 vec1, DirectX::XMFLOAT2 vec2);
	[[nodiscard]]
	DirectX::XMFLOAT3 CalcVecFromTwoPositions(DirectX::XMFLOAT3 vec1, DirectX::XMFLOAT3 vec2);

	/**
	* @brief		: up�x�N�g���̎Z�o
	* @param front  : front vector(z��)
	* @param right  : right vector(x��)
	*/
	[[nodiscard]]
	DirectX::XMFLOAT3 CalcUpVec(
		const DirectX::XMFLOAT3& front,
		const DirectX::XMFLOAT3& right);

	/**
	* @brief		: right�x�N�g���̎Z�o
	* @param up     : up vector(y��)
	* @param front  : front vector(z��)
	*/
	[[nodiscard]]
	DirectX::XMFLOAT3 CalcRightVec(
		const DirectX::XMFLOAT3& up,
		const DirectX::XMFLOAT3& front);

	/**
	* @brief	   : calculate front vector
	* @param right : right vector(x��)
	* @param up    : up vector(y��)
	*/
	[[nodiscard]]
	DirectX::XMFLOAT3 CalcFrontVec(
		const DirectX::XMFLOAT3& right,
		const DirectX::XMFLOAT3& up);

	[[nodiscard]]
	DirectX::XMFLOAT3 SphereLinear(
		const DirectX::XMVECTOR& v1,
		const DirectX::XMVECTOR& v2, float s);

	/**
	 * @brief	 : �p�x��(-180�� �` 180��)�ɐ��K��
	 * @example	 : 190�� -> -170���A-200�� -> 160��
	 */
	[[nodiscard]]
	float NormalizeAngle(const float angle);

	/**
	 * @brief     : ��̃x�N�g������p�x(���W�A��)���Z�o
	 * @attention : �����Ɏw�肷��x�N�g���͐��K���ς݂̃x�N�g�����g�p���鎖
	 * @return    : �ʓx�@�ł̊p�x
	 */
	[[nodiscard]]
	float CalcAngleFromTwoVec(
		const DirectX::SimpleMath::Vector3& v0,
		const DirectX::SimpleMath::Vector3& v1);

	/**
	 * @brief : ������W�n�����ɂ������̂��ꂼ��̊p�x
	 */
	[[nodiscard]] float CalcAngle_X(const DirectX::SimpleMath::Vector3& v);
	[[nodiscard]] float CalcAngle_Y(const DirectX::SimpleMath::Vector3& v);
	[[nodiscard]] float CalcAngle_Z(const DirectX::SimpleMath::Vector3& v);

	/**
	 * @brief : �N�H�[�^�j�I������I�C���[�p�̎Z�o
	 */
	DirectX::SimpleMath::Vector3 QuaternionToEulerAngle(const DirectX::SimpleMath::Quaternion& q);

	/**
	 * @brief				: ���ˉe�x�N�g���̎Z�o
	 *						  project_vec ���� onto_vec�ւ̐��ˉe�x�N�g�����Z�o����
	 * @param project_vec	: �ˉe���x�N�g��
	 * @param onto_vec		: �ˉe��x�N�g��
	 */
	[[nodiscard]]
	DirectX::XMFLOAT3 CalcProjectVector(
		const DirectX::XMFLOAT3& project_vec,
		const DirectX::XMFLOAT3& onto_vec);

	/**
	 * @brief  : ��������(int)
	 * @return : min����max�͈̔͂ł̗���(int)
	 */
	[[nodiscard]]
	int RandomIntInRange(int min, int max);
	/**
	 * @brief  : ��������(float)
	 * @return : min����max�͈̔͂ł̗���(float)
	 */
	[[nodiscard]]
	float RandomFloatInRange(float min, float max);

	/**
	 * @brief  : 2�̃x�N�g�������s�ǂ�������
	 * @return : true -> ���s�A false -> ���s����Ȃ�
	 */
	[[nodiscard]]
	bool IsParallel(
		const DirectX::SimpleMath::Vector3& v1,
		const DirectX::SimpleMath::Vector3& v2);

	/**
	 * @brief			: �����Ɠ_�Ƃ̍ŋߐړ_���Z�o
	 * @param l_start	: �����̒[�_(���̂܂܂��Ɛ����ɂȂ�̂Ŋ֐�����Line�ɕϊ�)
	 * @param l_end		: �����̒[�_(���̂܂܂��Ɛ����ɂȂ�̂Ŋ֐�����Line�ɕϊ�)
	 * @param p			: �C�ӂ̓_
	 * @return			: ������̍ŋߐړ_�̈ʒu
	 */
	[[nodiscard]]
	DirectX::SimpleMath::Vector3 ClosestPtPointLine(
		const DirectX::SimpleMath::Vector3& l_start,
		const DirectX::SimpleMath::Vector3& l_end,
		const DirectX::SimpleMath::Vector3& p
	);

	/**
	 * @brief	: �����Ɠ_�Ƃ̍ŋߐړ_���Z�o
	 * @param l	: ����l
	 * @param p	: �C�ӂ̓_p
	 * @return  : ������̍ŋߐړ_�̈ʒu
	 */
	[[nodiscard]]
	DirectX::SimpleMath::Vector3 ClosestPtPointLine(
		const Line& l,
		const DirectX::SimpleMath::Vector3& p
	);

	/**
	 * @brief   : �_c�Ɛ���ab�̍ŒZ�����̕����̎Z�o
	 * @param a : ����ab�̒[�_a
	 * @param b : ����ab�̒[�_b
	 * @param c : �_c
	 * @return  : �ŒZ�����̕����̒l
	 */
	[[nodiscard]]
	float SqDistPointSegment(
		const DirectX::SimpleMath::Vector3& a,
		const DirectX::SimpleMath::Vector3& b,
		const DirectX::SimpleMath::Vector3& c
	);

	/**
	 * @brief	  : �^����ꂽ����ab����ѓ_c�ɑ΂��āAab��̍ŋߐړ_d���v�Z
	 *			  : d(t) = a + t * (b - a)�ɂ��\�����d�̈ʒu�ɑ΂���t���Ԃ�
	 * @param a	  : ����ab�̒[�_
	 * @param b   : ����ab�̒[�_
	 * @param c	  : �C�ӂ̓_
	 * @param d	  : �ŋߐړ_
	 * @param t	  : d�̈ʒu�ɑ΂���t
	 * @attention : d,t�͎Q�Ɠn�������Ă���̂ŎZ�o���ꂽ���ʂ����邱�Ƃɒ���
	 */
	void ClosestPtPointSegment(
		const DirectX::SimpleMath::Vector3& a,
		const DirectX::SimpleMath::Vector3& b,
		const DirectX::SimpleMath::Vector3& c,
		DirectX::SimpleMath::Vector3& d,
		float& t);

	/**
	 * @brief			: 2�����̍ŒZ�����̎Z�o
	 * @param l1_start	: �����̒[�_(���̂܂܂��Ɛ����ɂȂ�̂Ŋ֐�����Line�ɕϊ�)
	 * @param l1_end    : �����̒[�_(���̂܂܂��Ɛ����ɂȂ�̂Ŋ֐�����Line�ɕϊ�)
	 * @param l2_start  : �����̒[�_(���̂܂܂��Ɛ����ɂȂ�̂Ŋ֐�����Line�ɕϊ�)
	 * @param l2_end    : �����̒[�_(���̂܂܂��Ɛ����ɂȂ�̂Ŋ֐�����Line�ɕϊ�)
	 * @param p1		: ����l1�̐����̈ʒu
	 * @param p2		: ����l2�̐����̈ʒu
	 * @return			: 2�����̍ŒZ����
	 */
	[[nodiscard]]
	float ClosestPtLineLine(
		const DirectX::SimpleMath::Vector3& l1_start,
		const DirectX::SimpleMath::Vector3& l1_end,
		const DirectX::SimpleMath::Vector3& l2_start,
		const DirectX::SimpleMath::Vector3& l2_end,
		DirectX::SimpleMath::Vector3& p1,
		DirectX::SimpleMath::Vector3& p2
	);

	/**
	 * @brief			: 2�����̍ŒZ�����̎Z�o
	 * @param l1		: �����̐ݒ�
	 * @param l2		: �����̐ݒ�
	 * @param p1		: ����l1�̐����̈ʒu
	 * @param p2		: ����l2�̐����̈ʒu
	 * @return			: 2�����̍ŒZ����
	 */
	[[nodiscard]]
	float ClosestPtLineLine(
		const Line& l1,
		const Line& l2,
		DirectX::SimpleMath::Vector3& p1,
		DirectX::SimpleMath::Vector3& p2
	);

	/**
	 * @brief			: �����Ɛ����̍ŋߐړ_�ƍŋߐړ_�Ԃ̋����̎Z�o
	 * @param p_start	: ����p�̎n�_
	 * @param p_end		: ����p�̏I�_
	 * @param q_start	: ����q�̎n�_
	 * @param q_end		: ����q�̏I�_
	 * @param s			: S1(s) = p_start + s * (p_end - p_start)
	 * @param t			: S2(t) = q_start + t * (q_end - q_start)
	 * @param c1		: �_p�̍ŋߐړ_
	 * @param c2		: �_q�̍ŋߐړ_
	 * @return			: �ŋߐړ_�Ԃ̋���(�ŒZ����)
	 */
	[[nodiscard]]
	float ClosestPtSegmentSegment(
		const DirectX::SimpleMath::Vector3& p_start,
		const DirectX::SimpleMath::Vector3& p_end,
		const DirectX::SimpleMath::Vector3& q_start,
		const DirectX::SimpleMath::Vector3& q_end,
		float& s, float& t,
		DirectX::SimpleMath::Vector3& c1,
		DirectX::SimpleMath::Vector3& c2);
} // arithmetic