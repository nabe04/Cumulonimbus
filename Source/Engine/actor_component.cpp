#include "actor_component.h"

ActorComponent::ActorComponent(Entity* entity, ActorType actor_type)
	: Component{ entity }
	, actor_type{ actor_type }
{
	if (actor_type == ActorType::Actor3D)
	{
		shader_state = std::make_shared<shader::MeshShaderState>();
	}
	else if (actor_type == ActorType::Actor2D)
	{
		shader_state_2D = std::make_shared<shader::SpriteShaderState>();
	}

	rendering_buffer = std::make_unique<RenderingBufferBitset>();
}

void ActorComponent::RenderImGui()
{
	if (actor_type == ActorType::Actor3D)
	{
		shader_state->RenderImgui();
	}
	else if (actor_type == ActorType::Actor2D)
	{
		//shader_state_2D-
	}
};