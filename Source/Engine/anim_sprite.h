#pragma once

#include <array>
#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>

#include "sprite.h"
#include "rename_type_mapping.h"

namespace cumulonimbus::component
{
	class AnimSpriteComponent final :public SpriteComponent
	{
	public:
		enum TexSide
		{
			LeftTop,
			RightTop,
			LeftBottom,
			RightBottom,
		};

	public:
		explicit AnimSpriteComponent(ecs::Registry* registry, mapping::rename_type::Entity ent,
									 ID3D11Device* device,
									 const char* filename, PivotType pivot_type,
									 int src_left,   int src_top,
									 int src_width,  int src_height,
									 int num_clip_w, int num_clip_h);
		explicit AnimSpriteComponent() = default; // for cereal
		~AnimSpriteComponent() override = default;

		void PreGameUpdate(const float delta_time)override {};
		void GameUpdate(float delta_time) override {};
		void RenderImGui() override {};

		void Load(ecs::Registry* registry) override;

		/*
		* brief : アニメーション指定
		* clip : 指定位置(0スタート)
		*/
		void SetAnimation(size_t clip_w, size_t clip_h);

		[[nodiscard]] const DirectX::XMFLOAT2& GetAnimPivot() const { return anim_pivot; }
		[[nodiscard]] const std::array<DirectX::XMFLOAT2, 4>& GetAnimTexcoord() const { return anim_texcoord; }
		[[nodiscard]] const DirectX::XMINT2& GetNumClip() const { return num_clip; }

		template <class Archive>
		void serialize(Archive&& archive);
	private:
		using Texcoord = std::vector<DirectX::XMFLOAT2>;
		std::array<Texcoord, 4>			 clip_texcoord{};	// Animation用のtexcoord(横優先)
		std::array<DirectX::XMFLOAT2, 4> anim_texcoord{};
		DirectX::XMINT2					 num_clip{};
		DirectX::XMFLOAT2				 anim_pivot{};

		void CreateAnim(const int num_clip_w, const int num_clip_h);
		void AdjustAnimPivot(PivotType pivot, const int left, const int top, const int width, const int height);
	};
} // cumulonimbus::component