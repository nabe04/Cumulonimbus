#include "anim_sprite.h"

#include <cereal/types/vector.hpp>
#include <cereal/types/array.hpp>
#include <cereal_helper.h>

#include "texture.h"
#include "scene.h"
#include "locator.h"
#include "file_path_helper.h"

namespace cumulonimbus::component
{
	AnimSpriteComponent::AnimSpriteComponent(ecs::Registry* const registry, const ecs::Entity ent,
		ID3D11Device* device,
		const char* filename, const PivotType pivot_type,
		const int src_left,   const int src_top,
		const int src_width,  const int src_height,
		const int num_clip_w, const int num_clip_h)
		:SpriteComponent{ registry, ent, device, filename, pivot_type, src_left, src_top, src_width, src_height }
	{
		num_clip.x = num_clip_w;
		num_clip.y = num_clip_h;

		CreateAnim(num_clip_w, num_clip_h);
		AdjustAnimPivot(pivot_type, src_left, src_top, src_width / num_clip.x, src_height / num_clip.y);
		SetAnimation(0, 0);
	}

	void AnimSpriteComponent::CreateAnim(const int num_clip_w, const int num_clip_h)
	{
		const float rate_w = 1.0f / static_cast<float>(num_clip_w);
		const float rate_h = 1.0f / static_cast<float>(num_clip_h);

		for (auto& it : clip_texcoord)
		{
			it.resize(static_cast<size_t>(num_clip_w) * static_cast<size_t>(num_clip_h));
		}

		//clip_texcoord.resize(static_cast<size_t>(num_clip_w * num_clip_h));

		DirectX::XMFLOAT2 texcoord{};

		for (size_t h = 0; h < num_clip_h; ++h)
		{
			texcoord.x = 0;

			for (size_t w = 0; w < num_clip_w; ++w)
			{
				// left top
				clip_texcoord.at(TexSide::LeftTop).at(w + h * num_clip_w).x = texcoord.x;
				clip_texcoord.at(TexSide::LeftTop).at(w + h * num_clip_w).y = texcoord.y;
				// right top
				clip_texcoord.at(TexSide::RightTop).at(w + h * num_clip_w).x = texcoord.x + rate_w;
				clip_texcoord.at(TexSide::RightTop).at(w + h * num_clip_w).y = texcoord.y;
				// left bottom
				clip_texcoord.at(TexSide::LeftBottom).at(w + h * num_clip_w).x = texcoord.x;
				clip_texcoord.at(TexSide::LeftBottom).at(w + h * num_clip_w).y = texcoord.y + rate_h;
				// right bottom
				clip_texcoord.at(TexSide::RightBottom).at(w + h * num_clip_w).x = texcoord.x + rate_w;
				clip_texcoord.at(TexSide::RightBottom).at(w + h * num_clip_w).y = texcoord.y + rate_h;


				texcoord.x += rate_w;
			}

			texcoord.y += rate_h;
		}
	}

	void AnimSpriteComponent::AdjustAnimPivot(PivotType pivot, const int left, const int top, const int width, const int height)
	{
		switch (pivot)
		{
		case PivotType::Center:
			anim_pivot.x = static_cast<float>(width / 2);
			anim_pivot.y = static_cast<float>(height / 2);
			break;
		case PivotType::BottomCenter:
			anim_pivot.x = static_cast<float>(width / 2);
			anim_pivot.y = static_cast<float>(height);
			break;
		case PivotType::TopCenter:
			anim_pivot.x = static_cast<float>(width / 2);
			anim_pivot.y = static_cast<float>(top);
			break;
		case PivotType::LeftTop:
			anim_pivot.x = static_cast<float>(left);
			anim_pivot.y = static_cast<float>(top);
			break;
		case PivotType::LeftBottom:
			anim_pivot.x = static_cast<float>(left);
			anim_pivot.y = static_cast<float>(height);
			break;
		case PivotType::RightTop:
			anim_pivot.x = static_cast<float>(width);
			anim_pivot.y = static_cast<float>(top);
			break;
		case PivotType::RightBottom:
			anim_pivot.x = static_cast<float>(width);
			anim_pivot.y = static_cast<float>(height);
			break;
		}
	}

	void AnimSpriteComponent::SetAnimation(size_t clip_w, size_t clip_h)
	{
		const size_t h = num_clip.x * clip_h;

		anim_texcoord.at(TexSide::LeftTop) = clip_texcoord.at(TexSide::LeftTop).at(h + clip_w);
		anim_texcoord.at(TexSide::RightTop) = clip_texcoord.at(TexSide::RightTop).at(h + clip_w);
		anim_texcoord.at(TexSide::LeftBottom) = clip_texcoord.at(TexSide::LeftBottom).at(h + clip_w);
		anim_texcoord.at(TexSide::RightBottom) = clip_texcoord.at(TexSide::RightBottom).at(h + clip_w);
	}

	void AnimSpriteComponent::Save(const std::string& file_path)
	{
		const std::string file_path_and_name = file_path + file_path_helper::GetTypeName<AnimSpriteComponent>() + file_path_helper::GetJsonExtension();
		std::ofstream ofs(file_path_and_name);
		cereal::JSONOutputArchive o_archive(ofs);
		o_archive(*this);
	}

	void AnimSpriteComponent::Load(const std::string& file_path_and_name)
	{
		{
			std::ifstream ifs(file_path_and_name);
			cereal::JSONInputArchive i_archive(ifs);
			i_archive(*this);

			texture = Locator::GetResourceManager()->GetTextureResouece(texture->GetTextureData()->filename);

			variable_texcoords.at(0) = { .0f,.0f };
			variable_texcoords.at(1) = { 1.f,.0f };
			variable_texcoords.at(2) = { .0f,1.f };
			variable_texcoords.at(3) = { 1.f,1.f };

			AdjustSrcTexturePivot(pivot_type, 0, 0, src_width, src_height);
			CreateVertexBuffer(GetRegistry()->GetScene()->GetFramework()->GetDevice());
		}
	}

}

