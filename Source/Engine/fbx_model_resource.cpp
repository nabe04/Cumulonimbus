#include "fbx_model_resource.h"

#include <cassert>
#include <fstream>

#include <fbxsdk.h>

#include "cereal_helper.h"

FbxModelResource::FbxModelResource(ID3D11Device* device, const char* import_filename, const char* dir_path)
{
	Import(import_filename);

	{// ファイル名取得 & 保存
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_FNAME];

		_splitpath_s(import_filename, NULL, NULL, dir, _MAX_DIR, fname,
			_MAX_FNAME, ext, _MAX_FNAME);

		model_name = fname;
	}

	for (auto& material : model_data.materials)
	{
		if (material.texture_filename.empty())
			continue;

		char path_buffer[_MAX_PATH];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_FNAME];

		_splitpath_s(material.texture_filename.c_str(), NULL, NULL, dir, _MAX_DIR, fname,
			_MAX_FNAME, ext, _MAX_FNAME);

		_makepath_s(path_buffer, _MAX_PATH, NULL, dir_path,
			fname, ext);

		// モデルに使われているテクスチャのファイル名から
		// ShaderResourceViewを作成
		material.shader_resource_view = TextureManager::GetInstance()->CreateTexture(device ,path_buffer)->GetTextureData()->texture_view;
	}

	for (auto& mesh : model_data.meshes)
	{
		for (auto& subset : mesh.subsets)
		{
			subset.material = &model_data.materials.at(subset.material_index);
		}

		// 頂点バッファ
		{
			D3D11_BUFFER_DESC buffer_desc = {};
			D3D11_SUBRESOURCE_DATA subresource_data = {};

			buffer_desc.ByteWidth = static_cast<UINT>(sizeof(ModelData::Vertex) * mesh.vertices.size());
			//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
			buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			buffer_desc.CPUAccessFlags = 0;
			buffer_desc.MiscFlags = 0;
			buffer_desc.StructureByteStride = 0;
			subresource_data.pSysMem = mesh.vertices.data();
			subresource_data.SysMemPitch = 0;
			subresource_data.SysMemSlicePitch = 0;

			HRESULT hr = device->CreateBuffer(&buffer_desc, &subresource_data, mesh.vertex_buffer.GetAddressOf());
			if (FAILED(hr))
				assert(!"CreateBuffer(Vertex) error of FbxModelResource class ");
		}

		// インデックスバッファ
		{
			D3D11_BUFFER_DESC buffer_desc = {};
			D3D11_SUBRESOURCE_DATA subresource_data = {};

			buffer_desc.ByteWidth = static_cast<UINT>(sizeof(u_int) * mesh.indices.size());
			//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
			buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			buffer_desc.CPUAccessFlags = 0;
			buffer_desc.MiscFlags = 0;
			buffer_desc.StructureByteStride = 0;
			subresource_data.pSysMem = mesh.indices.data();
			subresource_data.SysMemPitch = 0; //Not use for index buffers.
			subresource_data.SysMemSlicePitch = 0; //Not use for index buffers.
			HRESULT hr = device->CreateBuffer(&buffer_desc, &subresource_data, mesh.index_buffer.GetAddressOf());
			if (FAILED(hr))
				assert(!"CreateBuffer(Index) error of FbxModelResource class ");
		}
	}
}

FbxModelResource::~FbxModelResource()
{

}

void FbxModelResource::Import(const char* filename)
{
	std::ifstream ifs(filename, std::ios::binary);

	cereal::BinaryInputArchive archive(ifs);

	archive(
		CEREAL_NVP(model_data.nodes),
		CEREAL_NVP(model_data.materials),
		CEREAL_NVP(model_data.meshes),
		CEREAL_NVP(model_data.animations)
	);
}
