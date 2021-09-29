#ifndef SHADER_ASSET_MAPPING_H
#define SHADER_ASSET_MAPPING_H

#ifdef __cplusplus

namespace cumulonimbus::mapping::shader_assets
{
	/*
	 * brief	 : ���f����`�悷��ۂɎg�p����V�F�[�_�[�̎��
	 * ��caution : Shader�𑝌������ꍇ�͉��̃}�N���̒l���ύX����
	 */
	enum class ShaderAsset3D : int
	{
		SampleShader,
		Diffuse,
		Phong,
		Metal,
		Toon,
		ReflectionMapping,
		RefractionMapping,
		SingleColor,

		End
	};

	/*
	 * brief : ���f���̕`��ȊO(�|�X�g�v���Z�X�Ȃ�)�Ɏg�p����V�F�[�_�[�̎��
	 */
	enum class LocalShaderAsset
	{
		Collision,

		End
	};

	enum class ShaderAsset2D
	{
		Standard,

		End
	};
} // cumulonimbus::mapping::shader_assets

#endif // __cplusplus

/*
 * brief : GBuffer��ShaderSlot�p�}�N��
 *		   enum class ShaderAsset3D�ɍ��킹��
 */
#define ShaderSlot_Standard				1
#define ShaderSlot_SampleShader			2
#define ShaderSlot_Diffuse				3
#define ShaderSlot_Phong				4
#define ShaderSlot_Metal				5
#define ShaderSlot_Toon					6
#define ShaderSlot_ReflectionMapping	7
#define ShaderSlot_RefractionMapping	8
#define ShaderSlot_SingleColor			9

#endif // SHADER_ASSET_MAPPING_H