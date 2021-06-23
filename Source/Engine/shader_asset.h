#pragma once

#include <cereal/cereal.hpp>

namespace cumulonimbus::shader_asset
{
	class ShaderAsset
	{
	public:
		explicit ShaderAsset() = default;
		virtual ~ShaderAsset() = default;

		virtual void BindCBuffer()	 = 0;
		virtual void UnbindCBuffer() = 0;
		virtual void RenderImGui()	 = 0;

		template <typename Archive>
		void serialize(Archive&& archive){}
	};
}
