#ifndef SHADER_ASSET_MAPPING_H
#define SHADER_ASSET_MAPPING_H

#ifdef __cplusplus

namespace cumulonimbus::mapping::shader_assets
{
	enum class ShaderAsset3D
	{
		Standard,
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

	enum class ShaderAsset2D
	{
		Standard,

		End
	};
} // cumulonimbus::mapping::shader_assets

#endif // __cplusplus

/*
 * brief : GBufferÇÃShaderSlotópÉ}ÉNÉç
 *		   enum class ShaderAsset3DÇ…çáÇÌÇπÇÈ
 */ 
#define ShaderSlot_Standard				static_cast<int>(cumulonimbus::mapping::shader_assets::ShaderAsset3D::Standard)
#define ShaderSlot_SampleShader			static_cast<int>(cumulonimbus::mapping::shader_assets::ShaderAsset3D::SampleShader)
#define ShaderSlot_Diffuse				static_cast<int>(cumulonimbus::mapping::shader_assets::ShaderAsset3D::Diffuse)
#define ShaderSlot_Phong				static_cast<int>(cumulonimbus::mapping::shader_assets::ShaderAsset3D::Phong)
#define ShaderSlot_Metal				static_cast<int>(cumulonimbus::mapping::shader_assets::ShaderAsset3D::Metal)
#define ShaderSlot_Toon					static_cast<int>(cumulonimbus::mapping::shader_assets::ShaderAsset3D::Toon)
#define ShaderSlot_ReflectionMapping	static_cast<int>(cumulonimbus::mapping::shader_assets::ShaderAsset3D::ReflectionMapping)
#define ShaderSlot_RefractionMapping	static_cast<int>(cumulonimbus::mapping::shader_assets::ShaderAsset3D::RefractionMapping)
#define ShaderSlot_SingleColor			static_cast<int>(cumulonimbus::mapping::shader_assets::ShaderAsset3D::SingleColor)

#endif // SHADER_ASSET_MAPPING_H