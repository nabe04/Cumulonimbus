#ifndef SHADER_ASSET_MAPPING_H
#define SHADER_ASSET_MAPPING_H

#ifdef __cplusplus

namespace cumulonimbus::mapping::shader_assets
{
	enum class ShaderAsset3D : int
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
#define ShaderSlot_Standard				static_cast<int>(cumulonimbus::mapping::shader_assets::ShaderAsset3D::Standard)				+ 1
#define ShaderSlot_SampleShader			static_cast<int>(cumulonimbus::mapping::shader_assets::ShaderAsset3D::SampleShader)			+ 1
#define ShaderSlot_Diffuse				static_cast<int>(cumulonimbus::mapping::shader_assets::ShaderAsset3D::Diffuse)				+ 1
#define ShaderSlot_Phong				static_cast<int>(cumulonimbus::mapping::shader_assets::ShaderAsset3D::Phong)				+ 1
#define ShaderSlot_Metal				static_cast<int>(cumulonimbus::mapping::shader_assets::ShaderAsset3D::Metal)				+ 1
#define ShaderSlot_Toon					static_cast<int>(cumulonimbus::mapping::shader_assets::ShaderAsset3D::Toon)					+ 1
#define ShaderSlot_ReflectionMapping	static_cast<int>(cumulonimbus::mapping::shader_assets::ShaderAsset3D::ReflectionMapping)	+ 1
#define ShaderSlot_RefractionMapping	static_cast<int>(cumulonimbus::mapping::shader_assets::ShaderAsset3D::RefractionMapping)	+ 1
#define ShaderSlot_SingleColor			static_cast<int>(cumulonimbus::mapping::shader_assets::ShaderAsset3D::SingleColor)			+ 1

#endif // SHADER_ASSET_MAPPING_H