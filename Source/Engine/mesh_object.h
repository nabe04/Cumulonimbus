//#pragma once
//
//#include <memory>
//#include <wrl.h>
//
//#include <d3d11.h>
//#include <DirectXMath.h>
//
//#include <cereal/cereal.hpp>
//#include <cereal/types/polymorphic.hpp>
//
//#include "blend.h"
//#include "component_base.h"
//#include "depth_stencil.h"
//#include "enum_state_map.h"
//#include "rasterizer.h"
//#include "rename_type_mapping.h"
//#include "rendering_buffer.h"
//#include "sampler.h"
//#include "shader_manager.h"
//
//namespace cumulonimbus::component
//{
//	//class MeshObjectComponent :public ComponentBase
//	//{
//	//protected:
//	//	//-- States of DirectX --//
//	//	EnumStateMap<BlendState>			blend_state{};
//	//	EnumStateMap<RasterizeState>		rasterizer_state{};
//	//	EnumStateMap<RenderingSampleState>	sampler_state{};
//	//	EnumStateMap<DepthStencilState>		depth_stencil_state{};
//
//	//	shader::MeshShaderState	shader_state;
//	//	RenderingBufferBitset	rendering_buffer;
//
//	//	bool is_visible = true;	// �`�悷�邩
//
//	//public:
//	//	explicit MeshObjectComponent() = default; // for cereal
//	//	explicit MeshObjectComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
//	//	~MeshObjectComponent() override = default;
//
//	//	void PreGameUpdate(const float delta_time)override {};
//	//	void GameUpdate(const float delta_time)override {};
//	//	void RenderImGui() override;
//
//	//	void Load(ecs::Registry* registry) override;
//
//	//	[[nodiscard]] auto GetBlendState()		    const { return blend_state.GetCurrentState(); }
//	//	[[nodiscard]] auto GetRasterizerState()	    const { return rasterizer_state.GetCurrentState(); }
//	//	[[nodiscard]] auto GetDepthStencilState()   const { return depth_stencil_state.GetCurrentState(); }
//	//	[[nodiscard]] auto GetSamplerState()		const { return sampler_state.GetCurrentState(); }
//	//	[[nodiscard]] auto GetShaderState()		    const { return shader_state; }
//	//	[[nodiscard]] auto GetIsVisible()			const { return is_visible; }
//
//	//	void SetBlendState(const BlendState& state) { blend_state.SetState(state); }
//	//	void SetRasterizerState(const RasterizeState& state) { rasterizer_state.SetState(state); }
//	//	void SetDepthStencilState(const DepthStencilState& state) { depth_stencil_state.SetState(state); }
//	//	void SetSamplerState(const RenderingSampleState& state) { sampler_state.SetState(state); }
//	//	void SetShaderState(shader::MeshShaderTypes type) { shader_state.SetShaderState(type); }
//	//	void SetIsVisible(const bool visible) { is_visible = visible; }
//
//	//	[[nodiscard]] RenderingBufferBitset* UsingBuffer() { return &rendering_buffer; }
//
//	//	template<class Archive>
//	//	void load(Archive&& archive, uint32_t  version)
//	//	{
//	//		archive(
//	//			cereal::base_class<ComponentBase>(this),
//	//			cereal::make_nvp("blend state", blend_state),
//	//			cereal::make_nvp("rasterizer state", rasterizer_state),
//	//			cereal::make_nvp("sampler state", sampler_state),
//	//			cereal::make_nvp("depth stencil_state", depth_stencil_state),
//
//	//			cereal::make_nvp("shader state", shader_state),
//	//			cereal::make_nvp("rendering_buffer", rendering_buffer)
//	//		);
//	//	}
//
//	//	template<class Archive>
//	//	void save(Archive&& archive, uint32_t version) const
//	//	{
//	//		archive(
//	//			cereal::base_class<ComponentBase>(this),
//	//			cereal::make_nvp("blend state", blend_state),
//	//			cereal::make_nvp("rasterizer state", rasterizer_state),
//	//			cereal::make_nvp("sampler state", sampler_state),
//	//			cereal::make_nvp("depth stencil_state", depth_stencil_state),
//
//	//			cereal::make_nvp("shader state", shader_state),
//	//			cereal::make_nvp("rendering_buffer", rendering_buffer)
//	//		);
//	//	}
//	//};
//}
//
////CEREAL_REGISTER_TYPE(cumulonimbus::component::MeshObjectComponent)
////CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::MeshObjectComponent)
////CEREAL_CLASS_VERSION(cumulonimbus::component::MeshObjectComponent, 0)