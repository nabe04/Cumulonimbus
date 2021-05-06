#pragma once

#include <memory>

#include <DirectXMath.h>

#include "imgui.h"

#include "shader.h"
#include "constant_buffer.h"

namespace shader
{
	struct M_SingleColor
	{
		DirectX::XMFLOAT4 single_color{};

		void EditParam()
		{
			ImGui::ColorEdit4("Color", (float*)&single_color);
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