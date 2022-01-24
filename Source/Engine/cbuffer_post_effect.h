#ifndef CBUFFER_POST_EFFECT
#define CBUFFER_POST_EFFECT

#ifdef __cplusplus
#include <cereal/cereal.hpp>
#include "cereal_helper.h"
#endif // __cplusplus

#include "shader_interop.h"

// Distortコンスタントバッファ
CBUFFER(DistortCB, CBSlot_Distort)
{
	float  distort_radius;			 // ぼかしを行わない半径
	float  distort_elapsed_time;	 // 経過時間
	float  distort_time_scale;		 // uvスクロール時のスケール
	float  distort_noise_scale;		 // ノイズスケール
	float  distort_noise_attenuation;		 //
	float2 distort_scroll_direction; // uvスクロール方向
	float  distort_padding0;

#ifdef __cplusplus
	template<class Archive>
	void load(Archive && archive, const uint32_t version)
	{
		archive(
			CEREAL_NVP(distort_radius),
			CEREAL_NVP(distort_elapsed_time),
			CEREAL_NVP(distort_time_scale),
			CEREAL_NVP(distort_noise_scale),
			CEREAL_NVP(distort_noise_attenuation),
			CEREAL_NVP(distort_scroll_direction)
		);
	}

	template<class Archive>
	void save(Archive&& archive, const uint32_t version) const
	{
		archive(
			CEREAL_NVP(distort_radius),
			CEREAL_NVP(distort_elapsed_time),
			CEREAL_NVP(distort_time_scale),
			CEREAL_NVP(distort_noise_scale),
			CEREAL_NVP(distort_noise_attenuation),
			CEREAL_NVP(distort_scroll_direction)
		);
	}
#endif // __cplusplus
};

#ifdef __cplusplus
CEREAL_CLASS_VERSION(DistortCB, 0)
#endif // __cplusplus

// Screen Filter
CBUFFER(ScreenFilterCB, CBSlot_ScreenFilter)
{
	float3 sfilter_filter_color;
	int sfilter_is_grey_scale;
	float sfilter_dissolve_threshold;

#ifdef __cplusplus
	template<class Archive>
	void load(Archive && archive, const uint32_t version)
	{
		if(version == 0)
		{
			archive(
				CEREAL_NVP(sfilter_filter_color),
				CEREAL_NVP(sfilter_is_grey_scale),
				CEREAL_NVP(sfilter_dissolve_threshold)
			);
		}
		if(version == 1)
		{
			archive(
				CEREAL_NVP(sfilter_filter_color),
				CEREAL_NVP(sfilter_is_grey_scale),
				CEREAL_NVP(sfilter_dissolve_threshold)
			);
		}
	}

	template<class Archive>
	void save(Archive&& archive, const uint32_t version) const
	{
		archive(
			CEREAL_NVP(sfilter_filter_color),
			CEREAL_NVP(sfilter_is_grey_scale),
			CEREAL_NVP(sfilter_dissolve_threshold)
		);
	}
#endif // __cplusplus
};

#ifdef __cplusplus
CEREAL_CLASS_VERSION(ScreenFilterCB, 1)
#endif // __cplusplus

CBUFFER(DissolveScreenCB, CBSlot_DissolveScreen)
{
	float dissolve_amount;
	float3 dissolve_padding;

#ifdef __cplusplus
	template<class Archive>
	void load(Archive && archive, const uint32_t version)
	{
		archive(
			CEREAL_NVP(dissolve_amount)
		);
	}

	template<class Archive>
	void save(Archive && archive, const uint32_t version) const
	{
		archive(
			CEREAL_NVP(dissolve_amount)
		);
	}
#endif // __cplusplus
};

#ifdef __cplusplus
CEREAL_CLASS_VERSION(DissolveScreenCB, 0)
#endif // __cplusplus

#endif // CBUFFER_POST_EFFECT
