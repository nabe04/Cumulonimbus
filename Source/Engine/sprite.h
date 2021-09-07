#pragma once
#include <array>
#include <memory>

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>

#include "component_base.h"
#include "cereal_helper.h"
#include "texture.h"
#include "rename_type_mapping.h"

enum class PivotType
{
	Center,
	BottomCenter,
	TopCenter,
	LeftTop,
	LeftBottom,
	RightTop,
	RightBottom,
};

namespace cumulonimbus::component
{
	class SpriteComponent : public ComponentBase
	{
	public:
		explicit SpriteComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent,
		                         ID3D11Device* device,
		                         const char* filename, PivotType pivot_type,
		                         int src_left, int src_top,
		                         int src_width, int src_height);
		explicit SpriteComponent() = default;	// For cereal
		~SpriteComponent() override = default;

		void NewFrame(const float delta_time)override {};
		void Update(const float delta_time) override;
		void RenderImGui() override;

		void Load(ecs::Registry* registry) override;


		[[nodiscard]] int VariableWidth()  const { return variable_width; }
		[[nodiscard]] int VariableHeight() const { return variable_height; }
		[[nodiscard]] const DirectX::XMFLOAT4& Color() const { return color; }

		void  VariableWidth(int width) { variable_width = width; }
		void  VariableHeight(int height) { variable_height = height; }
		void  Color(const DirectX::XMFLOAT4& color) { this->color = color; }

		[[nodiscard]] const std::array<DirectX::XMFLOAT2, 4>& GetVariableTexcoords() const { return variable_texcoords; }
		[[nodiscard]] const DirectX::XMFLOAT2& GetSrcPivot() const { return src_pivot; }
		[[nodiscard]] ID3D11Buffer* GetVertexBuffer() const { return vertex_buffer.Get(); }
		[[nodiscard]] ID3D11Buffer** GetVertexBufferAddress() { return vertex_buffer.GetAddressOf(); }
		[[nodiscard]] TextureResource* GetTexture() const { return texture.get(); }

		template<class Archive>
		void serialize(Archive&& archive)
		{
			archive(
				cereal::base_class<ComponentBase>(this),
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

	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;

		std::shared_ptr<TextureResource> texture;

		PivotType			pivot_type{};
		DirectX::XMFLOAT2	src_pivot{};
		int					src_width{};
		int					src_height{};

		std::array<DirectX::XMFLOAT2, 4> variable_texcoords{};
		int variable_width{};
		int variable_height{};

		DirectX::XMFLOAT4 color{ 1.f,1.f,1.f,1.f };

		// For Imgui
		DirectX::XMFLOAT2 image_size = { 100,100 };

		void AdjustSrcTexturePivot(const PivotType pivot_type,
			const int src_left, const int src_top,
			const int src_width, const int src_height);

		void CreateVertexBuffer(ID3D11Device* device);
	};
}

CEREAL_REGISTER_TYPE(cumulonimbus::component::SpriteComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::SpriteComponent)
