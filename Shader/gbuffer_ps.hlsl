// PS_Input(VS_Output)
#define PIN_USE_W_POSITION
#define PIN_USE_NORMAL
#define PIN_USE_TANGENT
#define PIN_USE_BINORMAL
#define PIN_USE_TEXCOORD0

#include "globals.hlsli"

SamplerState default_sampler : register(s0);

PS_Output main(PS_Input pin)
{
    PS_Output pout = (PS_Output) 0;

	// �ڋ�Ԏ�
    const float3 vx = normalize(pin.tangent);
    const float3 vy = normalize(pin.binorma);
    const float3 vz = normalize(pin.normal);

	// �ڋ�ԍs��
    const float3x3 v_mat = { { vx }, { vy }, { vz } };

    float3 normal = texture_normal.Sample(default_sampler, pin.texcoord0).xyz;
    normal = normal * 2.0f - 1.0f;

    // �@���}�b�v���烏�[���h�֕ϊ�
    normal = normalize(mul(normal, v_mat));

    const float4 color    = texture_base_color.Sample(default_sampler, pin.texcoord0);
    const float4 position = pin.w_position;

    pout.albedo_color   = color;
    //pout.albedo_color   = float4(1, 1, 1, 1);
    pout.position       = position;
    pout.normal         = float4(normal, 1.0f);

	return pout;
}