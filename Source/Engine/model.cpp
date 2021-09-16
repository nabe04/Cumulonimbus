#include "model.h"

#include <fstream>
#include <set>

#include "cereal_helper.h"
#include "file_path_helper.h"
#include "locator.h"

namespace cumulonimbus::asset
{
	template <class Archive>
	void Model::serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(model_data)
		);
	}

	Model::Model(const std::filesystem::path& path)
	{
		Load(path);

		int a;
		a = 0;
	}

	void Model::Save(const std::filesystem::path& path)
	{
		{
			std::ofstream ofs(path.string() + file_path_helper::GetModelExtension(), std::ios_base::binary);
			cereal::BinaryOutputArchive output_archive(ofs);
			output_archive(*this);
		}
	}

	void Model::Load(const std::filesystem::path& path)
	{
		{// モデルのロード
			// pathの拡張子が「.model」の場合
			if (!std::set<std::filesystem::path>{".model"}.contains(path.extension()))
				assert(!"The extension is different(Model::Model)");

			std::ifstream ifs(path, std::ios_base::binary);
			if (!ifs)
				assert(!"Not open file");
			cereal::BinaryInputArchive input_archive(ifs);
			input_archive(*this);
		}

		for (auto& mesh : model_data.meshes)
		{
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

				HRESULT hr = locator::Locator::GetDx11Device()->device.Get()->CreateBuffer(&buffer_desc, &subresource_data, mesh.vertex_buffer.GetAddressOf());
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
				HRESULT hr = locator::Locator::GetDx11Device()->device.Get()->CreateBuffer(&buffer_desc, &subresource_data, mesh.index_buffer.GetAddressOf());
				if (FAILED(hr))
					assert(!"CreateBuffer(Index) error of FbxModelResource class ");
			}
		}

		auto m = GetModelData();
		int a;
		a = 0;
	}

} // cumulonimbus::asset
