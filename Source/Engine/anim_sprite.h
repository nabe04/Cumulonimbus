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

public:
	explicit AnimSpriteComponent(Entity* entity, ID3D11Device* device,
		const char* filename, const PivotType pivot_type,
		const int src_left, const int src_top,
		const int src_width, const int src_height,
		const int num_clip_w, const int num_clip_h);
	explicit AnimSpriteComponent()  = default; // for cereal
	~AnimSpriteComponent() override = default;

	/*
	* brief : アニメーション指定
	* clip : 指定位置(0スタート)
	*/
	void SetAnimation(size_t clip_w, size_t clip_h);

	[[nodiscard]] const DirectX::XMFLOAT2& GetAnimPivot() const { return anim_pivot; }
	[[nodiscard]] const std::array<DirectX::XMFLOAT2, 4>& GetAnimTexcoord() const { return anim_texcoord; }
	[[nodiscard]] const DirectX::XMINT2& GetNumClip() const { return num_clip; }

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

private:
	using Texcoord = std::vector<DirectX::XMFLOAT2>;
	//std::vector<DirectX::XMFLOAT2> clip_texcoord;	// Animation用のtexcoord(横優先)
	std::array<Texcoord, 4> clip_texcoord;	// Animation用のtexcoord(横優先)

	DirectX::XMINT2 num_clip;
	std::array<DirectX::XMFLOAT2, 4> anim_texcoord{};

	DirectX::XMFLOAT2 anim_pivot;

	void CreateAnim(const int num_clip_w, const int num_clip_h);
	void AdjustAnimPivot(PivotType pivot, const int left, const int top, const int width, const int height);

};

CEREAL_REGISTER_TYPE(AnimSpriteComponent);
CEREAL_REGISTER_POLYMORPHIC_RELATION(SpriteComponent, AnimSpriteComponent)


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
		explicit AnimSpriteComponent(ecs::Registry* const registry, const ecs::Entity ent,
			ID3D11Device* device,
			const char* filename, PivotType pivot_type,
			int src_left,   int src_top,
			int src_width,  int src_height,
			int num_clip_w, int num_clip_h);
		explicit AnimSpriteComponent() = default; // for cereal
		~AnimSpriteComponent() override = default;

		void NewFrame(const float delta_time)override {};
		void Update(float delta_time) override {};
		void RenderImGui() override {};

		void Save(const std::string& file_path) override;
		void Load(const std::string& file_path_and_name) override;

		/*
		* brief : アニメーション指定
		* clip : 指定位置(0スタート)
		*/
		void SetAnimation(size_t clip_w, size_t clip_h);

		[[nodiscard]] const DirectX::XMFLOAT2& GetAnimPivot() const { return anim_pivot; }
		[[nodiscard]] const std::array<DirectX::XMFLOAT2, 4>& GetAnimTexcoord() const { return anim_texcoord; }
		[[nodiscard]] const DirectX::XMINT2& GetNumClip() const { return num_clip; }

		template <class Archive>
		void serialize(Archive&& archive)
		{
			archive(
				cereal::base_class<SpriteComponent>(this),
				CEREAL_NVP(clip_texcoord),
				CEREAL_NVP(num_clip),
				CEREAL_NVP(anim_texcoord),
				CEREAL_NVP(anim_pivot)
			);
		}

	private:
		using Texcoord = std::vector<DirectX::XMFLOAT2>;
		std::array<Texcoord, 4> clip_texcoord{};	// Animation用のtexcoord(横優先)

		DirectX::XMINT2 num_clip{};
		std::array<DirectX::XMFLOAT2, 4> anim_texcoord{};

		DirectX::XMFLOAT2 anim_pivot{};

		void CreateAnim(const int num_clip_w, const int num_clip_h);
		void AdjustAnimPivot(PivotType pivot, const int left, const int top, const int width, const int height);
	};
}

CEREAL_REGISTER_TYPE(cumulonimbus::component::AnimSpriteComponent);
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::SpriteComponent, cumulonimbus::component::AnimSpriteComponent)