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

		//-- コンスタントバッファのバインド & アンバインド --//
		/**
		 * @brief : コンスタントバッファ(ディレクショナルライト)のバインド
		 */
		void BindDirectionalLightCBuffer(ID3D11DeviceContext* immediate_context);
		/**
		 * @brief : コンスタントバッファ(ディレクショナルライト)のアンバインド
		 */
		void UnbindDirectionalLightCBuffer(ID3D11DeviceContext* immediate_context) const;
		/**
		 * @brief : コンスタントバッファ(ポイントライト)のバインド
		 */
		void BindPointLightCBuffer(ID3D11DeviceContext* immediate_context);
		/**
		 * @brief : コンスタントバッファ(ポイントライト)のアンバインド
		 */
		void UnbindPointLightCBuffer(ID3D11DeviceContext* immediate_context) const;
		/**
		 * @brief : コンスタントバッファ(スポットライト)のバインド
		 */
		void BindSpotLightCBuffer(ID3D11DeviceContext* immediate_context);
		/**
		 * @brief : コンスタントバッファ(スポットライト)のアンバインド
		 */
		void UnbindSpotLightCBuffer(ID3D11DeviceContext* immediate_context) const;
		/**
		 * @brief : コンスタントバッファ(ライト情報)のバインド
		 */
		void BindLightInfoCBuffer(ID3D11DeviceContext* immediate_context);
		/**
		 * @brief : コンスタントバッファ(ライト情報)のアンバインド
		 */
		void UnbindLightInfoCBuffer(ID3D11DeviceContext* immediate_context) const;

	private:
		//-- コンスタントバッファパラメーター --//
		// ディレクショナルライト
		DirectionalLightCB cb_directional_light{};
		// ポイントライト(最大500個)
		buffer::ConstantBuffer<PointLightsCB> cb_point_lights{};
		// スポットライト(最大500個)
		buffer::ConstantBuffer<SpotLightsCB> cb_spot_lights{};
		// ライト情報
		buffer::ConstantBuffer<LightInfoCB> cb_light_info{};

		//-- バッファ --//
		// ディレクショナルライト
		Microsoft::WRL::ComPtr<ID3D11Buffer> directional_light_buffer{};
		//// ポイントライト
		//Microsoft::WRL::ComPtr<ID3D11Buffer> point_light_buffer{};
		//// スポットライト
		//Microsoft::WRL::ComPtr<ID3D11Buffer> spot_light_buffer{};

		void CreateCBuffer(ID3D11Device* device);
	};
} // cumulonimbus::light
