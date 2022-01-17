#include "time_scale.h"

#include "cum_imgui_helper.h"

namespace cumulonimbus::system
{
	template <class Archive>
	void Time::load(Archive&& archive, uint32_t version)
	{
		archive(
			CEREAL_NVP(scale)
		);
	}

	template <class Archive>
	void Time::save(Archive&& archive, uint32_t version) const
	{
		archive(
			CEREAL_NVP(scale)
		);
	}

	void Time::RenderImGui(ecs::Registry* registry)
	{
		if (const ImGuiTreeNodeFlags tree_flg{ ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth };
			ImGui::CollapsingHeader(ICON_FA_CLOCK"Time Scale", tree_flg))
		{
			IMGUI_LEFT_LABEL(ImGui::DragFloat, "Scale", &scale, 0.1f, 0.0f, FLT_MAX);
		}
	}

} // cumulonimbus::system
