#pragma once

#include "shader.h"

namespace shader
{
	// Todo : è¡Ç∑
	class Texture2DShader final :public Shader<Empty>
	{
	private:
		const char* vs_name = "./Shader/cso/sprite2d_vs.cso";
		const char* ps_name = "./Shader/cso/sprite2d_ps.cso";

	public:
		explicit Texture2DShader(ID3D11Device* device);
		~Texture2DShader() = default;

		void Activate(ID3D11DeviceContext* immediate_context) override;
		void Deactivate(ID3D11DeviceContext* immediate_context) override;
	};
}
