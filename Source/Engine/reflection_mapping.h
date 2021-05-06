#pragma once

#include "imgui.h"

#include "shader.h"

namespace shader
{
	class ReflectionMapping : public Shader<Empty>
	{
	private:
		const char* vs_name = "./Shader/cso/reflection_mapping_vs.cso";
		const char* ps_name = "./Shader/cso/reflection_mapping_ps.cso";

	public:
		explicit ReflectionMapping(ID3D11Device* device);

		void Activate(ID3D11DeviceContext* immediate_context) override;
		void Deactivate(ID3D11DeviceContext* immediate_context) override;
	};
}