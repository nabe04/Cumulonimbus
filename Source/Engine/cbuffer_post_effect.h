#ifndef CBUFFER_POST_EFFECT
#define CBUFFER_POST_EFFECT

#ifdef __cplusplus
#include <cereal/cereal.hpp>
#include "cereal_helper.h"
#endif // __cplusplus

#include "shader_interop.h"

// Distort�R���X�^���g�o�b�t�@
CBUFFER(DistortCB, CBSlot_Distort)
{
	float  distort_radius;			 // �ڂ������s��Ȃ����a
	float  distort_elapsed_time;	 // �o�ߎ���
	float  distort_time_scale;		 // uv�X�N���[�����̃X�P�[��
	float  distort_noise_scale;		 // �m�C�Y�X�P�[��
	float  distort_noise_attenuation;		 //
	float2 distort_scroll_direction; // uv�X�N���[������
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

#ifdef __cplusplus
	template<class Archive>
	void load(Archive && archive, const uint32_t version)
	{
		archive(
			CEREAL_NVP(sfilter_filter_color),
			CEREAL_NVP(sfilter_is_grey_scale)
		);
	}

	template<class Archive>
	void save(Archive&& archive, const uint32_t version) const
	{
		archive(
			CEREAL_NVP(sfilter_filter_color),
			CEREAL_NVP(sfilter_is_grey_scale)
		);
	}
#endif // __cplusplus
};

#ifdef __cplusplus
CEREAL_CLASS_VERSION(ScreenFilterCB, 0)
#endif // __cplusplus

#endif // CBUFFER_POST_EFFECT
