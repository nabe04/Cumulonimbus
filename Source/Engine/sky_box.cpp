#include "sky_box.h"

#include <cassert>
#include <imgui.h>
#include <string>

#include "ecs.h"
#include "generic.h"
#include "locator.h"
#include "scene.h"
#include "shader.h"
#include "shader_filename_mapping.h"

//CEREAL_REGISTER_TYPE(cumulonimbus::component::SkyBoxComponent)
//CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::SkyBoxComponent)

namespace
{
	const std::string sky_box_file_path{ "./Data/Assets/cubemap/skybox" };
}

namespace cumulonimbus::graphics
{
	SkyBox::SkyBox(system::System& system, ID3D11Device* device, const std::string& filename)
	{
		CreateVertexBufferAndIndexBuffer(device);
		//if (filename.empty())
		//{
		//	file_path_and_name = sky_box_file_path;
		//	CreateTextures(device, sky_box_file_path.c_str());
		//}
		//else
		//{
		//	file_path_and_name = filename;
		//	CreateTextures(device, filename.c_str());
		//}

		CreateCubeTexture(device, "./Data/Assets/cubemap/Mars/mars.dds");

		cb_transform  = std::make_unique<buffer::ConstantBuffer<TransformCB>>(device);

		vertex_shader = std::make_unique<shader::VertexShader>(device, mapping::shader_filename::vs::SkyBox_VS().c_str());
		pixel_shader  = std::make_unique<shader::PixelShader>( device, mapping::shader_filename::ps::SkyBox_PS().c_str());

		// System::Renderì‡Ç≈é¿çsÇ∑ÇÈä÷êîÇÃìoò^
		system.RegisterRenderFunction(utility::GetHash<SkyBox>(), [&](ecs::Registry* registry) {RenderImGui(registry); });
	}

	void SkyBox::Update(const float dt)
	{
		// Worldïœä∑çsóÒÇÃçÏê¨
		scaling_mat		= DirectX::SimpleMath::Matrix::CreateScale(scale);
		rotation_mat	= DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(angle.y, angle.x, angle.z);
		translation_mat = DirectX::SimpleMath::Matrix::CreateTranslation(position);

		world_mat = scaling_mat * rotation_mat * translation_mat;

		cb_transform->data.bone_transforms[0] = world_mat;
	}

	void SkyBox::RenderImGui(ecs::Registry* registry)
	{
		if (ImGui::TreeNode("Sky map texture"))
		{
			for (auto& it : cube_textures)
			{
				ImGui::Text(it->GetFilePath().c_str());
				ImGui::SameLine();
				ImGui::Image((void*)it->GetShaderResourceView(), { 100,100 });
			}

			ImGui::TreePop();
		}
	}

	void SkyBox::Load(system::System& system)
	{
		ID3D11Device* device = locator::Locator::GetDx11Device()->device.Get();
		CreateVertexBufferAndIndexBuffer(device);
		CreateTextures(device, file_path_and_name.c_str());
		CreateCubeTexture(device, "./Data/Assets/cubemap/Mars/mars.dds");

		cb_transform  = std::make_unique<buffer::ConstantBuffer<TransformCB>>(device);

		vertex_shader = std::make_unique<shader::VertexShader>(device, mapping::shader_filename::vs::SkyBox_VS().c_str());
		pixel_shader  = std::make_unique<shader::PixelShader>( device, mapping::shader_filename::ps::SkyBox_PS().c_str());

		// System::Renderì‡Ç≈é¿çsÇ∑ÇÈä÷êîÇÃìoò^
		system.RegisterRenderFunction(utility::GetHash<SkyBox>(),
									  [&](ecs::Registry* registry) { RenderImGui(registry); });
	}

	void SkyBox::BindShader(ID3D11DeviceContext* immediate_context) const
	{
		vertex_shader->Activate(immediate_context);
		pixel_shader->Activate(immediate_context);
	}

	void SkyBox::UnBindShader(ID3D11DeviceContext* immediate_context) const
	{
		vertex_shader->Deactivate(immediate_context);
		pixel_shader->Deactivate(immediate_context);
	}

	void SkyBox::BindCBuffer(
		ID3D11DeviceContext* immediate_context,
		const bool set_in_vs, const bool set_in_ps) const
	{
		cb_transform->Activate(immediate_context, CBSlot_Transform, set_in_vs, set_in_ps);
	}

	void SkyBox::UnBindCBuffer(ID3D11DeviceContext* immediate_context) const
	{
		cb_transform->Deactivate(immediate_context);
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
			vertex_buffer_desc.Usage			   = D3D11_USAGE_DEFAULT;
			vertex_buffer_desc.ByteWidth		   = sizeof(shader::Vertex) * static_cast<u_int>(verteies.size());
			vertex_buffer_desc.BindFlags		   = D3D11_BIND_VERTEX_BUFFER;
			vertex_buffer_desc.CPUAccessFlags	   = 0;
			vertex_buffer_desc.MiscFlags		   = 0;
			vertex_buffer_desc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA init_data{};
			init_data.pSysMem			= verteies.data();
			init_data.SysMemPitch		= 0;
			init_data.SysMemSlicePitch	= 0;

			hr = device->CreateBuffer(&vertex_buffer_desc, &init_data, vertex_buffer.GetAddressOf());
			if (FAILED(hr))
				assert(!"Create Vertex buffer error(sky box)");
		}

		{// Create Index buffer

			// Define the index buffer
			D3D11_BUFFER_DESC index_buffer_desc{};
			index_buffer_desc.Usage					= D3D11_USAGE_DEFAULT;
			index_buffer_desc.ByteWidth				= sizeof(u_int) * 36;
			index_buffer_desc.BindFlags				= D3D11_BIND_INDEX_BUFFER;
			index_buffer_desc.CPUAccessFlags		= 0;
			index_buffer_desc.MiscFlags				= 0;
			index_buffer_desc.StructureByteStride	= 0;

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
			init_data.pSysMem			= index_vertex_id;
			init_data.SysMemPitch		= 0;
			init_data.SysMemSlicePitch	= 0;

			hr = device->CreateBuffer(&index_buffer_desc, &init_data, index_buffer.GetAddressOf());
			if (FAILED(hr))
				assert(!"Create Index buffer error(sky box)");
		}
	}

	void SkyBox::CreateCubeTexture(ID3D11Device* device, const std::string& filepath_and_name)
	{
		cube_texture = std::make_unique<asset::Texture>(device, filepath_and_name.c_str());
	}


	void SkyBox::CreateTextures(ID3D11Device* device, const char* filename)
	{
		for (int i = 0; i < 6; ++i)
		{
			std::string filename_s = filename;
			std::string no = std::to_string(i) + ".png";
			filename_s += no;
			cube_textures.at(i) = std::make_unique<asset::Texture>(device, filename_s.c_str());
		}

		// texture descriptor
		D3D11_TEXTURE2D_DESC texture_desc{};
		texture_desc.Width				= cube_textures.at(0)->GetWidth();
		texture_desc.Height				= cube_textures.at(0)->GetHeight();
		texture_desc.MipLevels			= 1;
		texture_desc.ArraySize			= 6;
		texture_desc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
		texture_desc.SampleDesc.Count	= 1;
		texture_desc.SampleDesc.Quality = 0;
		texture_desc.Usage				= D3D11_USAGE_DEFAULT;
		texture_desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
		texture_desc.CPUAccessFlags		= 0;
		texture_desc.MiscFlags			= D3D11_RESOURCE_MISC_TEXTURECUBE;
		// subresource data
		D3D11_SUBRESOURCE_DATA init_data[6]{};
		for (int i = 0; i < 6; ++i)
		{
			init_data[i].pSysMem = cube_textures.at(i)->GetScratchImage().GetPixels();
			init_data[i].SysMemPitch = static_cast<u_int>(cube_textures.at(i)->GetScratchImage().GetImage(0, 0, 0)->rowPitch);
			init_data[i].SysMemSlicePitch = 0;
		}
		// create the texture resource
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		HRESULT hr = device->CreateTexture2D(&texture_desc, init_data, texture2d.GetAddressOf());
		if (FAILED(hr))
			assert(!"CreateTexture2D error(skybox.cpp)");

		// create the resource view on the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
		srv_desc.Format						= texture_desc.Format;
		srv_desc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURECUBE;
		srv_desc.Texture2D.MostDetailedMip	= 0;
		srv_desc.Texture2D.MipLevels		= 1;
		hr = device->CreateShaderResourceView(texture2d.Get(), &srv_desc, texture_view.GetAddressOf());
		if (FAILED(hr))
			assert(!"CreateShaderResoueceView error(skybox.cpp)");
	}
} // cumulonimbus::graphics
