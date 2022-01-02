#pragma once
#include <array>

#include "cbuffer_lights.h"
#include "constant_buffer.h"

namespace cumulonimbus::light
{
	class LightManager final
	{
	public:
		explicit LightManager(ID3D11Device* device);

		void SetDirectionalLight(const DirectionalLightCB& directional_light);
		void SetPointLight(const PointLight& point_light, u_int index);
		void SetSpotLight(const SpotLight& spot_light, u_int index);
		void SetLightInfo(const LightInfoCB& light_info);
		void SetNumPointLights(uint num_lights);
		void SetNumSpotLights(uint num_lights);

		void BindCBuffers(ID3D11DeviceContext* immediate_context);
		void UnbindCBuffers(ID3D11DeviceContext* immediate_context) const;

		//-- �R���X�^���g�o�b�t�@�̃o�C���h & �A���o�C���h --//
		/**
		 * @brief : �R���X�^���g�o�b�t�@(�f�B���N�V���i�����C�g)�̃o�C���h
		 */
		void BindDirectionalLightCBuffer(ID3D11DeviceContext* immediate_context);
		/**
		 * @brief : �R���X�^���g�o�b�t�@(�f�B���N�V���i�����C�g)�̃A���o�C���h
		 */
		void UnbindDirectionalLightCBuffer(ID3D11DeviceContext* immediate_context) const;
		/**
		 * @brief : �R���X�^���g�o�b�t�@(�|�C���g���C�g)�̃o�C���h
		 */
		void BindPointLightCBuffer(ID3D11DeviceContext* immediate_context);
		/**
		 * @brief : �R���X�^���g�o�b�t�@(�|�C���g���C�g)�̃A���o�C���h
		 */
		void UnbindPointLightCBuffer(ID3D11DeviceContext* immediate_context) const;
		/**
		 * @brief : �R���X�^���g�o�b�t�@(�X�|�b�g���C�g)�̃o�C���h
		 */
		void BindSpotLightCBuffer(ID3D11DeviceContext* immediate_context);
		/**
		 * @brief : �R���X�^���g�o�b�t�@(�X�|�b�g���C�g)�̃A���o�C���h
		 */
		void UnbindSpotLightCBuffer(ID3D11DeviceContext* immediate_context) const;
		/**
		 * @brief : �R���X�^���g�o�b�t�@(���C�g���)�̃o�C���h
		 */
		void BindLightInfoCBuffer(ID3D11DeviceContext* immediate_context);
		/**
		 * @brief : �R���X�^���g�o�b�t�@(���C�g���)�̃A���o�C���h
		 */
		void UnbindLightInfoCBuffer(ID3D11DeviceContext* immediate_context) const;

	private:
		//-- �R���X�^���g�o�b�t�@�p�����[�^�[ --//
		// �f�B���N�V���i�����C�g
		DirectionalLightCB cb_directional_light{};
		// �|�C���g���C�g(�ő�500��)
		buffer::ConstantBuffer<PointLightsCB> cb_point_lights{};
		// �X�|�b�g���C�g(�ő�500��)
		buffer::ConstantBuffer<SpotLightsCB> cb_spot_lights{};
		// ���C�g���
		buffer::ConstantBuffer<LightInfoCB> cb_light_info{};

		//-- �o�b�t�@ --//
		// �f�B���N�V���i�����C�g
		Microsoft::WRL::ComPtr<ID3D11Buffer> directional_light_buffer{};
		//// �|�C���g���C�g
		//Microsoft::WRL::ComPtr<ID3D11Buffer> point_light_buffer{};
		//// �X�|�b�g���C�g
		//Microsoft::WRL::ComPtr<ID3D11Buffer> spot_light_buffer{};

		void CreateCBuffer(ID3D11Device* device);
	};
} // cumulonimbus::light
