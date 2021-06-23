#ifndef SHADER_INTEROP_H
#define SHADER_INTEROP_H

#include "texture_resource_mapping.h"
#include "sampler_mapping.h"
#include "constant_buffer_mapping.h"

#ifdef __cplusplus
// Application-side types:
#include <DirectXMath.h>

typedef DirectX::XMMATRIX   matrix;
typedef DirectX::XMFLOAT4X4 float4x4;
typedef DirectX::XMFLOAT2	float2;
typedef DirectX::XMFLOAT3	float3;
typedef DirectX::XMFLOAT4	float4;
typedef uint32_t			uint;
typedef DirectX::XMUINT2	uint2;
typedef DirectX::XMUINT3	uint3;
typedef DirectX::XMUINT4	uint4;
typedef DirectX::XMINT2		int2;
typedef DirectX::XMINT3		int3;
typedef DirectX::XMINT4		int4;

// ïœêîñº
#define CG_GETBINDSLOT(name) __CBUFFERBINDSLOT__##name##__
#define CBUFFER(name, slot) \
	static const int CG_GETBINDSLOT(name) = slot; \
	struct alignas(16) name
#define CONSTANTBUFFER(name, type, slot) CBUFFER(name, slot)

#else

// Shader - side types:

#define CBUFFER(name, slot)					cbuffer name : register(b ## slot)
#define CONSTANTBUFFER(name, type, slot)	CONSTANTBUFFER< type > name : register(b ## slot)

#define TEXTURE1D(name, type, slot)			Texture1D< type > name : register(t ## slot)
#define TEXTURE1DARRAY(name, type, slot)	Texture1DArray< type > name : register(t ## slot)
#define RWTEXTURE1D(name, type, slot)		RWTexture1D< type > name : register(u ## slot)

#define TEXTURE2D(name, type, slot)			Texture2D< type > name : register(t ## slot)
#define TEXTURE2DMS(name, type, slot)		Texture2DMS< type > name : register(t ## slot)
#define TEXTURE2DARRAY(name, type, slot)	Texture2DArray< type > name : register(t ## slot)
#define RWTEXTURE2D(name, type, slot)		RWTexture2D< type > name : register(u ## slot)
#define RWTEXTURE2DARRAY(name, type, slot)	RWTexture2DArray< type > name : register(u ## slot)
#define ROVTEXTURE2D(name, type, slot)		RasterizerOrderedTexture2D< type > name : register(u ## slot)

#define TEXTURECUBE(name, type, slot)		TextureCube< type > name : register(t ## slot)
#define TEXTURECUBEARRAY(name, type, slot)	TextureCubeArray< type > name : register(t ## slot)

#define TEXTURE3D(name, type, slot)			Texture3D< type > name : register(t ## slot)
#define RWTEXTURE3D(name, type, slot)		RWTexture3D< type > name : register(u ## slot)
#define ROVTEXTURE3D(name, type, slot)		RasterizerOrderedTexture3D< type > name : register(u ## slot)


#define SAMPLERSTATE(name, slot)			SamplerState name : register(s ## slot)
#define SAMPLERCOMPARISONSTATE(name, slot)	SamplerComparisonState name : register(s ## slot)


#endif // __cplusplus


#endif //SHADER_INTEROP_H