#ifndef CBUFFER_SPRITE_H
#define CBUFFER_SPRITE_H

#ifdef __cplusplus
#include <cereal/cereal.hpp>
#include "cereal_helper.h"
#endif // __cplusplus

#include "shader_interop.h"

// Spriteコンスタントバッファ
CBUFFER(SpriteCB, CBSlot_Sprite)
{
	float2 sprite_tiling;
	float2 sprite_offset;

#ifdef __cplusplus
	template<class Archive>
	void load(Archive && archive, const uint32_t version)
	{
		archive(
			CEREAL_NVP(sprite_tiling),
			CEREAL_NVP(sprite_offset)
		);
	}

	template<class Archive>
	void save(Archive&& archive, const uint32_t version) const
	{
		archive(
			CEREAL_NVP(sprite_tiling),
			CEREAL_NVP(sprite_offset)
		);
	}
#endif // __cplusplus
};

#ifdef __cplusplus
CEREAL_CLASS_VERSION(SpriteCB, 0)
#endif // __cplusplus

#endif // CBUFFER_SPRITE_H

