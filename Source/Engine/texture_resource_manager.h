#pragma once
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "texture.h"

namespace cumulonimbus::manager::texture
{
	class TextureResourceManager final
	{
	public:
		TextureResourceManager(ID3D11Device* device);
		~TextureResourceManager() = default;
		TextureResource* CreateTexture(ID3D11Device* device, const std::string_view tex_filename);

	  	[[nodiscard]] const std::map<std::string, std::unique_ptr<TextureResource>>& GetTextureResources() const
	  	{
			return textures;
	  	}

		bool Contents(const std::string& texture_filename);

		/*
		 * brief     : ImGui::Combo���g�p���āA������filename��ύX����
		 * ��caution : ImGui��ł̕ҏW�Ȃ̂�ImGui�ȊO�̗p�r�Ŏg�p���Ȃ��悤�ɒ���
		 */
		void ModifyTextureFilename(std::string& texture_filename, const std::string& combo_label);

	private:
		std::map<std::string, std::unique_ptr<TextureResource>> textures = {};
		std::unique_ptr<cumulonimbus::asset::DummyTexture> dummy_texture_white{ nullptr };
		std::unique_ptr<cumulonimbus::asset::DummyTexture> dummy_texture_normal{ nullptr };

		void InitialLoad(ID3D11Device* device);
		void RemoveTexture(const std::string_view tex_filename);
	};
} // cumulonimbus::manager::texture
