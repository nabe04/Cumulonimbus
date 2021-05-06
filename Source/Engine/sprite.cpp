#include "sprite.h"

#include <cassert>

#include <Windows.h>

#include "framework.h"
#include "shader.h"
#include "scene.h"
#include "framework.h"
#include "texture.h"
#include "transform.h"
#include "transform_component.h"
#include "sprite_object.h"
#include "locator.h"

SpriteComponent::SpriteComponent(Entity* entity, ID3D11Device* device,
	const char* texture_filename, const PivotType pivot_type,
	const int src_left, const int src_top,
	const int src_width, const int src_height)
	: Component{ entity }
	, pivot_type{ pivot_type }
	, src_pivot{ 0, 0 }
	, src_width{ src_width }
	, src_height{ src_height }
	, variable_width{ src_width }
	, variable_height{ src_height }
{
	if (!entity->HasComponent<SpriteObject>())
		assert(!"Don't have SpriteObject(SpriteComponent)");

	component_name = "Sprite";
	//texture = std::make_unique<TextureResource>(device, texture_filename);
	texture = Locator::GetResourceManager().GetTextureResouece(texture_filename);

	variable_texcoords.at(0) = { .0f,.0f };
	variable_texcoords.at(1) = { 1.f,.0f };
	variable_texcoords.at(2) = { .0f,1.f };
	variable_texcoords.at(3) = { 1.f,1.f };

	AjustSrcTexturePivot(pivot_type, src_left, src_top, src_width, src_height);
	CreateVertexBuffer(device);
}

void SpriteComponent::Update(const float delta_time)
{
	auto* transform = GetEntity()->GetComponent<TransformComponent>()->GetTransform();

	if (!transform)
		assert(!"Not found component(TransformComponenet)");

	// 2Dに必要ない値のリセット
	transform->SetPositionZ(0.0f);
	transform->SetScaleZ(1.f);
	transform->SetWorldRotation_X(.0f);
	transform->SetWorldRotation_Y(.0f);

	DirectX::XMFLOAT2 clip_rate{};

	clip_rate.x = static_cast<float>(variable_width) / static_cast<float>(src_width);
	clip_rate.y = static_cast<float>(variable_height) / static_cast<float>(src_height);

	// Setting texcoords
	variable_texcoords.at(1).x = clip_rate.x; // Righit top
	variable_texcoords.at(2).y = clip_rate.y; // Left bottom
	variable_texcoords.at(3)   = clip_rate;   // Right bottom
}

void SpriteComponent::AjustSrcTexturePivot(const PivotType pivot_type,
	const int src_left, const int src_top,
	const int src_width, const int src_height)
{
	switch (pivot_type)
	{
	case PivotType::Center:
		src_pivot.x = static_cast<float>(src_width / 2);
		src_pivot.y = static_cast<float>(src_height / 2);
		break;
	case PivotType::BottomCenter:
		src_pivot.x = static_cast<float>(src_width / 2);
		src_pivot.y = static_cast<float>(src_height);
		break;
	case PivotType::TopCenter:
		src_pivot.x = static_cast<float>(src_width / 2);
		src_pivot.y = static_cast<float>(src_top);
		break;
	case PivotType::LeftTop:
		src_pivot.x = static_cast<float>(src_left);
		src_pivot.y = static_cast<float>(src_top);
		break;
	case PivotType::LeftBottom:
		src_pivot.x = static_cast<float>(src_left);
		src_pivot.y = static_cast<float>(src_height);
		break;
	case PivotType::RightTop:
		src_pivot.x = static_cast<float>(src_width);
		src_pivot.y = static_cast<float>(src_top);
		break;
	case PivotType::RightBottom:
		src_pivot.x = static_cast<float>(src_width);
		src_pivot.y = static_cast<float>(src_height);
		break;
	}
}

void SpriteComponent::CreateVertexBuffer(ID3D11Device* device)
{
	HRESULT hr = E_FAIL;
	std::array<shader::VertexSprite, 4> vertices;

	{
		D3D11_BUFFER_DESC bd{};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(shader::VertexSprite) * static_cast<u_int>(vertices.size());
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData{};
		initData.pSysMem = vertices.data();

		HRESULT hr = device->CreateBuffer(&bd, &initData, vertex_buffer.GetAddressOf());
		if (FAILED(hr))
			assert(!"CreateBuffer Error (Texture2DComponent class)");
	}
}

void SpriteComponent::RenderImGui()
{
	if (ImGui::TreeNode("Sprite Data"))
	{
		if (ImGui::TreeNode("Image"))
		{
			ImGui::Text(texture->GetTextureData()->file_path.c_str());
			ImGui::SameLine();
			ImGui::Image((void*)texture->GetTextureData()->texture_view.Get(), { image_size.x,image_size.y });

			if (ImGui::Button("Reset"))
			{
				image_size = DirectX::XMFLOAT2{ 100.0f,100.0f };
			}

			ImGui::SliderFloat2("Size", (float*)&image_size, 0.0f, 300.0f);
			ImGui::TreePop();
		}

		ImGui::SliderInt("Clip Width", &variable_width, 0, src_width);
		ImGui::SliderInt("Clip Height", &variable_height, 0, src_height);

		ImGui::TreePop();
	}
}


void SpriteComponent::Save(std::string file_path)
{
	const std::string file_path_and_name = file_path + component_name + ".json";
	std::ofstream ofs(file_path_and_name);
	cereal::JSONOutputArchive o_archive(ofs);
	o_archive(
		CEREAL_NVP(component_name),
		CEREAL_NVP(texture),
		CEREAL_NVP(pivot_type),
		CEREAL_NVP(src_pivot),
		CEREAL_NVP(src_width),
		CEREAL_NVP(src_height),
		CEREAL_NVP(variable_texcoords),
		CEREAL_NVP(variable_width),
		CEREAL_NVP(variable_height),
		CEREAL_NVP(color),
		CEREAL_NVP(image_size)
	);
}

void SpriteComponent::Load(Entity* entity, std::string file_path_and_name)
{
	{
		std::ifstream ifs(file_path_and_name);
		cereal::JSONInputArchive i_archive(ifs);
		i_archive(
			CEREAL_NVP(component_name),
			CEREAL_NVP(texture),
			CEREAL_NVP(pivot_type),
			CEREAL_NVP(src_pivot),
			CEREAL_NVP(src_width),
			CEREAL_NVP(src_height),
			CEREAL_NVP(variable_texcoords),
			CEREAL_NVP(variable_width),
			CEREAL_NVP(variable_height),
			CEREAL_NVP(color),
			CEREAL_NVP(image_size)
		);

		this->entity = entity;
	}

	texture = Locator::GetResourceManager().GetTextureResouece(texture->GetTextureData()->filename);

	variable_texcoords.at(0) = { .0f,.0f };
	variable_texcoords.at(1) = { 1.f,.0f };
	variable_texcoords.at(2) = { .0f,1.f };
	variable_texcoords.at(3) = { 1.f,1.f };

	AjustSrcTexturePivot(pivot_type, 0, 0, src_width, src_height);
	CreateVertexBuffer(entity->GetScene()->GetFramework()->GetDevice());
}

