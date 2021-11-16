//#pragma once
//
//#include <array>
//#include <bitset>
//#include <string>
//
//namespace cumulonimbus::scene
//{
//	class Scene;
//} // cumulonimbus::scene
//
//
//class ContentBrowser final
//{
//	enum Contents
//	{
//		FBX,
//		Texture,
//
//		End,
//	};
//
//private:
//	inline static const std::array<std::string, 2> contents = { "FBX Mesh" ,"Texture" };
//	std::bitset<contents.size()> is_selected{};
//
//public:
//	explicit ContentBrowser() = default;
//	~ContentBrowser() = default;
//
//	void Render(cumulonimbus::scene::Scene* scene);
//
//private:
//	void SelectedContent(cumulonimbus::scene::Scene* scene);
//};