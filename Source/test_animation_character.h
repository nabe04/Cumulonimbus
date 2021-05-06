#pragma once

#include "ecs.h"
#include "actor_component.h"

/**************************************
*
*
*	FBXのアニメーションテスト用
*
*
***************************************/

class AnimationCharacterComponent :public ActorComponent
{

public:
	AnimationCharacterComponent(Entity* entity);

	void NewFrame(const float delta_time) override;
	void Update(const float delta_time) override;
	void RenderImGui() override;
};