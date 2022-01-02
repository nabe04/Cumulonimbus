#ifndef CBUFFER_LIGHTS_H
#define CBUFFER_LIGHTS_H

#ifdef __cplusplus
#include <cereal/cereal.hpp>
#include "cereal_helper.h"
#endif // __cplusplus

#include "shader_interop.h"

static const int MAX_POINT_LIGHT = 500;
static const int MAX_SPOT_LIGHT  = 500;

///////////////////////////////////////////////////
//			Directional Light					 //
///////////////////////////////////////////////////

// Directional Lightコンスタントバッファ
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

///////////////////////////////////////////////////
//					Point Light					 //
///////////////////////////////////////////////////

// Point Lightパラメータ
struct PointLight
{
	float3 p_light_position;
	float  p_light_intensity;
	float3 p_light_color;
	float  p_light_range;

#ifdef __cplusplus
	template<class Archive>
	void load(Archive&& archive, const uint32_t version)
	{
		archive(
			CEREAL_NVP(p_light_position),
			CEREAL_NVP(p_light_intensity),
			CEREAL_NVP(p_light_color),
			CEREAL_NVP(p_light_range)
		);
	}

	template<class Archive>
	void save(Archive&& archive, const uint32_t version) const
	{
		archive(
			CEREAL_NVP(p_light_position),
			CEREAL_NVP(p_light_intensity),
			CEREAL_NVP(p_light_color),
			CEREAL_NVP(p_light_range)
		);
	}
#endif // __cplusplus
};

#ifdef __cplusplus
CEREAL_CLASS_VERSION(PointLight, 0)
#endif // __cplusplus

// Point Light コンスタントバッファ
CBUFFER(PointLightsCB, CBSlot_PointLight)
{
	PointLight point_lights[MAX_POINT_LIGHT];
};

///////////////////////////////////////////////////
//					Spot Light					 //
///////////////////////////////////////////////////

// Spot Lightパラメータ
struct SpotLight
{
	float3 s_light_direction;
	float  s_light_intensity;
	float3 s_light_position;
	float  s_light_range;
	float4 s_light_color;
	float  s_light_inner_cone;
	float  s_light_outer_cone;
	float2 s_light_dummy;

#ifdef __cplusplus
	template<class Archive>
	void load(Archive&& archive, const uint32_t version)
	{
		archive(
			CEREAL_NVP(s_light_direction),
			CEREAL_NVP(s_light_intensity),
			CEREAL_NVP(s_light_position),
			CEREAL_NVP(s_light_range),
			CEREAL_NVP(s_light_color),
			CEREAL_NVP(s_light_inner_cone),
			CEREAL_NVP(s_light_outer_cone)
		);
	}

	template<class Archive>
	void save(Archive&& archive, const uint32_t version) const
	{
		archive(
			CEREAL_NVP(s_light_direction),
			CEREAL_NVP(s_light_intensity),
			CEREAL_NVP(s_light_position),
			CEREAL_NVP(s_light_range),
			CEREAL_NVP(s_light_color),
			CEREAL_NVP(s_light_inner_cone),
			CEREAL_NVP(s_light_outer_cone)
		);
	}
#endif // __cplusplus
};

#ifdef __cplusplus
CEREAL_CLASS_VERSION(SpotLight, 0)
#endif // __cplusplus

CBUFFER(SpotLightsCB, CBSlot_SpotLight)
{
	SpotLight spot_lights[MAX_SPOT_LIGHT];
};

/////////////////////////////////////////////////////////
//			 Point & Spot Light information			   //
/////////////////////////////////////////////////////////
CBUFFER(LightInfoCB, CBSlot_LightInfo)
{
	uint p_light_count;	// シーン内に存在するポイントライト数
	uint s_light_count; // シーン内の存在するスポットライト数
	float2 light_info_dummy;
};


#endif // CBUFFER_LIGHTS_H
