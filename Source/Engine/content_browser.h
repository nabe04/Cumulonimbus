#pragma once

#include <array>
#include <bitset>
#include <string>

#include <imgui.h>

class Scene;

class ContentBrawser final
{
	enum Contents
	{
		FBX,
		Texture,

		End,
	};

private:
	inline static const std::array<std::string, 2> contents = { "FBX Mesh" ,"Texture" };
	std::bitset<contents.size()> is_selected{};

public:
	explicit ContentBrawser() = default;
	~ContentBrawser() = default;

	[[noreturn]] void Render(Scene* scene);

private:
	void SelectedContent(Scene* scene);
 };