#pragma once
#include <wrl.h>
#include <string>

#include "shader.h"

namespace shader
{
	class Standard3D final : public Shader<Empty>
	{
	private:
		const char* vs_name = "./Shader/cso/3d_standard_vs.cso";
		const char* ps_name = "./Shader/cso/3d_standard_ps.cso";

	public:
		Standard3D(ID3D11Device* device);

		void Activate(ID3D11DeviceContext* immediate_context) override;
		void Deactivate(ID3D11DeviceContext* immediate_context) override;
	};

}
