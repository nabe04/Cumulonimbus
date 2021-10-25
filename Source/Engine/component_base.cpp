#include "component_base.h"

namespace
{
	const std::string context_id{ "context_menu" };	// ImGui::PopupのコンテキストメニューID
}

namespace cumulonimbus::component
{
	void ComponentBase::MenuPopup(
		const mapping::rename_type::Entity& ent,
		bool& is_remove_component) const
	{
		ImGui::PushID(ent.c_str());
		if (ImGui::Button(ICON_FA_ELLIPSIS_V))
		{
			ImGui::OpenPopup(context_id.c_str());
		}
		if (ImGui::BeginPopup(context_id.c_str()))
		{
			if (ImGui::MenuItem("Remove Component"))
			{
				is_remove_component = true;
			}
			else
			{
				is_remove_component = false;
			}
			ImGui::EndPopup();
		}
		ImGui::SameLine();
	//	const bool ret_val = ImGui::CollapsingHeader(header_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

		//return ret_val;
		//return true;
	}
} // cumulonimbus::component