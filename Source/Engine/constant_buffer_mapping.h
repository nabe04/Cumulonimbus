#ifndef CONSTANT_BUFFER_MAPPING_H
#define CONSTANT_BUFFER_MAPPING_H

// �S�ẴV�F�[�_�[�ɋ��ʂ��Ďg�p����
// 5�`9�Ԃ͍��㋤�ʂ��Ďg�p����\�����l���ċ󂯂Ă���
#define CBSlot_Renderer_Frame	0
#define CBSlot_Camera			1	// �J����
#define CBSlot_Transform		2	// �g�����X�t�H�[��
#define CBSlot_Material			3	// �}�e���A��
#define CBSlot_DirectionalLight	4	// �f�B���N�V���i�����C�g
#define CBSlot_PointLight		5	// �|�C���g���C�g
#define CBSlot_SpotLight		6	// �X�|�b�g���C�g

// �I���f�}���h
#define CBSlot_OnDemand0		10
#define CBSlot_OnDemand1		11
#define CBSlot_OnDemand2		12
#define CBSlot_OnDemand3		13
#define CBSlot_OnDemand4		14

// ���ꂼ��̃V�F�[�_�[�Ǝ���CBuffer(3D���f���p)
#define CBSlot_Phong				CBSlot_OnDemand0
#define CBSlot_Metal				CBSlot_OnDemand0
#define CBSlot_Toon					CBSlot_OnDemand0
#define CBSlot_ReflectionMapping	CBSlot_OnDemand0
#define CBSlot_RefractionMapping	CBSlot_OnDemand0
#define CBSlot_SingleColor			CBSlot_OnDemand0
// ���ꂼ��̃V�F�[�_�[�Ǝ���CBuffer(2D�X�v���C�g�p)
#define CBSlot_Sprite				CBSlot_OnDemand0
#define CBSlot_Billboard			CBSlot_OnDemand0

// ���̑��̃V�F�[�_�[
#define CBSlot_DebugCollision		CBSlot_OnDemand0
#define CBSlot_DebugPrimitive		CBSlot_OnDemand0

// �|�C���g & �X�|�b�g���C�g���
#define CBSlot_LightInfo			CBSlot_OnDemand0
#endif // CONSTANT_BUFFER_MAPPING_H