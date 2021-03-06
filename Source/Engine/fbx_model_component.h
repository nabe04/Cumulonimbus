//#pragma once
//
//#include <memory>
//#include <vector>
//
//#include <d3d11.h>
//#include <DirectXMath.h>
//#include <SimpleMath.h>
//
//#include <cereal/cereal.hpp>
//#include <cereal/types/polymorphic.hpp>
//
//#include "component_base.h"
//#include "model_data.h"
//#include "rename_type_mapping.h"
//#include "shader_asset_mapping.h"
//#include "shader_interop_renderer.h"
//#include "state_machine.h"
//
//class FbxModelResource;
//
////namespace shader
////{
////	class MeshShaderState;
////	enum class MeshShaderTypes;
////} // shader
//
//namespace cumulonimbus
//{
//	namespace shader_asset
//	{
//		class Material3DManager;
//		class PBRMaterial;
//	} // shader_asset
//} // cumulonimbus
//
//enum class FbxAnimationState
//{
//	Switch,
//	Blending,
//	Update,
//};
//
//namespace cumulonimbus::component
//{
//	class FbxModelComponent final : public ComponentBase
//	{
//	public:
//		struct Node
//		{
//			std::string					name{};
//			int							parent_index{ -1 };
//			//Node*						parent{};
//			DirectX::XMFLOAT3			scale{};
//			DirectX::XMFLOAT4			rotate{};
//			DirectX::XMFLOAT3			translate{};
//			DirectX::SimpleMath::Matrix	local_transform{};
//			DirectX::SimpleMath::Matrix	world_transform{};
//
//			template<typename Archive>
//			void serialize(Archive&& archive)
//			{
//				archive(
//					cereal::make_nvp("node_name", name),
//					CEREAL_NVP(parent_index),
//					//cereal::make_nvp("parent node", parent),
//					CEREAL_NVP(scale),
//					CEREAL_NVP(rotate),
//					CEREAL_NVP(translate),
//					CEREAL_NVP(local_transform),
//					CEREAL_NVP(world_transform)
//				);
//			}
//		};
//		using ComponentBase::ComponentBase;
//		explicit FbxModelComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const std::shared_ptr<FbxModelResource>& resource);
//		explicit FbxModelComponent()  = default; // for cereal
//		~FbxModelComponent() override = default;
//
//		void PreGameUpdate(float delta_time) override;
//		void GameUpdate(float delta_time) override;
//		void RenderImGui() override;
//
//		/**
//		 * @brief		: モデルのマテリアル全てのシェーダーのセット
//		 * @param asset	: 使用するシェーダーの種類１
//		 */
//		void SetAllShader(mapping::shader_assets::ShaderAsset3D asset);
//
//		/**
//		 * @brief					: 指定された要素のマテリアル情報(シェーダー、シェーダーパラメータ、テクスチャ)の取得
//		 * @param material_index	: メッシュのsubsetが持つmaterial_indexを指定
//		 */
//		[[nodiscard]] const shader_asset::Material3DManager* GetMaterialsManager(uint material_index) const;
//		/**
//		 * @brief					: 指定されて要素のPBRマテリアル情報の取得
//		 * @param material_index	: メッシュのsubsetが持つmaterial_indexを指定
//		 */
//		[[nodiscard]] const shader_asset::PBRMaterial* GetPBRMaterial(uint material_index) const;
//
//		/**
//		 * @brief					: 指定された要素のPBRマテリアルの持つコンスタントバッファのバインド
//		 *							  CBufferスロット番号->CBSlot_Material(4)
//		 * @param  material_index	: メッシュのsubsetが持つmaterial_index
//		 */
//		void BindPBRMaterialCBuff(uint material_index) const;
//		void UnbindPBRMaterialCBuff(uint material_index) const;
//		/**
//		 * @brief					: 指定された要素のPBRマテリアルの持つテクスチャのバインド
//		 *							  Textureスロット番号->TexSlot_BaseColorMap(20)
//		 *												   TexSlot_NormalMap(21)
//		 *												   TexSlot_PBRMap(32)
//		 *												   TexSlot_RoughnessMap(27)
//		 *												   TexSlot_SpecularMap(28)
//		 *												   TexSlot_AOMap(33)
//		 * @param material_index	: メッシュのsubsetが持つmaterial_index
//		 */
//		void BindPBRMaterialTexture(uint material_index) const;
//		void UnbindPBRMaterialTexture(uint material_index) const;
//		/**
//		 * @brief					:  指定された要素のPBRマテリアルの持つ
//		 *							   コンスタントバッファ、テクスチャのバインド
//		 * @param material_index	: メッシュのsubsetが持つmaterial_index
//		 */
//		void BindPBRMaterialCBuffAndTexture(uint material_index) const;
//		void UnbindPBRMaterialCBuffAndTexture(uint material_index) const;
//
//		// アニメーション
//		[[nodiscard]] bool IsPlayAnimation() const;
//		/**
//		 * @brief					: アニメーションの遷移
//		 * @param animation_index	: 格納されているアニメーションの要素番号
//		 * @param loop				: ループ再生をするか
//		 * @param switch_time		: 次のアニメーションに遷移するまでの時間
//		 */
//		void SwitchAnimation(int animation_index, bool loop = false, float switch_time = 0.1f);
//
//		[[nodiscard]] const DirectX::SimpleMath::Matrix& GetNodeMatrix(const char* node_name);
//		[[nodiscard]] const DirectX::SimpleMath::Matrix& GetNodeParentMatrix(const char* node_name);
//		[[nodiscard]] const std::vector<Node>&			 GetNodes()		const	{ return nodes; }
//		[[nodiscard]] std::vector<Node>&				 GetNodes()				{ return nodes; }
//		[[nodiscard]] const FbxModelResource*			 GetResource()	const	{ return resource.get(); }
//		[[nodiscard]]	    FbxModelResource*			 GetResource()			{ return resource.get(); }
//		[[nodiscard]] const DirectX::XMFLOAT4&			 GetColor()		const	{ return color; }
//		//[[nodiscard]] const ModelData::Material
//
//		void UpdateAnimState(const float delta_time);
//
//		// 現在のキーフレーム
//		[[nodiscard]] int CurrentKeyframe() const { return current_keyframe; }
//
//		void Load(ecs::Registry* registry) override;
//
//		//// For cereal
//		//template<typename Archive>
//		//void serialize(Archive&& archive)
//		//{
//		//	archive(
//		//		cereal::base_class<ComponentBase>(this),
//
//		//		CEREAL_NVP(resource),
//		//		CEREAL_NVP(nodes),
//
//		//		CEREAL_NVP(current_animation_index),
//		//		CEREAL_NVP(prev_animation_index),
//		//		CEREAL_NVP(current_keyframe),
//		//		CEREAL_NVP(prev_key_index),
//
//		//		CEREAL_NVP(current_seconds),
//		//		CEREAL_NVP(prev_seconds),
//		//		CEREAL_NVP(animation_switch_time),
//		//		CEREAL_NVP(changer_timer),
//
//		//		CEREAL_NVP(loop_animation),
//		//		CEREAL_NVP(end_animation),
//
//		//		CEREAL_NVP(prev_animation),
//		//		CEREAL_NVP(anim_states)
//		//	);
//		//}
//
//		template<class Archive>
//		void load(Archive&& archive, uint32_t version)
//		{
//			archive(
//				cereal::base_class<ComponentBase>(this),
//
//				CEREAL_NVP(resource),
//				CEREAL_NVP(nodes),
//
//				CEREAL_NVP(current_animation_index),
//				CEREAL_NVP(prev_animation_index),
//				CEREAL_NVP(current_keyframe),
//				CEREAL_NVP(prev_key_index),
//
//				CEREAL_NVP(current_seconds),
//				CEREAL_NVP(prev_seconds),
//				CEREAL_NVP(animation_switch_time),
//				CEREAL_NVP(changer_timer),
//
//				CEREAL_NVP(loop_animation),
//				CEREAL_NVP(end_animation),
//
//				CEREAL_NVP(prev_animation),
//				CEREAL_NVP(anim_states)
//			);
//		}
//
//		template<class Archive>
//		void save(Archive&& archive, uint32_t version) const
//		{
//			archive(
//				cereal::base_class<ComponentBase>(this),
//
//				CEREAL_NVP(resource),
//				CEREAL_NVP(nodes),
//
//				CEREAL_NVP(current_animation_index),
//				CEREAL_NVP(prev_animation_index),
//				CEREAL_NVP(current_keyframe),
//				CEREAL_NVP(prev_key_index),
//
//				CEREAL_NVP(current_seconds),
//				CEREAL_NVP(prev_seconds),
//				CEREAL_NVP(animation_switch_time),
//				CEREAL_NVP(changer_timer),
//
//				CEREAL_NVP(loop_animation),
//				CEREAL_NVP(end_animation),
//
//				CEREAL_NVP(prev_animation),
//				CEREAL_NVP(anim_states)
//			);
//		}
//
//	private:
//		std::shared_ptr<FbxModelResource>	resource{nullptr};
//		std::vector<Node>					nodes{};
//
//		std::vector <std::shared_ptr<shader_asset::PBRMaterial>>		pbr_materials;	   // mesh単位でマテリアル(テクスチャなど)を変更できるように
//		std::vector<std::shared_ptr<shader_asset::Material3DManager>>	materials_manager; // mesh単位でマテリアル(テクスチャなど)を変更できるように
//
//		int	current_animation_index = -1;
//		int	prev_animation_index	= -1;	// 前のアニメーションのインデックス番号(ブレンドで使用)
//		int	current_keyframe		= 0;	// 現在のキーフレーム
//		int	prev_key_index			= 0;
//
//		float current_seconds		= 0.0f;
//		float prev_seconds			= 0.0f; // アニメーションが切り替わった時点の前のアニメーションのキーフレーム(ブレンドで使用)
//		float animation_switch_time = 0;
//		float changer_timer			= 0;
//
//		bool loop_animation			= false; // アニメーションのループ再生
//		bool end_animation			= false;
//
//
//		ModelData::Animation prev_animation;
//		StateMachine<FbxAnimationState, void, const float>	anim_states;
//
//		DirectX::XMFLOAT4 color = { 0.8f, 0.8f, 0.8f, 1.0f };
//
//		void InitializeParameter();
//		void Initialize(const std::shared_ptr<FbxModelResource>& resource, ecs::Registry* registry, mapping::rename_type::Entity ent);
//
//		// 前のアニメーションのキーフレームを算出
//		// brief : アニメーションが切り替わる時に使用
//		[[nodiscard]]
//		int CalcPrevKeyframe(int key_index);
//
//		// 行列計算
//		void CalculateLocalTransform();
//		void CalculateWorldTransform(const DirectX::XMFLOAT4X4& world_transform);
//
//		void BlendNextAnimation(float dt);
//		void UpdateAnimation(float dt);
//	};
//} // cumulonimbus::component
//
//CEREAL_REGISTER_TYPE(cumulonimbus::component::FbxModelComponent)
//CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::FbxModelComponent)
//CEREAL_CLASS_VERSION(cumulonimbus::component::FbxModelComponent, 0)
