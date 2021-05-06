#pragma once

#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

#include "ecs.h"
#include "transform.h"
#include "transform_component.h"

class EffekseerComponent : public Component
{
protected:
	Effekseer::Handle				handle = -1;

public:
	EffekseerComponent(Entity* entity) :Component{ entity } {};

	const Effekseer::Handle  GetHandle()  { return handle; }

	virtual void NewFrame(const float delta_time) {};
	virtual void Update(const float delta_time)  {};
	virtual void Play() = 0;
};