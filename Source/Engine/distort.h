#pragma once
#include "cbuffer_post_effect.h"
#include "constant_buffer.h"
#include "post_effect.h"

namespace cumulonimbus::post_effect
{
	class Distort final : public PostEffect
	{
	public:
		explicit Distort() = default;
		explicit Distort(ID3D11Device* device);
		~Distort() override = default;

		void Initialize(ID3D11Device* device) override;

		void RenderImGui(ecs::Registry* registry) override;


		template<class Archive>
		void load(Archive&& archive, uint32_t version)
		{
			archive(
				cereal::base_class<PostEffect>(this),
				CEREAL_NVP(cb_distort)
			);
		}

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const
		{
			archive(
				cereal::base_class<PostEffect>(this),
				CEREAL_NVP(cb_distort)
			);
		}

		[[nodiscard]]
		buffer::ConstantBuffer<DistortCB>& GetCBuffer() const
		{
			return *(cb_distort.get());
		}
	private:
		std::unique_ptr<buffer::ConstantBuffer<DistortCB>> cb_distort{};

		void Begin(ID3D11DeviceContext* immediate_context) const override;
		void End(ID3D11DeviceContext* immediate_context) const override;
	};
} // cumulonimbus::post_effect

CEREAL_REGISTER_TYPE(cumulonimbus::post_effect::Distort)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::post_effect::PostEffect, cumulonimbus::post_effect::Distort)
CEREAL_CLASS_VERSION(cumulonimbus::post_effect::Distort, 0)