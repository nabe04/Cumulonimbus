#pragma once

#include "my_effekseer.h"
#include "input_manager.h"

class EffekseerTest :public EffekseerComponent
{
public:
	EffekseerTest(Entity* entity) :EffekseerComponent{ entity } {}

	void Play() override;
};