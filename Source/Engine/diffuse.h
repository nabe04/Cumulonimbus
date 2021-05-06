#pragma once
#include <wrl.h>

#include "shader.h"

namespace shader
{
	class Diffuse final : public Shader<Empty>
	{
	private:
		const char* vs_name = "./Shader/cso/diffuse_vs.cso";
		const char* ps_name = "./Shader/cso/diffuse_ps.cso";

	public:
		Diffuse(ID3D11Device* device);

		void Activate(ID3D11DeviceContext* immediate_context) override;
		void Deactivate(ID3D11DeviceContext* immediate_context) override;
	};
}
