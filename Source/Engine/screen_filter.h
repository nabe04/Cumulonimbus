#pragma once
#include "cbuffer_post_effect.h"
#include "constant_buffer.h"
#include "post_effect.h"

namespace cumulonimbus::post_effect
{
	class ScreenFilter final : public PostEffect
	{
	public:
		explicit ScreenFilter() = default;
		explicit ScreenFilter(ID3D11Device* device);
		~ScreenFilter() override = default;

		void Initialize(ID3D11Device* device) override;

		void RenderImGui(ecs::Registry* registry) override;

		template<class Archive>
		void load(Archive&& archive, uint32_t version)
		{
			archive(
				cereal::base_class<PostEffect>(this),
				CEREAL_NVP(cb_screen_filter),
				CEREAL_NVP(dissolve_tex_id)
			);
		}

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const
		{
			archive(
				cereal::base_class<PostEffect>(this),
				CEREAL_NVP(cb_screen_filter),
				CEREAL_NVP(dissolve_tex_id)
			);
		}

		void AddDissolveThreshold(float arg) const;

		[[nodiscard]]
		buffer::ConstantBuffer<ScreenFilterCB>& GetCBuffer() const
		{
			return *(cb_screen_filter.get());
		}

		void SetIsGrayScale(const float arg)
		{
			is_gray_scale = arg;
		}
	private:
		std::unique_ptr<buffer::ConstantBuffer<ScreenFilterCB>> cb_screen_filter{};
		bool is_gray_scale{ false };
		mapping::rename_type::UUID dissolve_tex_id{};

		void Begin(ID3D11DeviceContext* immediate_context) const override;
		void End(ID3D11DeviceContext* immediate_context) const override;
	};
} // cumulonimbus::post_effect

CEREAL_REGISTER_TYPE(cumulonimbus::post_effect::ScreenFilter)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::post_effect::PostEffect, cumulonimbus::post_effect::ScreenFilter)
CEREAL_CLASS_VERSION(cumulonimbus::post_effect::ScreenFilter, 0)
