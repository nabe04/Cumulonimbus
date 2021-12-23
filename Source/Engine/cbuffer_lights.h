#ifndef CBUFFER_LIGHTS_H
#define CBUFFER_LIGHTS_H

#ifdef __cplusplus
#include <cereal/cereal.hpp>
#include "cereal_helper.h"
#endif // __cplusplus

#include "shader_interop.h"

const int MAX_POINT_LIGHT = 500;
const int MAX_SPOT_LIGHT  = 500;

// Directional Lightのコンスタントバッファ
CBUFFER(DirectionalLightCB, CBSlot_DirectionalLight)
{
	float3 d_light_direction;
	float  d_light_intensity;
	float4 d_light_color;

#ifdef __cplusplus
	template<class Archive>
	void load(Archive&& archive,const uint32_t version)
	{
		archive(
			CEREAL_NVP(d_light_direction),
			CEREAL_NVP(d_light_intensity),
			CEREAL_NVP(d_light_color)
		);
	}

	template<class Archive>
	void save(Archive&& archive,const uint32_t version) const
	{
		archive(
			CEREAL_NVP(d_light_direction),
			CEREAL_NVP(d_light_intensity),
			CEREAL_NVP(d_light_color)
		);
	}
#endif // __cplusplus

};

#ifdef __cplusplus
CEREAL_CLASS_VERSION(DirectionalLightCB, 0)
#endif // __cplusplus

#endif // CBUFFER_LIGHTS_H


