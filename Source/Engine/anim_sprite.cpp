#include "anim_sprite.h"

#include "scene.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::OldAnimSpriteComponent);
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::OldSpriteComponent, cumulonimbus::component::OldAnimSpriteComponent)

namespace cumulonimbus::component
{
	template <class Archive>
	void OldAnimSpriteComponent::serialize(Archive&& archive)
	{
		archive(
			cereal::base_class<OldSpriteComponent>(this),
			CEREAL_NVP(clip_texcoord),
			CEREAL_NVP(num_clip),
			CEREAL_NVP(anim_texcoord),
			CEREAL_NVP(anim_pivot)
		);
	}


	OldAnimSpriteComponent::OldAnimSpriteComponent(ecs::Registry* const registry, const  mapping::rename_type::Entity ent,
		ID3D11Device* device,
		const char* filename, const PivotType pivot_type,
		const int src_left,   const int src_top,
		const int src_width,  const int src_height,
		const int num_clip_w, const int num_clip_h)
		:OldSpriteComponent{ registry, ent, device, filename, pivot_type, src_left, src_top, src_width, src_height }
	{
		num_clip.x = num_clip_w;
		num_clip.y = num_clip_h;

		CreateAnim(num_clip_w, num_clip_h);
		AdjustAnimPivot(pivot_type, src_left, src_top, src_width / num_clip.x, src_height / num_clip.y);
		SetAnimation(0, 0);
	}

	void OldAnimSpriteComponent::CreateAnim(const int num_clip_w, const int num_clip_h)
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

	void OldAnimSpriteComponent::AdjustAnimPivot(PivotType pivot, const int left, const int top, const int width, const int height)
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

	void OldAnimSpriteComponent::SetAnimation(size_t clip_w, size_t clip_h)
	{
		const size_t h = num_clip.x * clip_h;

		anim_texcoord.at(TexSide::LeftTop) = clip_texcoord.at(TexSide::LeftTop).at(h + clip_w);
		anim_texcoord.at(TexSide::RightTop) = clip_texcoord.at(TexSide::RightTop).at(h + clip_w);
		anim_texcoord.at(TexSide::LeftBottom) = clip_texcoord.at(TexSide::LeftBottom).at(h + clip_w);
		anim_texcoord.at(TexSide::RightBottom) = clip_texcoord.at(TexSide::RightBottom).at(h + clip_w);
	}

	void OldAnimSpriteComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}

} // cumulonimbus::component

