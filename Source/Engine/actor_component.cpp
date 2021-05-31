#include "actor_component.h"

ActorComponent::ActorComponent(Entity* entity, ActorType actor_type)
	: Component{ entity }
{
	this->actor_type.SetState(actor_type);

	blend_state.SetState(BlendState::Alpha);
	rasterizer_state.SetState(RasterizeState::Cull_Back);
	sampler_state.SetState(RenderingSampleState::Linear_Border);
	depth_stencil_state.SetState(DepthStencilState::DepthTest_True_Write_True);
}

void ActorComponent::RenderImGui()
{
	if (actor_type.GetCurrentState() == ActorType::Actor3D)
	{
		shader_state.RenderImGui();
	}
	else if (actor_type.GetCurrentState() == ActorType::Actor2D)
	{
		//shader_state_2D-
	}
};