#include "obj_model_component.h"

#include <fstream>

#include "scene.h"
#include "view.h"
#include "string_helper.h"
#include "transform.h"
#include "transform_component.h"
#include "texture.h"
#include "shader_manager.h"
#include "light.h"

ObjModelComponent::ObjModelComponent(Entity* entity, ID3D11Device* device, const std::string_view filename)
	:Component{ entity }
{
	LoadOBJ(device, filename);
	SetCenerLocalPos();
}

void ObjModelComponent::NewFrame(const float delta_time)
{

}

void ObjModelComponent::Update(const float delta_time)
{
	auto* view = GetEntity()->GetScene()->GetView();

	SetCenterWorldPos();
}

void ObjModelComponent::RenderImGui()
{
	static shader::CB_Material edit_material{};

	edit_material.EditParam();
}

bool ObjModelComponent::LoadOBJ(ID3D11Device* device, std::string_view filename)
{
	u_int currentIndex = 0;

	std::vector<XMFLOAT4>    positions;
	std::vector<XMFLOAT3>    normals;
	std::vector<XMFLOAT2>    texcoords;
	std::vector<std::string> mtl_filenames;

	dirPath = string_helper::getDirectoryPath(filename);	// Get directory path

	{
		std::ifstream fin(string_helper::toString(filename).c_str());
		if (!fin)
			assert(!"OBJ file not found");

		char command[256];
		while (fin)
		{
			fin >> command;
			if (0 == strcmp(command, "mtllib"))
			{
				std::string mtllib;
				fin >> mtllib;
				mtl_filenames.push_back(mtllib);
				fin.ignore(1024, '\n');
			}
			else if (0 == strcmp(command, "usemtl"))
			{
				char usemtl[256];
				fin >> usemtl;

				Subset current_subset{};
				current_subset.usemtl = usemtl;
				current_subset.index_start = static_cast<u_int>(indices.size());
				subsets.push_back(current_subset);
				fin.ignore(1024, '\n');
			}
			else if (0 == strcmp(command, "v"))
			{
				float x, y, z;
				fin >> x >> y >> z;
				positions.push_back(XMFLOAT4{ x,y,z ,1.0f});
				PosComparison(XMFLOAT3{ x,y,z });
				fin.ignore(1024, '\n');
			}
			else if (0 == strcmp(command, "vn"))
			{
				float x, y, z;
				fin >> x >> y >> z;
				normals.push_back(XMFLOAT3{ x,y,z });
				fin.ignore(1024, '\n');
			}
			else if (0 == strcmp(command, "vt"))
			{
				float u, v;
				fin >> u >> v;
				texcoords.push_back(XMFLOAT2{ u,1.0f - v });
				fin.ignore(1024, '\n');
			}
			else if (0 == strcmp(command, "f"))
			{
				for (u_int i = 0; i < 3; i++)
				{
					shader::Vertex vertex;
					u_int v, vt, vn;

					fin >> v;
					vertex.position = positions[v - 1];
					if (L'/' == fin.peek())
					{
						fin.ignore();
						if (L'/' != fin.peek())
						{
							fin >> vt;
							vertex.tex = texcoords[vt - 1];
						}

						if (L'/' == fin.peek())
						{
							fin.ignore();
							fin >> vn;
							vertex.normal = normals[vn - 1];
						}
					}
					vertices.push_back(vertex);
					indices.push_back(currentIndex++);
				}
				fin.ignore(1024, L'\n');
			}
			else
			{
				fin.ignore(1024, L'\n');
			}
		}
		fin.close();
	}

	std::vector<Subset>::reverse_iterator rev_iterator = subsets.rbegin();				// 最後の要素のポインタを取得
	rev_iterator->index_count = static_cast<u_int>(indices.size()) - rev_iterator->index_start;					// インデックスバッファの個数を取得 (全体のインデックスのサイズ - 最後のインデックスの開始位置)
	for (rev_iterator = subsets.rbegin() + 1; rev_iterator != subsets.rend(); ++rev_iterator)	// for文の最初のところで +1 しているのはその上の行ですでに初めのカウントを代入しているから
	{
		rev_iterator->index_count = (rev_iterator - 1)->index_start - rev_iterator->index_start;	// 現在のイテレーターのカウントを算出 (一つ前のイテレーターのスタート位置 - 現在のイテレーターのスタート位置)
	}

	{//-- mtl file --//
		//const std::string =

		std::ifstream fin(dirPath + mtl_filenames[0].c_str());
		if (!fin)
			assert(!"OBJ file not found");

		char command[256];
		while (fin)
		{
			fin >> command;
			if (0 == strcmp(command, "#"))
			{
				// Comment
				fin.ignore(1024, '\n');
			}
			else if (0 == strcmp(command, "map_Kd"))
			{
				fin.ignore();
				std::string map_Kd;
				fin >> map_Kd;
				materials.rbegin()->map_Kd = dirPath + map_Kd;
				materials.rbegin()->texture = TextureManager::GetInstance()->CreateTexture(device ,materials.rbegin()->map_Kd);
				fin.ignore(1024, '\n');
			}
			else if (0 == strcmp(command, "newmtl"))
			{
				fin.ignore();
				std::string newmtl;
				Material material;
				fin >> newmtl;
				material.newmtl = newmtl;
				materials.push_back(material);
				fin.ignore(1024, '\n');
			}
			else if (0 == strcmp(command, "Ka"))
			{
				float r, g, b;
				fin >> r >> g >> b;
				materials.rbegin()->data.Ka = DirectX::XMFLOAT4(r, g, b,1.f);		// 末尾の要素にアクセス
				fin.ignore(1024, '\n');
			}
			else if (0 == strcmp(command, "Kd"))
			{
				float r, g, b;
				fin >> r >> g >> b;
				materials.rbegin()->data.Kd = DirectX::XMFLOAT4(r, g, b, 1.f);
				fin.ignore(1024, '\n');
			}
			else if (0 == strcmp(command, "Ks"))
			{
				float r, g, b;
				fin >> r >> g >> b;
				materials.rbegin()->data.Ks = DirectX::XMFLOAT4(r, g, b, 1.f);
				fin.ignore(1024, '\n');
			}
			else if (0 == strcmp(command, "illum"))
			{
				u_int illum;
				fin >> illum;
				materials.rbegin()->illum = illum;
				fin.ignore(1024, '\n');
			}
			else
			{
				fin.ignore(1024, '\n');
			}
		}
	}

	CreateBuffers(device, vertices.data(), static_cast<int>(vertices.size()), indices.data(), static_cast<int>(indices.size()));

	return true;
}

ObjModelComponent::~ObjModelComponent()
{
	for (auto& material : materials)
	{
		if (material.texture)
		{
			TextureManager::GetInstance()->DecrementRefCount(material.texture->GetTextureFilename());
		}
	}
}

void ObjModelComponent::CreateBuffers(ID3D11Device* device,
	shader::Vertex* vertices, int num_vertices,
	u_int* indices, int num_indices)
{
	HRESULT hr;
	{//-- Create vertex buffer --//
		D3D11_BUFFER_DESC bd{};

		bd.ByteWidth			= sizeof(shader::Vertex) * num_vertices;
		bd.Usage				= D3D11_USAGE_DEFAULT;
		bd.BindFlags			= D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags		= 0;
		bd.MiscFlags			= 0;
		bd.StructureByteStride	= 0;

		D3D11_SUBRESOURCE_DATA initData{};
		initData.pSysMem			= vertices;
		initData.SysMemPitch		= 0;
		initData.SysMemSlicePitch	= 0;

		hr = device->CreateBuffer(&bd, &initData, vertex_buffer.GetAddressOf());
		if (FAILED(hr))
			assert(!"CreateBuffer (Vertex Buffer) error");
	}
	{//-- Create index buffer --//
		D3D11_BUFFER_DESC bd{};

		bd.ByteWidth			= sizeof(u_int) * num_indices;
		bd.Usage				= D3D11_USAGE_DEFAULT;
		bd.BindFlags			= D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags		= 0;
		bd.MiscFlags			= 0;
		bd.StructureByteStride	= 0;

		D3D11_SUBRESOURCE_DATA initData{};
		initData.pSysMem			= indices;
		initData.SysMemPitch		= 0;
		initData.SysMemSlicePitch	= 0;

		hr = device->CreateBuffer(&bd, &initData, index_buffer.GetAddressOf());
		if (FAILED(hr))
			assert(!"CreateBuffer (Index Buffer) error");
	}
}

void ObjModelComponent::PosComparison(const XMFLOAT3 pos)
{
	if (top < pos.y)
	{
		top = pos.y;
	}
	if (bottom > pos.y)
	{
		bottom = pos.y;
	}
	if (right < pos.x)
	{
		right = pos.x;
	}
	if (left > pos.x)
	{
		left = pos.x;
	}
	if (front > pos.z)
	{
		front = pos.z;
	}
	if (back < pos.z)
	{
		back = pos.z;
	}
}

void ObjModelComponent::SetCenerLocalPos()
{
	center_pos.x = (left + right) / 2;
	center_pos.y = (top + bottom) / 2;
	center_pos.z = (front + back) / 2;
}

void ObjModelComponent::SetCenterWorldPos()
{
	center_pos.x += GetEntity()->GetComponent<TransformComponent>()->GetTransform()->DeltaPosition().x;
	center_pos.y += GetEntity()->GetComponent<TransformComponent>()->GetTransform()->DeltaPosition().y;
	center_pos.z += GetEntity()->GetComponent<TransformComponent>()->GetTransform()->DeltaPosition().z;
}