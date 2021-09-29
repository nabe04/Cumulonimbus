#ifndef SHADER_ASSET_MAPPING_H
#define SHADER_ASSET_MAPPING_H

#ifdef __cplusplus

namespace cumulonimbus::mapping::shader_assets
{
	/*
	 * brief	 : モデルを描画する際に使用するシェーダーの種類
	 * ※caution : Shaderを増減した場合は下のマクロの値も変更する
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
	 * brief : モデルの描画以外(ポストプロセスなど)に使用するシェーダーの種類
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
 * brief : GBufferのShaderSlot用マクロ
 *		   enum class ShaderAsset3Dに合わせる
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