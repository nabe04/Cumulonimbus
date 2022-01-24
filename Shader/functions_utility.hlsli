#ifndef FUNCTIONS_UTILITY_SHF
#define FUNCTIONS_UTILITY_SHF

#include "macros.hlsli"

/**
 * @brief : GBuffer���PS_Output::shader_slot�̐F���̌v�Z
 *         R8_UNORM�e�N�X�`���Ȃ̂ŃX���b�g�ԍ���0�`1��
 *         256�K���ɕϊ�����
 * @param  slot  : CB_ShaderSlot::shader_slot�̒l
 */
inline float CalcGBuffShaderSlotVal(int slot)
{
	// 0.00392156f : 1 / 255
    return float(slot) * 0.00392156f;
}

/**
 * @brief           : �K���}�␳�̌v�Z
 * @param color     : �K���}�␳�����������F(�e�N�X�`��)
 * @param gamma_val : �␳�����������ݏ�l
 */
inline float3 CalcGammaCorrection(float3 color,float gamma_val)
{
    return pow(abs(color), 1.0f / gamma_val);
}

/**
 * @brier               : �f�K���}�␳�̌v�Z
 * @param color         : �K���}�␳�����������F(�e�N�X�`��)
 * @param de_gamma_val  : �␳�����������ݏ�l
 */
inline float3 CalcDeGammaCorrection(float3 color,float de_gamma_val)
{
    return pow(abs(color), de_gamma_val);
}

/**
 * @brief : float��epsilon�̒l���l���������l�֐�
 */
inline bool IsEqual(float l_value,float r_value)
{
    const int l_val = floor(l_value);
    const int r_val = floor(r_value);

	// epsilon == 1.192092896e-07F
    return (l_val - r_val) <= 1.192092896e-07F ? true : false;
}

/**
 * @brief : �����_���Ȓl�̎擾
 */
inline float Random(float2 val)
{
    val = float2(dot(val, float2(127.1, 311.7)),
                 dot(val, float2(269.5, 183.3)));
    return -1.0 + 2.0 * frac(sin(val) * 43758.5453123);
}

/**
 * @brief : �p�[�����m�C�Y�K�p
 */
inline float PerlinNoise(const float2 scroll_val, const float2 st)
{
    const float2 p = floor(st);
    const float2 f = frac(st);
    const float2 u = f * f * (3.0 - 2.0 * f);

    //const float v00 = Random(scroll_val + p + float2(0, 0));
    //const float v10 = Random(scroll_val + p + float2(1, 0));
    //const float v01 = Random(scroll_val + p + float2(0, 1));
    //const float v11 = Random(scroll_val + p + float2(1, 1));

    const float v00 = Random( p + float2(0, 0));
    const float v10 = Random( p + float2(1, 0));
    const float v01 = Random( p + float2(0, 1));
    const float v11 = Random( p + float2(1, 1));

    return lerp(lerp(dot(v00, f - float2(0, 0)), dot(v10, f - float2(1, 0)), u.x),
                             lerp(dot(v01, f - float2(0, 1)), dot(v11, f - float2(1, 1)), u.x),
                             u.y) + 0.5f;
}

/**
 * @brief : ������0�̃x�N�g�����l���������K��
 */
inline float3 SafeNormalize(const float3 in_vec)
{
    const float dp3 = max(FLT_MIN, dot(in_vec, in_vec));
    return in_vec * rsqrt(dp3);
}

/**
 * @brief      : �ڋ��(�^���W�F���g�X�y�[�X)�ƃm�[�}���}�b�v����@�����Z�o
 * @param tangent : �^���W�F���g(��)�x�N�g��
 * @param binormal : �o�C�m�[�}��(�]��)�x�N�g��
 * @param normal : �m�[�}��(�@��)�x�N�g��
 * @param normal_map : �m�[�}���}�b�v�e�N�X�`��
 */
inline float3 CalcNormalFromTangentSpace(const float3 tangent,const float3 binormal,const float3 normal,float3 normal_map)
{
	// �ڋ�Ԏ�
    const float3 vx = normalize(tangent);
    const float3 vy = normalize(binormal);
    const float3 vz = normalize(normal);

	// �ڋ�ԍs��
    const float3x3 mat = { { vx }, { vy }, { vz } };
    normal_map = normal_map * 2.0f - 1.0f;

	// �@���}�b�v���烏�[���h�֕ϊ�
    return normal_map = normalize(mul(normal_map, mat));
}

/**
 * @brief : use_channel���1�ȏ�̒l�̃e�N�X�`���`�����l���l���擾����B
 *          ���ׂẴ`�����l����1�����̏ꍇspare_param(��5����)���g�p�����
 * @param use_channel : �g�p�������`�����l��(�g�p������������1�ȏ���i�[����)
 * @param tex : �擾����e�N�X�`��
 * @param ss : �T���v���[
 * @param uv_pos : UV���W
 * @param spare_param : use_channel�̒l�����ׂ�1�����̏ꍇ�g�p�����l
 */
inline float GetSingleTextureChannel(
    const float4 use_channel, const Texture2D tex,
    const SamplerState ss,const float2 uv_pos,
    const float spare_param)
{
    const float4 col = tex.Sample(ss, uv_pos);
	// �Ԃ��F(�������^�[���������ꍇ�x�����o������)
    float ret_col = 0.0f;

    if (use_channel.x >= 1.0f)
        ret_col = col.r;
    else if (use_channel.y >= 1.0f)
        ret_col = col.g;
    else if (use_channel.z >= 1.0f)
        ret_col = col.b;
    else if(use_channel.w >= 1.0f)
        ret_col = col.a;
	else
        ret_col = spare_param;

    return ret_col;
}

/**
 * @return : 1 - �����l
 */
inline float OneMinus(const float val)
{
    return 1.0f - val;
}

/**
 * @brief : RGB�J���[���O���[�X�P�[���ɕϊ�
 */
inline float ToGrayColor(const float3 color)
{
    return dot(color, float3(0.3f, 0.59f, 0.11f));
}

/**
 * @brief : UV���W��NDC���W�ɕϊ�
 */
float2 UvToNDC(float2 uv)
{
    uv.y = -uv.y;
    float2 pos_xy = uv.xy * 2.0f - float2(1.0f, -1.0f);

    return pos_xy;
}

/**
 * @brief : �[�x�e�N�X�`�����烏�[���h��ԏ�̍��W���Z�o
 * @param texcoord : �e�X�N�`�����W
 * @param depth : �[�x�l
 * @param inv_view_projection : �r���[�E�v���W�F�N�V�����s��̋t�s��
 */
inline float3 DepthToWPosition(
	float2 texcoord,
	const float depth,
	const float4x4 inv_view_projection)
{
	// �[�x�l����v���W�F�N�V�������W�ł̈ʒu���Z�o
    texcoord.y = -texcoord.y;
    float2 pos_xy = texcoord.xy * 2.0f - float2(1.0f, -1.0f);
    const float4 projected_position = float4(pos_xy, depth, 1.0f);

    const float4 position = mul(inv_view_projection, projected_position);

    return position.xyz / position.w;
}

#endif // FUNCTIONS_UTILITY_SHF