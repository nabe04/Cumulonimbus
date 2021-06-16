#pragma once
#include <string>

namespace cumulonimbus::mapping::shader_filename
{
	inline std::string DepthExtraction_VS() { return { "./Shader/cso/variance_shadow_depth_extraction_vs.cso" }; }
	inline std::string DepthExtraction_PS() { return { "./Shader/cso/variance_shadow_depth_extraction_ps.cso" }; }
}