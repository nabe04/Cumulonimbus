#include "graphics_state.h"

namespace cumulonimbus::graphics
{
	//template <class Archive>
	//void GraphicsState::serialize(Archive&& archive)
	//{
	//	archive(
	//		CEREAL_NVP(blend_state),
	//		CEREAL_NVP(rasterizer_state),
	//		CEREAL_NVP(depth_stencil_state),
	//		CEREAL_NVP(sampler_state)
	//	);
	//}

	void GraphicsState::RenderImGui()
	{
		if (ImGui::TreeNode("Mesh States"))
		{
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
			ImGui::TreePop();
		}
	}
} // cumulonimbus::graphics
