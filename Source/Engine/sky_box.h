#pragma once

#include <array>
#include <memory>
#include <string>

#include <d3d11.h>
#include <wrl.h>

#include "cereal/cereal.hpp"
#include "cereal/types/array.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/types/string.hpp"

#include "ecs.h"
#include "component_base.h"
#include "texture.h"
#include "shader.h"

class SkyBox :public Component
{
private:
	std::array<std::unique_ptr<TextureResource>, 6> cube_texture;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture_view;

	std::string vs_name = "./Shader/cso/sky_box_vs.cso";
	std::string ps_name = "./Shader/cso/sky_box_ps.cso";
	std::unique_ptr<shader::VertexShader> vertex_shader;
	std::unique_ptr<shader::PixelShader>  pixel_shader;

private:
	void CreateVertexBufferAndIndexBuffer(ID3D11Device* device);
	void CreateTextures(ID3D11Device* device, const char* filename);

public:
	/*
	* brief スカイボックスの作成
	* filename : キューブマップのファイル名(番号は書かない)
	*/
	SkyBox(Entity* entity, ID3D11Device* device, const char* filename);
	SkyBox() = default; // for cereal

	void ActivateShader(ID3D11DeviceContext* immediate_context) const
	{
		vertex_shader->Activate(immediate_context);
		pixel_shader->Activate(immediate_context);
	}

	void DeactivateShader(ID3D11DeviceContext* immediate_context) const
	{
		vertex_shader->Deactivate(immediate_context);
		pixel_shader->Deactivate(immediate_context);
	}

	ID3D11Buffer* GetVertexBuffer() { return vertex_buffer.Get(); }
	ID3D11Buffer* GetIndexBuffer() { return index_buffer.Get(); }
	ID3D11Buffer** GetVertexBufferAddress() { return vertex_buffer.GetAddressOf(); }
	ID3D11Buffer** GetIndexBufferAddress() { return index_buffer.GetAddressOf(); }

	ID3D11ShaderResourceView* GetShaderResoueceView() { return texture_view.Get(); }
	ID3D11ShaderResourceView** GetShaderResoueceViewAddress() { return texture_view.GetAddressOf(); }

	void RenderImGui() override;

	void Save(std::string file_path) override;
	void Load(Entity* entity, std::string file_path_and_name) override;

	template <class Archive>
	void serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(component_name),
			CEREAL_NVP(cube_texture),
			CEREAL_NVP(vs_name),
			CEREAL_NVP(ps_name)
		);
	}
};