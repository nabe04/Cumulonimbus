#pragma once

#include <d3d11.h>
#include <wrl.h>

#include <array>

enum  RenderingSampleState
{
	Linear_Border,
	Point_Wrap,
	Linear_Wrap,
	Anistropic,

	End,
};

enum class SpriteSamplerState
{
	Linear_Border,

	End,
};

enum class SamplerState
{
	Linear_Border,
	Point_Wrap,
	Linear_Wrap,
	Anistropic,

	End,
};

class Sampler final
{
public:
	explicit Sampler() = default; // for cereal
	explicit Sampler(
		ID3D11Device* device,
		D3D11_FILTER filter = D3D11_FILTER_ANISOTROPIC,
		D3D11_TEXTURE_ADDRESS_MODE address_mode = D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_COMPARISON_FUNC comparison_func = D3D11_COMPARISON_NEVER,
		float r = 1, float g = 1, float b = 1, float a = 1);
	~Sampler() = default;

	/**
	 * @brief : custom_sampler_state�̃o�C���h
	 */
	void Activate(ID3D11DeviceContext* immediate_context, unsigned int slot, bool set_in_vs = false);
	/**
	 * @brief : default_sampler_states�̃o�C���h
	 */
	void Activate(ID3D11DeviceContext* immediate_context, SamplerState state, unsigned int slot, bool set_in_vs = false);
	/**
	 * @brief : Sampler State�̃A���o�C���h
	 */
	void Deactivate(ID3D11DeviceContext* immediate_context);

private:
	unsigned int using_slot = 0;
	// SamplerState�쐬���Ɉ����Ŏw�肵�č쐬����
	Microsoft::WRL::ComPtr<ID3D11SamplerState> custom_sampler_state;
	// �f�t�H���g�̃T���v���[�S
	std::array<Microsoft::WRL::ComPtr<ID3D11SamplerState>, static_cast<u_int>(SamplerState::End)> default_sampler_states;
	// SamplerState�̃Z�b�g���ɃZ�b�g����O�ɕێ����Ă���Sampler�i�[�ꏊ
	std::array<Microsoft::WRL::ComPtr<ID3D11SamplerState>, 2> save_sampler_states;

	/**
	 * @brief : SamplerState�̍쐬
	 */
	void CreateSamplerState(
		ID3D11SamplerState** sampler,
		ID3D11Device* device, D3D11_FILTER filter	= D3D11_FILTER_ANISOTROPIC,
		D3D11_TEXTURE_ADDRESS_MODE address_mode		= D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_COMPARISON_FUNC comparison_func		= D3D11_COMPARISON_NEVER,
		float r = 1, float g = 1, float b = 1, float a = 1);
};