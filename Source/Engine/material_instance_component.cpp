#include "material_instance_component.h"

#include <imgui.h>
#include <cereal/types/vector.hpp>

#include "ecs.h"
#include "shader_assets_component.h"

namespace cumulonimbus::component
{
	MaterialInstance3DComponent::MaterialInstance3DComponent(ecs::Registry* registry,
															 mapping::rename_type::Entity ent)
		:ComponentBase{ registry,ent }
	{
		registry->AddComponent<component::ShaderAssets3DComponent>(ent);
	}

	void MaterialInstance3DComponent::RenderImGui()
	{
		if (ImGui::TreeNode("Material Instance"))
		{
			if(ImGui::BeginCombo("Shader Type",shader_assets.GetCurrentStateName().c_str()))
			{
				for(auto& name : shader_assets.GetStateNames())
				{
					bool is_selected = (shader_assets.GetCurrentState() == shader_assets.GetStateMap().at(name));

					if(ImGui::Selectable(name.c_str(), is_selected))
					{
						shader_assets.SetState(shader_assets.GetStateMap().at(name));
					}
					if(is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}

			GetRegistry()->GetComponent<component::ShaderAssets3DComponent>(GetEntity()).RenderImGui(shader_assets.GetCurrentState());

			ImGui::TreePop();
		}
	}

	void MaterialInstance3DComponent::Save(const std::string& file_path)
	{
	}

	void MaterialInstance3DComponent::Load(const std::string& file_path_and_name)
	{
	}

}

