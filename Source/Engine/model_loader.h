#pragma once
#include <fbxsdk.h>

#include "loader.h"
#include "model.h"

namespace cumulonimbus::asset
{
	class ModelLoader final : public Loader
	{
	public:
		explicit ModelLoader() = default;
		~ModelLoader() override = default;
		/**
		 * @brief : アセットのロード
		 * @param : asset_manager : AssetManagerクラスの参照
		 * @param path : ロードされたモデルのファイルパス
		 */
		void Load(AssetManager& asset_manager, const std::filesystem::path& path) override;
		/**
		 * @brief : アセットのロード
		 * @param : asset_manager : AssetManagerクラスの参照
		 * @param from: ロードされたモデルのファイルパス
		 * @param to : コピー先のファイルパス
		 */
		void Load(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) override;
		bool Supported(std::filesystem::path extension) override;
	private:
		std::map<mapping::rename_type::UUID, std::unique_ptr<Model>> models{};

		/**
		 * @brief : フォルダのコピーとモデル読み込み
		 * @remark : 「.fbx」形式の場合選択されたモデルを含むフォルダごとコピーする
		 * @param from : 選択されたモデルまでのパス(拡張子を含む)
		 * @param to : コピー先のフォルダまでのパス
		 */
		mapping::rename_type::UUID Convert(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) override;

		/**
		 * @brief : オーバーロードされているLoad関数の共通処理記述部
		 */
		void Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id) override;

		/**
		 * @brief : 「.fbx」ファイルを「.model」形式に変換
		 * @return : 「.model」ファイルまでの相対パス
		 */
		std::filesystem::path BuildModel(AssetManager& asset_manager, const std::filesystem::path& path);

		/**
		 * @brief : Node Dataの作成
		 */
		void BuildNodes(Model& model, FbxNode* fbx_node, int parent_node_index);
		void BuildNode(Model& model, FbxNode* fbx_node, int parent_node_index);

		/**
		 * @brief : Mesh Dataの作成
		 */
		void BuildMeshes(Model& model, FbxNode* fbx_node);
		void BuildMesh(Model& model, FbxNode* fbx_node, FbxMesh* fbx_mesh);

		/**
		 * Material Dataの作成
		 */
		void BuildMaterials(AssetManager& asset_manager, Model& model, const std::filesystem::path& parent_path, FbxScene* fbx_scene);
		void BuildMaterial(AssetManager& asset_manager, Model& model, const std::filesystem::path& parent_path, FbxSurfaceMaterial* fbx_surface_material);

		/**
		 * @brief : Animation Dataの作成
		 */
		void BuildAnimations(Model& model, FbxScene* fbx_scene);

		/**
		 * @brief : インデックスの検索
		 */
		int FindNodeIndex(Model& model, const char* name);
		int FindMaterialIndex(Model& model, FbxScene* fbx_scene, const FbxSurfaceMaterial* fbx_surface_material);
	};
} // cumulonimbus::asset
