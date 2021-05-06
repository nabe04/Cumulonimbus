#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "shader.h"
#include "light.h"

#include "imgui.h"

namespace shader
{

	struct M_SpotLight
	{// (b6)
		//light::SpotLight datas[Light::SPOT_MAX] = {};

		void EditParam()
		{
			//for (int i = 0; i < Light::SPOT_MAX; i++)
			//{
			//	if (i >= Light::GetNumSpotLights())
			//	{
			//		datas[i].type = 0;
			//		break;
			//	}

			//	if (ImGui::TreeNode((void*)(intptr_t)i, "Info %d", i))
			//	{
			//		datas[i].index = i;
			//		datas[i].type = 1;
			//		ImGui::ColorEdit3("Color", (float*)&datas[i].color);
			//		ImGui::DragFloat3("Positon", (float*)&datas[i].position, 0.01f, -100.0f, 100.0f);
			//		ImGui::DragFloat3("Direction", (float*)&datas[i].direction, 0.01f, -1.0f, 1.0f);
			//		ImGui::DragFloat("Range", &datas[i].range, 0.1f, 0.0f, 100.0f, "%.3f");

			//		datas[i].near_area = 0.9f;
			//		datas[i].far_area = 0.8f;

			//		ImGui::TreePop();
			//	}
			//}
		}

	};

	//class SpotLight :public Shader<M_SpotLight>
	//{
	//private:
	//	const char* vs_name = "./Shader/cso/spot_light_vs.cso";
	//	const char* ps_name = "./Shader/cso/spot_light_ps.cso";

	//public:
	//	SpotLight(ID3D11Device* device);

	//	void Activate(ID3D11DeviceContext* immediate_context) override;
	//	void Deactivate(ID3D11DeviceContext* immediate_context) override;
	//};
}