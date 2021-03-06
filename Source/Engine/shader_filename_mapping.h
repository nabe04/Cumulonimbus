#pragma once
#include <string>

namespace cumulonimbus::mapping::shader_filename
{
	namespace vs
	{
		inline std::string Toon_VS()							{ return { "./Shader/cso/toon_vs.cso" }; }
		inline std::string Phong_VS()							{ return { "./Shader/cso/phong_vs.cso" }; }
		inline std::string Image_VS()							{ return { "./Shader/cso/image_vs.cso" }; }
		inline std::string Metal_VS()							{ return { "./Shader/cso/cook_torrance_vs.cso" }; }
		inline std::string SkyBox_VS()							{ return { "./Shader/cso/sky_box_vs.cso" }; }
		inline std::string Diffuse_VS()							{ return { "./Shader/cso/diffuse_vs.cso" }; }
		inline std::string GBuffer_VS()							{ return { "./Shader/cso/gbuffer_vs.cso" }; }
		inline std::string Sprite2D_VS()						{ return { "./Shader/cso/sprite2d_vs.cso" }; }
		inline std::string Billboard_VS()						{ return { "./Shader/cso/billboard_vs.cso" }; }
		inline std::string Standard3D_VS()						{ return { "./Shader/cso/3d_standard_vs.cso" }; }
		inline std::string SingleColor_VS()						{ return { "./Shader/cso/single_color_vs.cso" }; }
		inline std::string SampleShader_VS()					{ return { "./Shader/cso/sample_shader_vs.cso" }; }
		inline std::string DebugCollision_VS()					{ return { "./Shader/cso/debug_collision_vs.cso" }; }
		inline std::string DebugPrimitive_VS()					{ return { "./Shader/cso/debug_primitive_vs.cso" }; }
		inline std::string StandardSprite_VS()					{ return { "./Shader/cso/standard_sprite_vs.cso" }; }
		inline std::string DepthExtraction_VS()					{ return { "./Shader/cso/variance_shadow_depth_extraction_vs.cso" }; }
		inline std::string RefractionMapping_VS()				{ return { "./Shader/cso/refraction_mapping_vs.cso" }; }
		inline std::string ReflectionMapping_VS()				{ return { "./Shader/cso/reflection_mapping_vs.cso" }; }
		inline std::string NormalShadowDepthExtraction_VS()		{ return { "./Shader/cso/normal_shadow_depth_extraction_vs.cso" }; }
		inline std::string VarianceShadowDepthExtraction_VS()	{ return { "./Shader/cso/variance_shadow_depth_extraction_vs.cso" }; }
	} // vs
	namespace ps
	{
		inline std::string Toon_PS()							{ return { "./Shader/cso/toon_ps.cso" }; }
		inline std::string Phong_PS()							{ return { "./Shader/cso/phong_ps.cso" }; }
		inline std::string Metal_PS()							{ return { "./Shader/cso/cook_torrance_ps.cso" }; }
		inline std::string SkyBox_PS()							{ return "./Shader/cso/sky_box_ps.cso"; }
		inline std::string Diffuse_PS()							{ return { "./Shader/cso/diffuse_ps.cso" }; }
		inline std::string Distort_PS()							{ return { "./Shader/cso/distort.cso" }; }
		inline std::string GBuffer_PS()							{ return { "./Shader/cso/gbuffer_ps.cso" }; }
		inline std::string ToonMTR_PS()							{ return {"./Shader/cso/toon_mtr_ps.cso"}; }
		inline std::string MetalMTR_PS()						{ return { "./Shader/cso/metal_mtr_ps.cso" }; }
		inline std::string PhongMTR_PS()						{ return { "./Shader/cso/phong_mtr_ps.cso" }; }
		inline std::string Sprite2D_PS()						{ return { "./Shader/cso/sprite2d_ps.cso" }; }
		inline std::string Billboard_PS()						{ return { "./Shader/cso/billboard_ps.cso" }; }
		inline std::string DiffuseMRT_PS()						{ return { "./Shader/cso/diffuse_mrt_ps.cso" }; }
		inline std::string Monochrome_PS()						{ return { "./Shader/cso/monochrome_ps.cso" }; }
		inline std::string Standard3D_PS()						{ return { "./Shader/cso/3d_standard_ps.cso" }; }
		inline std::string SingleColor_PS()						{ return { "./Shader/cso/single_color_ps.cso" }; }
		inline std::string SampleShader_PS()					{ return { "./Shader/cso/sample_shader_ps.cso" }; }
		inline std::string ScreenFilter_PS()					{ return { "./Shader/cso/screen_filter.cso" }; }
		inline std::string DebugCollision_PS()					{ return { "./Shader/cso/debug_collision_ps.cso" }; }
		inline std::string DebugPrimitive_PS()					{ return { "./Shader/cso/debug_primitive_ps.cso" }; }
		inline std::string GlowExtraction_PS()					{ return { "./Shader/cso/glow_extraction_ps.cso" }; }
		inline std::string LensDistortion_PS()					{ return { "./Shader/cso/lens_distortion.cso" }; }
		inline std::string StandardSprite_PS()					{ return { "./Shader/cso/standard_sprite_ps.cso" }; }
		inline std::string SingleColorMTR_PS()					{ return { "./Shader/cso/single_color_mtr_ps.cso" }; }
		inline std::string SampleShaderMTR_PS()					{ return { "./Shader/cso/sample_shader_mtr_ps.cso" }; }
		inline std::string DepthExtraction_PS()					{ return { "./Shader/cso/variance_shadow_depth_extraction_ps.cso" }; }
		inline std::string GBufferLighting_PS()					{ return { "./Shader/cso/gbuffer_lighting_ps.cso" }; }
		inline std::string OutlineExtraction_PS()				{ return { "./Shader/cso/outline_extraction_ps.cso" }; }
		inline std::string ReflectionMapping_PS()				{ return { "./Shader/cso/reflection_mapping_ps.cso" }; }
		inline std::string RefractionMapping_PS()				{ return { "./Shader/cso/refraction_mapping_ps.cso" }; }
		inline std::string ReflectionMappingMTR_PS()			{ return { "./Shader/cso/reflection_mapping_mtr_ps.cso" }; }
		inline std::string RefractionMappingMTR_PS()			{ return { "./Shader/cso/refraction_mapping_mtr_ps.cso" }; }
		inline std::string GaussianBlurVertical_PS()			{ return { "./Shader/cso/gaussian_blur_vertical_ps.cso" }; }
		inline std::string GaussianBlurHorizontal_PS()			{ return { "./Shader/cso/gaussian_blur_horizontal_ps.cso" }; }
		inline std::string SorbelFilterConvolution_PS()			{ return { "./Shader/cso/sorbel_filter_convolution_ps.cso" }; }
		inline std::string GaussianBlurConvolution_PS()			{ return { "./Shader/cso/gaussian_blur_convolution_ps.cso" }; }
		inline std::string NormalShadowDepthExtraction_PS()		{ return { "./Shader/cso/normal_shadow_depth_extraction_ps.cso" }; }
		inline std::string VarianceShadowDepthExtraction_PS()	{ return { "./Shader/cso/variance_shadow_depth_extraction_ps.cso" }; }
	} // ps
} // cumulonimbus::mapping::shader_filename