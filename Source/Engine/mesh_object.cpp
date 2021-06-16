#include "mesh_object.h"

#include <string>
#include <vector>
#include <fstream>

#include <nameof.h>

#include "file_path_helper.h"

MeshObject::MeshObject(Entity* entity)
	:Component{ entity }
{
	component_name = "MeshObject";

	blend_state.SetState(BlendState::Alpha);
	rasterizer_state.SetState(RasterizeState::Cull_Back);
	sampler_state.SetState(RenderingSampleState::Linear_Border);
	depth_stencil_state.SetState(DepthStencilState::DepthTest_True_Write_True);
}

void MeshObject::RenderImGui()
{
	if (ImGui::TreeNode("Mesh States"))
	{
		shader_state.RenderImGui();

#pragma region blend state
		if (ImGui::TreeNode("Blend state"))
		{
			std::vector<std::string> names{};
			for (int i = 0; i < static_cast<int>(BlendState::End); ++i)
				names.emplace_back(NAMEOF_ENUM(static_cast<BlendState>(i)));

			if (ImGui::BeginCombo("Blend State", blend_state.GetCurrentStateName().c_str()))
			{
				for (int n = 0; n < static_cast<int>(BlendState::End); n++)
				{
					const bool is_selected = (static_cast<int>(blend_state.GetCurrentState()) == n);
					if (ImGui::Selectable(names.at(n).c_str(), is_selected))
					{
						if (static_cast<int>(blend_state.GetCurrentState()) != n)
						{
							blend_state.SetState(static_cast<BlendState>(n));
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
			std::vector<std::string> names{};
			for (int i = 0; i < static_cast<int>(RasterizeState::End); ++i)
				names.emplace_back(NAMEOF_ENUM(static_cast<RasterizeState>(i)));

			if (ImGui::BeginCombo("Blend State", rasterizer_state.GetCurrentStateName().c_str()))
			{
				for (int n = 0; n < static_cast<int>(RasterizeState::End); n++)
				{
					const bool is_selected = (static_cast<int>(rasterizer_state.GetCurrentState()) == n);
					if (ImGui::Selectable(names.at(n).c_str(), is_selected))
					{
						if (static_cast<int>(rasterizer_state.GetCurrentState()) != n)
						{
							rasterizer_state.SetState(static_cast<RasterizeState>(n));
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
			std::vector<std::string> names{};
			for (int i = 0; i < static_cast<int>(RenderingSampleState::End); ++i)
				names.emplace_back(NAMEOF_ENUM(static_cast<RenderingSampleState>(i)));

			if (ImGui::BeginCombo("Blend State", sampler_state.GetCurrentStateName().c_str()))
			{
				for (int n = 0; n < static_cast<int>(RenderingSampleState::End); n++)
				{
					const bool is_selected = (static_cast<int>(sampler_state.GetCurrentState()) == n);
					if (ImGui::Selectable(names.at(n).c_str(), is_selected))
					{
						if (static_cast<int>(sampler_state.GetCurrentState()) != n)
						{
							sampler_state.SetState(static_cast<RenderingSampleState>(n));
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
			std::vector<std::string> names{};
			for (int i = 0; i < static_cast<int>(DepthStencilState::End); ++i)
				names.emplace_back(NAMEOF_ENUM(static_cast<DepthStencilState>(i)));

			if (ImGui::BeginCombo("Depsh stencil State", depth_stencil_state.GetCurrentStateName().c_str()))
			{
				for (int n = 0; n < static_cast<int>(DepthStencilState::End); n++)
				{
					const bool is_selected = (static_cast<int>(depth_stencil_state.GetCurrentState()) == n);
					if (ImGui::Selectable(names.at(n).c_str(), is_selected))
					{
						if (static_cast<int>(depth_stencil_state.GetCurrentState()) != n)
						{
							depth_stencil_state.SetState(static_cast<DepthStencilState>(n));
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

void MeshObject::Save(std::string file_path)
{
	const std::string file_path_and_name = file_path + component_name + ".json";
	std::ofstream ofs(file_path_and_name);
	cereal::JSONOutputArchive o_archive(ofs);
	o_archive(
		CEREAL_NVP(component_name),
		CEREAL_NVP(blend_state),
		CEREAL_NVP(rasterizer_state),
		CEREAL_NVP(sampler_state),
		CEREAL_NVP( depth_stencil_state),

		CEREAL_NVP(shader_state),
		CEREAL_NVP(rendering_buffer)
	);
}

void MeshObject::Load(Entity* entity, std::string file_path_and_name)
{
	std::ifstream ifs(file_path_and_name);
	cereal::JSONInputArchive i_archive(ifs);
	i_archive(
		CEREAL_NVP(component_name),
		CEREAL_NVP(blend_state),
		CEREAL_NVP(rasterizer_state),
		CEREAL_NVP(sampler_state),
		CEREAL_NVP(depth_stencil_state),

		CEREAL_NVP(shader_state),
		CEREAL_NVP(rendering_buffer)
	);

	this->entity = entity;
}


namespace cumulonimbus::component
{
	MeshObjectComponent::MeshObjectComponent(ecs::Registry* const registry, const ecs::Entity ent)
		:ComponentBase{ registry, ent }
	{
		blend_state.SetState(BlendState::Alpha);
		rasterizer_state.SetState(RasterizeState::Cull_Back);
		sampler_state.SetState(RenderingSampleState::Linear_Border);
		depth_stencil_state.SetState(DepthStencilState::DepthTest_True_Write_True);
	}

	void MeshObjectComponent::RenderImGui()
	{
		if (ImGui::TreeNode("Mesh States"))
		{
			shader_state.RenderImGui();

#pragma region blend state
			if (ImGui::TreeNode("Blend state"))
			{
				std::vector<std::string> names{};
				for (int i = 0; i < static_cast<int>(BlendState::End); ++i)
					names.emplace_back(NAMEOF_ENUM(static_cast<BlendState>(i)));

				if (ImGui::BeginCombo("Blend State", blend_state.GetCurrentStateName().c_str()))
				{
					for (int n = 0; n < static_cast<int>(BlendState::End); n++)
					{
						const bool is_selected = (static_cast<int>(blend_state.GetCurrentState()) == n);
						if (ImGui::Selectable(names.at(n).c_str(), is_selected))
						{
							if (static_cast<int>(blend_state.GetCurrentState()) != n)
							{
								blend_state.SetState(static_cast<BlendState>(n));
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
				std::vector<std::string> names{};
				for (int i = 0; i < static_cast<int>(RasterizeState::End); ++i)
					names.emplace_back(NAMEOF_ENUM(static_cast<RasterizeState>(i)));

				if (ImGui::BeginCombo("Blend State", rasterizer_state.GetCurrentStateName().c_str()))
				{
					for (int n = 0; n < static_cast<int>(RasterizeState::End); n++)
					{
						const bool is_selected = (static_cast<int>(rasterizer_state.GetCurrentState()) == n);
						if (ImGui::Selectable(names.at(n).c_str(), is_selected))
						{
							if (static_cast<int>(rasterizer_state.GetCurrentState()) != n)
							{
								rasterizer_state.SetState(static_cast<RasterizeState>(n));
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
				std::vector<std::string> names{};
				for (int i = 0; i < static_cast<int>(RenderingSampleState::End); ++i)
					names.emplace_back(NAMEOF_ENUM(static_cast<RenderingSampleState>(i)));

				if (ImGui::BeginCombo("Blend State", sampler_state.GetCurrentStateName().c_str()))
				{
					for (int n = 0; n < static_cast<int>(RenderingSampleState::End); n++)
					{
						const bool is_selected = (static_cast<int>(sampler_state.GetCurrentState()) == n);
						if (ImGui::Selectable(names.at(n).c_str(), is_selected))
						{
							if (static_cast<int>(sampler_state.GetCurrentState()) != n)
							{
								sampler_state.SetState(static_cast<RenderingSampleState>(n));
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
				std::vector<std::string> names{};
				for (int i = 0; i < static_cast<int>(DepthStencilState::End); ++i)
					names.emplace_back(NAMEOF_ENUM(static_cast<DepthStencilState>(i)));

				if (ImGui::BeginCombo("Depsh stencil State", depth_stencil_state.GetCurrentStateName().c_str()))
				{
					for (int n = 0; n < static_cast<int>(DepthStencilState::End); n++)
					{
						const bool is_selected = (static_cast<int>(depth_stencil_state.GetCurrentState()) == n);
						if (ImGui::Selectable(names.at(n).c_str(), is_selected))
						{
							if (static_cast<int>(depth_stencil_state.GetCurrentState()) != n)
							{
								depth_stencil_state.SetState(static_cast<DepthStencilState>(n));
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

	void MeshObjectComponent::Save(const std::string& file_path)
	{
		const std::string file_path_and_name = file_path + file_path_helper::GetTypeName<MeshObjectComponent>() + file_path_helper::GetJsonExtension();
		std::ofstream ofs(file_path_and_name);
		cereal::JSONOutputArchive o_archive(ofs);
		o_archive(*this);
	}

	void MeshObjectComponent::Load(const std::string& file_path_and_name)
	{
		std::ifstream ifs(file_path_and_name);
		cereal::JSONInputArchive i_archive(ifs);
		i_archive(*this);
	}

}