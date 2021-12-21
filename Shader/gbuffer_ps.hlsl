// PS_Input(VS_Output)
#define PIN_USE_W_POSITION
#define PIN_USE_NORMAL
#define PIN_USE_TANGENT
#define PIN_USE_BINORMAL
#define PIN_USE_TEXCOORD0

#include "globals.hlsli"
#include "functions.hlsli"
#include "functions_utility.hlsli"
#include "../Source/Engine/material.h"

SamplerState default_sampler : register(s0);

PS_Output main(PS_Input pin)
{
    PS_Output pout = (PS_Output) 0;

	// �ڋ�Ԏ�
    const float3 vx = normalize(pin.tangent);
    const float3 vy = normalize(pin.binormal);
    const float3 vz = normalize(pin.normal);

	// �ڋ�ԍs��
    const float3x3 tbn_mat = { { vx }, { vy }, { vz } };

    float3 normal = texture_normal.Sample(default_sampler, pin.texcoord0).xyz;
    normal = normal * 2.0f - 1.0f;

    // �@���}�b�v���烏�[���h�֕ϊ�
    normal = normalize(mul(normal, tbn_mat));

    const float4 color    = texture_base_color.Sample(default_sampler, pin.texcoord0);
    const float4 position = pin.w_position;

	// ���^���l�X�l�̎擾
    const float metalness = GetSingleTextureChannel(mat_use_metalness_channel, texture_metalness, default_sampler, pin.texcoord0, 0.5f);
	// ���t�l�X�l�̎擾
    const float roughness = GetSingleTextureChannel(mat_use_roughness_channel, texture_roughness, default_sampler, pin.texcoord0, 0.5f);
    // �I�N�����[�W�����l�̎擾 mat_use_occlusion_channel
    const float occlusion = GetSingleTextureChannel(mat_use_occlusion_channel, texture_occlusion, default_sampler, pin.texcoord0, 0.5f);

    pout.color   = color;
    pout.normal  = float4(normal, 1.0f);
    pout.mro     = float4(metalness, roughness, occlusion, 1.0f);

    return pout;
}