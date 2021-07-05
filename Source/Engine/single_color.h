#ifndef SINGLE_COLOR_H
#define SINGLE_COLOR_H

#ifdef __cplusplus
#include <memory>

#include <DirectXMath.h>
#include <imgui.h>
#include <cereal/cereal.hpp>

#include "cereal_helper.h"
#include "constant_buffer.h"
#include "shader.h"
#include "shader_asset.h"
#endif // __cplusplus

#include "shader_interop_renderer.h"

CBUFFER(SingleColorCB, CBSlot_SingleColor)
{
	float4 single_color;

#ifdef __cplusplus
	template<typename Archive>
	void serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(single_color)
		);
	}
#endif // __cplusplus
};

#ifdef __cplusplus
namespace cumulonimbus
{
	namespace shader_system
	{
		class SingleColorShader final : public Shader
		{
		public:
			explicit SingleColorShader();
			~SingleColorShader() override = default;
		};
	}

	namespace shader_asset
	{
		class SingleColorAsset final : public ShaderAsset
		{
		public:
			explicit SingleColorAsset(u_int gbuff_shader_slot);
			explicit SingleColorAsset()  = default;
			~SingleColorAsset() override = default;
			
			void BindCBuffer()   override;
			void UnbindCBuffer() override;
			void RenderImGui()   override;

		private:
			std::unique_ptr<buffer::ConstantBuffer<SingleColorCB>> cb_single_color{ nullptr };
		};
	}
}



namespace shader
{
	struct M_SingleColor
	{
		DirectX::XMFLOAT4 single_color{};

		void EditParam()
		{
			ImGui::ColorEdit4("Color", (float*)&single_color);
		}

		template<typename Archive>
		void serialize(Archive&& archive)
		{
			archive(
				cereal::make_nvp("single_color", single_color)
			);
		}
	};

	struct CB_SingleColor
	{
		DirectX::XMFLOAT4 single_color{};
		DirectX::XMFLOAT3 padding{};

		void Edit()
		{
			ImGui::ColorEdit4("Color", (float*)&single_color);
		}
	};

	class SingleColor :public Shader<M_SingleColor>
	{
	private:
		const char* vs_name = "./Shader/cso/single_color_vs.cso";
		const char* ps_name = "./Shader/cso/single_color_ps.cso";

	public:
		SingleColor(ID3D11Device* device);

		void Activate(ID3D11DeviceContext* immediate_conntext) override;
		void Deactivate(ID3D11DeviceContext* immediate_context) override;
	};
}

#endif // __cplusplus

#endif // SINGLE_COLOR_H