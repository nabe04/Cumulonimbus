#pragma once

#include <array>
#include <bitset>
#include <string>

class Scene;

class ContentBrowser final
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
	explicit ContentBrowser() = default;
	~ContentBrowser() = default;

	void Render(Scene* scene);

private:
	void SelectedContent(Scene* scene);
};