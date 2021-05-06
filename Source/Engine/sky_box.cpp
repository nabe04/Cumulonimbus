#include "sky_box.h"

#include <cassert>
#include <string>

#include "imgui.h"
#include "scene.h"

#include "shader.h"

SkyBox::SkyBox(Entity* entity, ID3D11Device* device, const char* filename)
	:Component{ entity }
{
	component_name = "SkyBox";

	CreateVertexBufferAndIndexBuffer(device);
	CreateTextures(device, filename);

	vertex_shader = std::make_unique<shader::VertexShader>(device, vs_name.c_str());
	pixel_shader  = std::make_unique<shader::PixelShader>(device, ps_name.c_str());
}

void SkyBox::CreateVertexBufferAndIndexBuffer(ID3D11Device* device)
{
	HRESULT hr = E_FAIL;

	{// Create vertex buffer
		constexpr float side = 1.0f / 2.0f;

		std::array<shader::Vertex, 8> verteies{};
		verteies.at(0).position = DirectX::XMFLOAT4{ -side,-side,-side,1.0f };
		verteies.at(1).position = DirectX::XMFLOAT4{ side,-side,-side,1.0f };
		verteies.at(2).position = DirectX::XMFLOAT4{ -side, side,-side,1.0f };
		verteies.at(3).position = DirectX::XMFLOAT4{ side, side,-side,1.0f };
		verteies.at(4).position = DirectX::XMFLOAT4{ -side,-side, side,1.0f };
		verteies.at(5).position = DirectX::XMFLOAT4{ side,-side, side,1.0f };
		verteies.at(6).position = DirectX::XMFLOAT4{ -side, side, side,1.0f };
		verteies.at(7).position = DirectX::XMFLOAT4{ side, side, side,1.0f };

		D3D11_BUFFER_DESC vertex_buffer_desc{};
		vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		vertex_buffer_desc.ByteWidth = sizeof(shader::Vertex) * static_cast<u_int>(verteies.size());
		vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertex_buffer_desc.CPUAccessFlags = 0;
		vertex_buffer_desc.MiscFlags = 0;
		vertex_buffer_desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA init_data{};
		init_data.pSysMem = verteies.data();
		init_data.SysMemPitch = 0;
		init_data.SysMemSlicePitch = 0;

		hr = device->CreateBuffer(&vertex_buffer_desc, &init_data, vertex_buffer.GetAddressOf());
		if (FAILED(hr))
			assert(!"Create Vertex buffer error(sky box)");
	}

	{// Create Index buffer

		// Define the index buffer
		D3D11_BUFFER_DESC index_buffer_desc{};
		index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		index_buffer_desc.ByteWidth = sizeof(u_int) * 36;
		index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		index_buffer_desc.CPUAccessFlags = 0;
		index_buffer_desc.MiscFlags = 0;
		index_buffer_desc.StructureByteStride = 0;

		// Initialize Subresource of Index buffer
		u_int index_vertex_id[] =
		{
			0,2,1, 2,3,1,
			1,3,5, 3,7,5,
			2,6,3, 3,6,7,
			4,5,7, 4,7,6,
			0,4,2, 2,4,6,
			0,1,4, 1,5,4
		};

		D3D11_SUBRESOURCE_DATA init_data{};
		init_data.pSysMem = index_vertex_id;
		init_data.SysMemPitch = 0;
		init_data.SysMemSlicePitch = 0;

		hr = device->CreateBuffer(&index_buffer_desc, &init_data, index_buffer.GetAddressOf());
		if (FAILED(hr))
			assert(!"Create Index buffer error(sky box)");
	}
}

void SkyBox::CreateTextures(ID3D11Device* device, const char* filename)
{
	for (int i = 0; i < 6; ++i)
	{
		std::string filename_s = filename;
		std::string no = std::to_string(i) + ".png";
		filename_s += no;
		cube_texture.at(i) = std::make_unique<TextureResource>(device, filename_s.c_str());
	}

	// texture descriptor
	D3D11_TEXTURE2D_DESC texture_desc{};
	texture_desc.Width = cube_texture.at(0)->GetTextureData()->width;
	texture_desc.Height = cube_texture.at(0)->GetTextureData()->height;
	texture_desc.MipLevels = 1;
	texture_desc.ArraySize = 6;
	texture_desc.Format	= DXGI_FORMAT_R8G8B8A8_UNORM;
	texture_desc.SampleDesc.Count = 1;
	texture_desc.SampleDesc.Quality = 0;
	texture_desc.Usage = D3D11_USAGE_DEFAULT;
	texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texture_desc.CPUAccessFlags = 0;
	texture_desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	// subresource data
	D3D11_SUBRESOURCE_DATA init_data[6]{};
	for (int i = 0; i < 6; ++i)
	{
		init_data[i].pSysMem = cube_texture.at(i)->GetTextureData()->scratch.GetPixels();
		init_data[i].SysMemPitch = static_cast<u_int>(cube_texture.at(i)->GetTextureData()->scratch.GetImage(0, 0, 0)->rowPitch);
		init_data[i].SysMemSlicePitch = 0;
	}
	// create the texture resource
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	HRESULT hr = device->CreateTexture2D(&texture_desc, init_data, texture2d.GetAddressOf());
	if (FAILED(hr))
		assert(!"CreateTexture2D error(skybox.cpp)");

	// create the resource view on the texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
	srv_desc.Format = texture_desc.Format;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srv_desc.Texture2D.MostDetailedMip = 0;
	srv_desc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(texture2d.Get(), &srv_desc, texture_view.GetAddressOf());
	if (FAILED(hr))
		assert(!"CreateShaderResoueceView error(skybox.cpp)");
}

void SkyBox::RenderImGui()
{
	if (ImGui::TreeNode("Sky map texture"))
	{
		for (auto& it : cube_texture)
		{
			ImGui::Text(it->GetTextureData()->file_path.c_str());
			ImGui::SameLine();
			ImGui::Image((void*)it->GetTextureData()->texture_view.Get(), { 100,100 });
		}

		ImGui::TreePop();
	}
}

void SkyBox::Save(std::string file_path)
{
	const std::string file_path_and_name = file_path + component_name + ".json";
	std::ofstream ofs(file_path_and_name);
	cereal::JSONOutputArchive o_archive(ofs);
	o_archive(
		CEREAL_NVP(component_name),
		CEREAL_NVP(cube_texture),
		CEREAL_NVP(vs_name),
		CEREAL_NVP(ps_name)
	);
}

void SkyBox::Load(Entity* entity, std::string file_path_and_name)
{
	{
		std::ifstream ifs(file_path_and_name);
		cereal::JSONInputArchive i_archive(ifs);
		i_archive(
			CEREAL_NVP(component_name),
			CEREAL_NVP(cube_texture),
			CEREAL_NVP(vs_name),
			CEREAL_NVP(ps_name)
		);

		this->entity = entity;
	}

	CreateVertexBufferAndIndexBuffer(entity->GetScene()->GetFramework()->GetDevice());
	CreateTextures(entity->GetScene()->GetFramework()->GetDevice(),""); // Todo : SkyBox‚ÌTextureLoad•û–@‚ª–¢’è

	vertex_shader = std::make_unique<shader::VertexShader>(entity->GetScene()->GetFramework()->GetDevice(), vs_name.c_str());
	pixel_shader  = std::make_unique<shader::PixelShader>(entity->GetScene()->GetFramework()->GetDevice(), ps_name.c_str());
}

