#include "globals.hlsli"
#include "functions_utility.hlsli"
#include "../Source/Engine/cbuffer_post_effect.h"

SamplerState default_sampler : register(s0);

float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}