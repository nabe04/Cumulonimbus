#include "texture_resource_manager.h"

#include <imgui.h>

#include "cum_imgui_helper.h"
#include "string_helper.h"
#include "texture_filename_mapping.h"

namespace cumulonimbus::manager::texture
{
	TextureResourceManager::TextureResourceManager(ID3D11Device* device)
	{
		dummy_texture_white  = std::make_unique<DummyTexture>(device, DirectX::XMFLOAT4{ 1.f,1.f,1.f,1.f });
		dummy_texture_normal = std::make_unique<DummyTexture>(device, DirectX::XMFLOAT4{ 0.5f,0.5f,1.0f,1.0f });

		InitialLoad(device);
	}

	void TextureResourceManager::InitialLoad(ID3D11Device* device)
	{
		using namespace mapping;
		CreateTexture(device, texture_filename::NoImage256().c_str());
		CreateTexture(device, texture_filename::Coffee256().c_str());
		CreateTexture(device, texture_filename::UvChecker().c_str());
		CreateTexture(device, texture_filename::DefaultNormalMap().c_str());
		CreateTexture(device, texture_filename::White8x8Map().c_str());
		CreateTexture(device, texture_filename::GanfaulNormalMap().c_str());
	}


	TextureResource* TextureResourceManager::CreateTexture(ID3D11Device* device, const std::string_view tex_filename)
	{
		const auto find = textures.find(tex_filename.data());
		if (find != textures.end())
		{// Already loaded
			return find->second.get();
		}

		const std::string str_filename = string_helper::toString(tex_filename);
		const auto tex = textures.insert(std::make_pair(str_filename, std::make_unique<TextureResource>(device, str_filename.c_str())));
		return tex.first->second.get();
	}

	void TextureResourceManager::RemoveTexture(const std::string_view tex_filename)
	{
		textures.erase(tex_filename.data());
	}

	bool TextureResourceManager::Contents(const std::string& texture_filename)
	{
		if (textures.contains(texture_filename))
			return true;

		return false;
	}

	void TextureResourceManager::ModifyTextureFilename(std::string& texture_filename, const std::string& combo_label)
	{
		const std::string my_texture = texture_filename;
		ImGui::Spacing();
		ImGui::Text(combo_label.c_str());

		if (textures.contains(texture_filename))
		{
			helper::imgui::Image(textures.at(texture_filename)->GetTextureData()->texture_view.Get());
			ImGui::SameLine();
		}

		if(ImGui::BeginCombo(combo_label.c_str(),texture_filename.c_str()))
		{
			for(const auto& tex : textures)
			{
				helper::imgui::Image(tex.second->GetTextureData()->texture_view.Get(), { 50,50 });
				ImGui::SameLine();
				const bool is_selected = (my_texture == tex.second->GetTextureData()->file_path);
				if (ImGui::Selectable(tex.first.c_str(), is_selected, 0, { 500,50 }))
				{
					texture_filename = tex.second->GetTextureData()->file_path;
				}
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo(); // ImGui::BeginCombo(combo_label.c_str(),texture_filename.c_str())
		}
	}

} // cumulonimbus::manager::texture
