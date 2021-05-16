#include "mesh_object.h"

#include <string>
#include <vector>

#include <nameof.h>

MeshObject::MeshObject(Entity* entity)
	:Component{ entity }
{
	shader_state = std::make_shared<shader::MeshShaderState>();
	rendering_buffer = std::make_unique<RenderingBufferBitset>();
}

void MeshObject::RenderImGui()
{
	if (ImGui::TreeNode("Mesh States"))
	{
		shader_state->RenderImGui();

#pragma region blend state
		if (ImGui::TreeNode("Blend state"))
		{
			static int item_blend = static_cast<int>(blend_state);

			std::vector<std::string> names{};
			for (int i = 0; i < static_cast<int>(BlendState::End); ++i)
				names.emplace_back(NAMEOF_ENUM(static_cast<BlendState>(i)));

			if (ImGui::BeginCombo("Blend State", NAMEOF_ENUM(blend_state).data()))
			{
				for (int n = 0; n < static_cast<int>(BlendState::End); n++)
				{
					const bool is_selected = (static_cast<int>(blend_state) == n);
					if (ImGui::Selectable(names.at(n).c_str(), is_selected))
					{
						if (static_cast<int>(blend_state) != n)
						{
							blend_state = static_cast<BlendState>(n);
						}
					}
					//Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::TreePop();
		}
#pragma endregion blend state

#pragma region rasterizer state
		if (ImGui::TreeNode("Rasterizer state"))
		{
			static int item_blend = static_cast<int>(rasterizer_state);

			std::vector<std::string> names{};
			for (int i = 0; i < static_cast<int>(RasterizeState::End); ++i)
				names.emplace_back(NAMEOF_ENUM(static_cast<RasterizeState>(i)));

			if (ImGui::BeginCombo("Blend State", NAMEOF_ENUM(rasterizer_state).data()))
			{
				for (int n = 0; n < static_cast<int>(RasterizeState::End); n++)
				{
					const bool is_selected = (static_cast<int>(rasterizer_state) == n);
					if (ImGui::Selectable(names.at(n).c_str(), is_selected))
					{
						if (static_cast<int>(rasterizer_state) != n)
						{
							rasterizer_state = static_cast<RasterizeState>(n);
						}
					}
					//Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::TreePop();
		}
#pragma endregion rasterizer state

#pragma region sampler state
		if (ImGui::TreeNode("Sampler state"))
		{
			static int item_blend = static_cast<int>(sampler_state);

			std::vector<std::string> names{};
			for (int i = 0; i < static_cast<int>(RenderingSampleState::End); ++i)
				names.emplace_back(NAMEOF_ENUM(static_cast<RenderingSampleState>(i)));

			if (ImGui::BeginCombo("Blend State", NAMEOF_ENUM(sampler_state).data()))
			{
				for (int n = 0; n < static_cast<int>(RenderingSampleState::End); n++)
				{
					const bool is_selected = (static_cast<int>(sampler_state) == n);
					if (ImGui::Selectable(names.at(n).c_str(), is_selected))
					{
						if (static_cast<int>(sampler_state) != n)
						{
							sampler_state = static_cast<RenderingSampleState>(n);
						}
					}
					//Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::TreePop();
		}
#pragma endregion sampler state

#pragma region depths stencil state
		if (ImGui::TreeNode("Depsh stencil state"))
		{
			static int item_blend = static_cast<int>(depth_stencil_state);

			std::vector<std::string> names{};
			for (int i = 0; i < static_cast<int>(DepthStencilState::End); ++i)
				names.emplace_back(NAMEOF_ENUM(static_cast<DepthStencilState>(i)));

			if (ImGui::BeginCombo("Depsh stencil State", NAMEOF_ENUM(depth_stencil_state).data()))
			{
				for (int n = 0; n < static_cast<int>(DepthStencilState::End); n++)
				{
					const bool is_selected = (static_cast<int>(depth_stencil_state) == n);
					if (ImGui::Selectable(names.at(n).c_str(), is_selected))
					{
						if (static_cast<int>(depth_stencil_state) != n)
						{
							depth_stencil_state = static_cast<DepthStencilState>(n);
						}
					}
					//Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::TreePop();
		}
#pragma endregion depths stencil state

		ImGui::TreePop();
	}
};