#pragma once

#include <d3d11.h>

#include "shader.h"

namespace shader
{
	class StandardSprite final : public Shader<shader::Empty>
	{
	private:
		const char* vs_name = "./Shader/cso/standard_sprite_vs.cso";
		const char* ps_name = "./Shader/cso/standard_sprite_ps.cso";

	public:
		StandardSprite(ID3D11Device* device);

		void Activate(ID3D11DeviceContext* immediate_context) override;
		void Deactivate(ID3D11DeviceContext* immediate_context) override;
	};
}