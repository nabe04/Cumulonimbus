#pragma once

#include <array>
#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>

#include "cereal/cereal.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/array.hpp"
#include "cereal_helper.h"

#include "ecs.h"
#include "sprite.h"

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

private:
	using Texcoord = std::vector<DirectX::XMFLOAT2>;
	//std::vector<DirectX::XMFLOAT2> clip_texcoord;	// Animation用のtexcoord(横優先)
	std::array<Texcoord, 4> clip_texcoord;	// Animation用のtexcoord(横優先)

	DirectX::XMINT2 num_clip;
	std::array<DirectX::XMFLOAT2, 4> anim_texcoord{};

	DirectX::XMFLOAT2 anim_pivot;

private:
	void CreateAnim(const int num_clip_w, const int num_clip_h);
	void AdjustAnimPivot(PivotType pivot, const int left, const int top, const int width, const int height);

public:
	explicit AnimSpriteComponent(Entity* entity, ID3D11Device* device,
		const char* filename, const PivotType pivot_type,
		const int src_left, const int src_top,
		const int src_width, const int src_height,
		const int num_clip_w, const int num_clip_h);
	AnimSpriteComponent() = default; // for cereal

	/*
	* brief : アニメーション指定
	* clip : 指定位置(0スタート)
	*/
	void SetAnimation(size_t clip_w, size_t clip_h);

	const DirectX::XMFLOAT2& GetAnimPivot() { return anim_pivot; }
	const std::array<DirectX::XMFLOAT2, 4>& GetAnimTexcoord() { return anim_texcoord; }
	const DirectX::XMINT2& GetNumClip() { return num_clip; }

	void Save(std::string file_path) override;
	void Load(Entity* entity, std::string file_path_and_name) override;

	template <class Archive>
	void serialize(Archive&& archive)
	{
		archive(
			cereal::base_class<SpriteComponent>(this),
			CEREAL_NVP(component_name),
			CEREAL_NVP(clip_texcoord),
			CEREAL_NVP(num_clip),
			CEREAL_NVP(anim_texcoord),
			CEREAL_NVP(anim_pivot)
		);
	}

	~AnimSpriteComponent() = default;
};