#include "model_loader.h"

#include <filesystem>
#include <set>

#include "asset_sheet_manager.h"
#include "cereal_helper.h"
#include "filename_helper.h"
#include "file_path_helper.h"
#include "generic.h"
#include "locator.h"
#include "material_loader.h"
#include "texture.h"
#include "texture_loader.h"

namespace
{
	// テクスチャを保存するまでのパス
	const std::filesystem::path copy_dir = "./Data/Assets/Models";
}

// FbxDouble2 → XMFLOAT2
inline DirectX::XMFLOAT2 FbxDouble2ToFloat2(const FbxDouble2& fbx_value)
{
	return DirectX::XMFLOAT2{
		static_cast<float>(fbx_value[0]),
		static_cast<float>(fbx_value[1])
	};
}

// FbxDouble3 → XMFLOAT3
inline DirectX::XMFLOAT3 FbxDouble3ToFloat3(const FbxDouble3& fbx_value)
{
	return DirectX::XMFLOAT3{
		static_cast<float>(fbx_value[0]),
		static_cast<float>(fbx_value[1]),
		static_cast<float>(fbx_value[2])
	};
}

// FbxDouble4 → XMFLOAT3
inline DirectX::XMFLOAT3 FbxDouble4ToFloat3(const FbxDouble4& fbx_value)
{
	return DirectX::XMFLOAT3{
		static_cast<float>(fbx_value[0]),
		static_cast<float>(fbx_value[1]),
		static_cast<float>(fbx_value[2])
	};
}

// FbxDouble4 → XMFLOAT4
inline DirectX::XMFLOAT4 FbxDouble4ToFloat4(const FbxDouble4& fbx_value)
{
	return DirectX::XMFLOAT4{
		static_cast<float>(fbx_value[0]),
		static_cast<float>(fbx_value[1]),
		static_cast<float>(fbx_value[2]),
		static_cast<float>(fbx_value[3])
	};
}

// FbxDouble4 → XMFLOAT4
inline DirectX::XMFLOAT4X4 FbxAMatrixToFloat4x4(const FbxAMatrix& fbx_value)
{
	return DirectX::XMFLOAT4X4{
		static_cast<float>(fbx_value[0][0]),
		static_cast<float>(fbx_value[0][1]),
		static_cast<float>(fbx_value[0][2]),
		static_cast<float>(fbx_value[0][3]),
		static_cast<float>(fbx_value[1][0]),
		static_cast<float>(fbx_value[1][1]),
		static_cast<float>(fbx_value[1][2]),
		static_cast<float>(fbx_value[1][3]),
		static_cast<float>(fbx_value[2][0]),
		static_cast<float>(fbx_value[2][1]),
		static_cast<float>(fbx_value[2][2]),
		static_cast<float>(fbx_value[2][3]),
		static_cast<float>(fbx_value[3][0]),
		static_cast<float>(fbx_value[3][1]),
		static_cast<float>(fbx_value[3][2]),
		static_cast<float>(fbx_value[3][3])
	};
}

namespace cumulonimbus::asset
{
	mapping::rename_type::UUID ModelLoader::Convert(
		AssetManager& asset_manager,
		const std::filesystem::path& from,
		const std::filesystem::path& to)
	{
		const auto filename = std::filesystem::path{ from }.filename().replace_extension().string();
		std::filesystem::create_directory(to.string() + "/" + filename);
		const auto to_path = to.string() + "/" + filename;
		if (!equivalent(from.parent_path(), to_path))
		{
			// コピー先のフォルダ作成&コピー
			std::filesystem::copy(
				from.parent_path(), to_path,
				std::filesystem::copy_options::recursive |
				std::filesystem::copy_options::overwrite_existing);
		}

		std::filesystem::path load_path{};
		if(std::set<std::filesystem::path>{".fbx",".FBX"}.contains(from.extension()))
		{// 「.model」形式に変換
			const std::string model_path = to.string() + "/" + filename + "/" + from.filename().string();
			load_path = BuildModel(asset_manager, model_path);
		}
		else
		{// 拡張子が「.model」形式の場合
			load_path = to_path + "/" + from.filename().string();
		}

		for (const auto& [key, value] : asset_manager.GetAssetSheetManager().GetSheet<Model>().sheet)
		{
			if (load_path.compare(value) == 0)
				return key;
		}

		mapping::rename_type::UUID id;
		while (true)
		{
			id = utility::GenerateUUID();
			if (asset_manager.GetAssetSheetManager().GetSheet<Model>().sheet.contains(id))
				continue;
			break;
		}

		// アセットシートの登録
		asset_manager.GetAssetSheetManager().GetSheet<Model>().sheet.insert(std::make_pair(id, load_path.string()));
		return id;
	}

	void ModelLoader::Load(AssetManager& asset_manager,
						   const std::filesystem::path& path)
	{
		const auto id = Convert(asset_manager, path, copy_dir);
		Load(asset_manager, id);
	}

	void ModelLoader::Load(AssetManager& asset_manager,
						   const std::filesystem::path& from,
						   const std::filesystem::path& to)
	{
		const auto id = Convert(asset_manager, from, to);
		Load(asset_manager, id);
	}

	void ModelLoader::Load(AssetManager& asset_manager,
						   const mapping::rename_type::UUID& id)
	{
		// すでにモデルが存在する場合は処理を抜ける
		if (models.contains(id))
			return;

		// モデルの作成
		models.insert(std::make_pair(
			id,
			std::make_unique<Model>(asset_manager.GetAssetSheetManager().GetAssetFilename<Model>(id))));
		// アセットシート(更新後)の保存
		asset_manager.Save();
	}

	void ModelLoader::Rename(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id, const std::string& changed_name)
	{
		// アセットが存在しない場合処理を抜ける
		if (!models.contains(asset_id))
			return;

		// 変更前のファイルパス(拡張子を含まない)   ./Data/Assets/Models/"変更前のモデル名"/"変更前のモデル名"
		const std::filesystem::path	before_path			= std::filesystem::path{ asset_manager.GetAssetSheetManager().GetAssetFilename<Model>(asset_id) }.replace_extension();
		// 変更前の親ファイルパス(拡張子を含まない) ./Data/Assets/Models/"変更前のモデル名"
		const std::filesystem::path before_parent_path	= before_path.parent_path();

		// 重複確認済みのファイル名取得(拡張子を含まない)
		// asset_name = 重複確認済みのシーン名(ファイル名のみ、拡張子を含まない)
		const std::string asset_name = CompareAndReName<Model>(asset_manager,
															   before_parent_path.string() + "/" +
															   changed_name + file_path_helper::GetModelExtension()
															   ).filename().replace_extension().string();

		// 変更後のファイルパス(拡張子を含まない)   ./Data/Assets/Models/"変更前のモデル名"/"変更後のモデル名"
		const std::filesystem::path after_path	= before_parent_path.string() + "/" + asset_name;

		// -- ファイル & フォルダ名の変更 --//
		// 「.model」ファイルのファイル名変更
		// 例 : ./Data/Assets/"変更前のモデル名"/"変更前のモデル名.model" -> ./Data/Assets/"変更前のモデル名"/"変更後のモデル名.model"
		std::filesystem::rename(before_path.string()	    + file_path_helper::GetModelExtension(),
								before_parent_path.string() + "/" + asset_name + file_path_helper::GetModelExtension());
		// 「.fbx」ファイルのファイル名変更
		// 例 : ./Data/Assets/"変更前のモデル名"/"変更前のモデル名.fbx" -> ./Data/Assets/"変更前のモデル名"/"変更後のモデル名.fbx"
		std::filesystem::rename(before_path.string()		+ file_path_helper::GetFbxExtension(),
								before_parent_path.string() + "/" + asset_name + file_path_helper::GetFbxExtension());

		// アセットシート側のファイルパス変更(例 :  ./Data/Assets/Models/"変更後のモデル名"/"変更後のモデル名".model")
		asset_manager.GetAssetSheetManager().GetSheet<Model>().sheet.at(asset_id) = after_path.string() + file_path_helper::GetModelExtension();
		// アセットシートの保存
		asset_manager.Save();
	}


	void ModelLoader::Delete(AssetManager& asset_manager, const std::filesystem::path& path)
	{
		const mapping::rename_type::UUID model_id = asset_manager.GetAssetSheetManager().Search<Model>(path);
		// アセット(ID)が存在していなければ処理を抜ける
		if (!models.contains(model_id))
			return;

		DeleteModel(model_id, path);
		asset_manager.Save();
	}

	void ModelLoader::Delete(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id)
	{
		// アセット(ID)が存在していなければ処理を抜ける
		if (!models.contains(asset_id))
			return;

		const std::filesystem::path path = asset_manager.GetAssetSheetManager().GetAssetFilename<Model>(asset_id);
		DeleteModel(asset_id, path);
		asset_manager.Save();
	}

	bool ModelLoader::Supported(const std::filesystem::path extension)
	{
		static const std::set<std::filesystem::path> extensions
		{
			".fbx",
			".FBX",
			".model"
		};

		return extensions.contains(extension);
	}

	bool ModelLoader::HasModel(const mapping::rename_type::UUID& id) const
	{
		if (models.contains(id))
			return true;
		return false;
	}

	Model& ModelLoader::GetModel(const mapping::rename_type::UUID& model_id)
	{
		if (!models.contains(model_id))
			assert(!"Not found model(ModelLoader::GetModel)");

		return *models.at(model_id).get();
	}

	std::filesystem::path ModelLoader::SearchTextureFilePath(
		const std::filesystem::path& parent_path, const std::filesystem::path& filename) const
	{
		// ファイル拡張子
		const std::string exe		= filename.extension().string();
		// ファイル名(拡張子除く)
		const std::string name_only = std::filesystem::path{ parent_path }.filename().replace_extension().string();
		std::error_code ec{};

		std::filesystem::path result{};
		if(std::filesystem::exists(
			result = std::filesystem::path{ parent_path.string() + "/" + "Textures" + "/" + filename.string()},
			ec))
		{
			return result;
		}
		if (std::filesystem::exists(
			result = std::filesystem::path{
						parent_path.string() + "/" +
						name_only + file_path_helper::GetFbmExtension() + "/" +
						filename.string() },
			ec))
		{
			return result;
		}
		if (std::filesystem::exists(
			result = std::filesystem::path{
				parent_path.string() + "/" + filename.string() },
			ec))
		{
			return result;
		}

		assert(!"Not found file path(ModelLoader::GetTextureFilePath)");
		return {};
	}

	void ModelLoader::DeleteModel(const mapping::rename_type::UUID& model_id, const std::filesystem::path& path)
	{
		//asset::AssetManager& asset_manager = *locator::Locator::GetAssetManager();
		//const auto& material_loader = asset_manager.GetLoader<MaterialLoader>();
		//const auto& texture_loader = asset_manager.GetLoader<TextureLoader>();

		//// parent_path以下の全てのファイル名を取得
		//const std::vector<std::filesystem::path> sub_directories_file_path = utility::GetAllSubDirectoryFilePath(parent_path.string());
		//// sub_directories_file_pathが持つテクスチャファイル名郡
		//std::vector<std::filesystem::path> texture_files{};
		//// sub_directories_file_pathが持つマテリアルファイル名郡
		//std::vector<std::filesystem::path> material_files{};

		////-- 取得されたparent_path以下の全てのファイル名を分別していく --//
		//for(const auto& sub_directory_file_path : sub_directories_file_path)
		//{
		//	const std::string extension = sub_directory_file_path.extension().string();
		//	// マテリアルファイル(「.mat」)
		//	if (material_loader->Supported(extension))
		//	{
		//		material_files.emplace_back(sub_directory_file_path);

		//		if(const mapping::rename_type::UUID mat_id = asset_manager.GetAssetSheetManager().Search<Material>(sub_directory_file_path);
		//		   !mat_id.empty())
		//		{// マテリアルアセットが存在したので削除する
		//			material_loader->Delete(asset_manager, mat_id);
		//		}
		//	}

		//	// テクスチャファイル(「.png」、「.jpeg」、「.tga」、「.dds」)
		//	if (texture_loader->Supported(extension))
		//	{
		//		texture_files.emplace_back(sub_directory_file_path);

		//		if(const mapping::rename_type::UUID tex_id = asset_manager.GetAssetSheetManager().Search<Texture>(sub_directory_file_path);
		//		   !tex_id.empty())
		//		{// テクスチャアセットが存在したので削除する
		//			texture_loader->Delete(asset_manager, tex_id);
		//		}
		//	}
		//}

		// モデルアセットの削除
		models.erase(model_id);
		// ファイルの削除
		std::filesystem::remove(path);
	}

	std::filesystem::path ModelLoader::BuildModel(
		AssetManager& asset_manager,
		const std::filesystem::path& path)
	{
		FbxManager* fbx_manager = FbxManager::Create();

		// FBXに対する入出力を定義する
		FbxIOSettings* fbxIOS = FbxIOSettings::Create(fbx_manager, IOSROOT);	// 特別な理由がない限りIOSROOTを指定
		fbx_manager->SetIOSettings(fbxIOS);

		// インポータを生成(「.fbm」ファイルの作成)
		FbxImporter* fbx_importer = FbxImporter::Create(fbx_manager, "");
		// -1でファイルフォーマット自動判定
		if (!fbx_importer->Initialize(path.string().c_str(), -1, fbx_manager->GetIOSettings()))
			assert(!"FbxImporter::Initialize() : Failed!!\n");

		// SceneオブジェクトにFBXファイル内の情報を流し込む
		FbxScene* fbx_scene = FbxScene::Create(fbx_manager, "scene");
		fbx_importer->Import(fbx_scene);
		fbx_importer->Destroy();	// シーンを流し込んだらImporterは解放してOK

		// ジオメトリを三角形化しておく
		FbxGeometryConverter fbx_geometry_converter(fbx_manager);
		fbx_geometry_converter.Triangulate(fbx_scene, true);
		fbx_geometry_converter.RemoveBadPolygonsFromMeshes(fbx_scene);

#if 0
		// DirectX座標系へ変換
		FbxAxisSystem sceneAxisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();
		if (sceneAxisSystem != FbxAxisSystem::DirectX)
		{
			FbxAxisSystem::DirectX.ConvertScene(fbxScene);
		}
#elif 0
		// OpenGL座標系へ変換
		FbxAxisSystem sceneAxisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();
		if (sceneAxisSystem != FbxAxisSystem::OpenGL)
		{
			FbxAxisSystem::OpenGL.ConvertScene(fbxScene);
		}
#endif

		// モデル構築
		Model model{};
		FbxNode* fbx_root_node = fbx_scene->GetRootNode();
		BuildMaterials(asset_manager, model, path.parent_path(), fbx_scene);
		BuildNodes(model,fbx_root_node, -1);
		BuildMeshes(model, fbx_root_node);
		// アニメーション構築
		BuildAnimations(model, fbx_scene);
		// モデルの保存
		std::filesystem::path model_path{ path };
		model_path.replace_extension();
		model.Save(model_path);

		// マネージャ解放
		fbx_manager->Destroy();

		return model_path.string() + file_path_helper::GetModelExtension();
	}

	void ModelLoader::BuildNodes(
		Model& model, FbxNode* fbx_node,
		int parent_node_index)
	{
		FbxNodeAttribute* fbx_node_attribute = fbx_node->GetNodeAttribute();
		FbxNodeAttribute::EType fbx_node_attribute_type = FbxNodeAttribute::EType::eUnknown;

		if (fbx_node_attribute != nullptr)
		{
			fbx_node_attribute_type = fbx_node_attribute->GetAttributeType();
		}

		switch (fbx_node_attribute_type)
		{
		case FbxNodeAttribute::eUnknown:
		case FbxNodeAttribute::eNull:
		case FbxNodeAttribute::eMarker:
		case FbxNodeAttribute::eMesh:
		case FbxNodeAttribute::eSkeleton:
			BuildNode(model, fbx_node, parent_node_index);
			break;
		}

		// 再帰的に子ノードを処理する
		parent_node_index = static_cast<int>(model.GetModelData().GetNodes().size() - 1);
		for (int i = 0; i < fbx_node->GetChildCount(); ++i)
		{
			BuildNodes(model, fbx_node->GetChild(i), parent_node_index);
		}
	}

	void ModelLoader::BuildNode(
		Model& model, FbxNode* fbx_node,
		const int parent_node_index)
	{
		FbxAMatrix reverse_x;
		reverse_x.SetIdentity();
		reverse_x.SetRow(0, { -1,0,0,0 });

		// ノードのローカル変換行列を取得
		FbxAMatrix fbx_local_transform = reverse_x * fbx_node->EvaluateLocalTransform();

		ModelData::Node node;
		node.name			= fbx_node->GetName();
		node.parent_index	= parent_node_index;
		node.scale			= FbxDouble4ToFloat3(fbx_local_transform.GetS());
		node.rotate			= FbxDouble4ToFloat4(fbx_local_transform.GetQ());
		node.translate		= FbxDouble4ToFloat3(fbx_local_transform.GetT());

		model.GetModelData().GetNodes().emplace_back(node);
	}

	void ModelLoader::BuildMeshes(Model& model, FbxNode* fbx_node)
	{
		FbxNodeAttribute* fbx_node_attribute = fbx_node->GetNodeAttribute();
		FbxNodeAttribute::EType fbx_node_attribute_type = FbxNodeAttribute::EType::eUnknown;

		if (fbx_node_attribute != nullptr)
		{
			fbx_node_attribute_type = fbx_node_attribute->GetAttributeType();
		}

		switch (fbx_node_attribute_type)
		{
		case FbxNodeAttribute::eMesh:
			BuildMesh(model, fbx_node, static_cast<FbxMesh*>(fbx_node_attribute));
			break;
		}

		// 再帰的に子ノードを処理する
		for (int i = 0; i < fbx_node->GetChildCount(); ++i)
		{
			BuildMeshes(model, fbx_node->GetChild(i));
		}
	}

	void ModelLoader::BuildMesh(Model& model, FbxNode* fbx_node, FbxMesh* fbx_mesh)
	{
		int fbx_control_points_count = fbx_mesh->GetControlPointsCount(); // 頂点数取得
	//int fbxPolygonVertexCount = fbx_mesh->GetPolygonVertexCount();
	//const int* fbxPolygonVertices = fbx_mesh->GetPolygonVertices();

		int fbx_material_count = fbx_node->GetMaterialCount(); // マテリアル数取得
		int fbx_polygon_count = fbx_mesh->GetPolygonCount();  // ポリゴン数取得

		model.GetModelData().GetMeshes().emplace_back(ModelData::Mesh());
		ModelData::Mesh& mesh = model.GetModelData().GetMeshes().back();
		mesh.mesh_name = fbx_node->GetName();		// mesh名取得
		mesh.material_count = fbx_material_count;
		mesh.subsets.resize(fbx_material_count > 0 ? fbx_material_count : 1);
		mesh.node_index = FindNodeIndex(model, fbx_node->GetName());

		// サブセットのマテリアル設定
		for (int fbx_material_index = 0; fbx_material_index < fbx_material_count; ++fbx_material_index)
		{
			// fbx_nodeが持っているfbxMaterialIndex番目のマテリアルを取得
			const FbxSurfaceMaterial* fbx_surface_material = fbx_node->GetMaterial(fbx_material_index);

			ModelData::Subset& subset = mesh.subsets.at(fbx_material_index);

			// 現在のマテリアルが「vector<Material> materials」の何番目にあるかを検索
			int index = FindMaterialIndex(model, fbx_node->GetScene(), fbx_surface_material);
			subset.material_index = index;
		}

		// サブセットの頂点インデックス範囲設定
		if (fbx_material_count > 0)
		{
			for (int fbx_polygon_index = 0; fbx_polygon_index < fbx_polygon_count; ++fbx_polygon_index)
			{// マテリアル事の頂点の範囲を探索

				// 現在のメッシュの「fbxPolygonIndex」番目のポリゴンが属するマテリアルの番号を取得
				// (最大で「vecor<Material> materials」のサイズ分)
				int fbx_material_index = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(fbx_polygon_index);
				// 現在のメッシュのfbxMaterialIndexの頂点を見る範囲を加算
				mesh.subsets.at(fbx_material_index).index_count += 3;
			}

			int offset = 0;
			for (ModelData::Subset& subset : mesh.subsets)
			{// 描画開始位置の設定( & 描画範囲の指定 )
				subset.start_index = offset;
				offset += subset.index_count;

				subset.index_count = 0;
			}
		}

		// 頂点影響力データ
		struct BoneInfluence
		{
			int		use_count = 0;
			int		indices[4] = { 0, 0, 0, 0 };
			float	weights[4] = { 1.0f, 0.0f, 0.0f, 0.0f };

			void Add(int index, float weight)
			{
				if (use_count < 4)
				{
					indices[use_count] = index;
					weights[use_count] = weight;
					use_count++;
				}
			}
		};
		// 頂点影響力データを抽出する
		std::vector<BoneInfluence> bone_influences;
		{
			bone_influences.resize(fbx_control_points_count); // 頂点数分要素を確保

			FbxAMatrix fbx_geometric_transform
			{
				fbx_node->GetGeometricTranslation(FbxNode::eSourcePivot),
				fbx_node->GetGeometricRotation(FbxNode::eSourcePivot),
				fbx_node->GetGeometricScaling(FbxNode::eSourcePivot)
			};

			// スキニングに必要な情報を取得する
			int fbx_deformer_count = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
			for (int fbx_deformer_index = 0; fbx_deformer_index < fbx_deformer_count; ++fbx_deformer_index)
			{
				FbxSkin* fbx_skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(fbx_deformer_index, FbxDeformer::eSkin));

				int fbx_cluster_count = fbx_skin->GetClusterCount();
				for (int fbx_cluster_index = 0; fbx_cluster_index < fbx_cluster_count; ++fbx_cluster_index)
				{
					FbxCluster* fbx_cluster = fbx_skin->GetCluster(fbx_cluster_index);

					// 頂点影響力データを抽出する
					{
						int	 fbx_cluster_control_point_indices_count = fbx_cluster->GetControlPointIndicesCount();
						const int* fbx_control_point_indices = fbx_cluster->GetControlPointIndices();
						const double* fbx_control_point_weights = fbx_cluster->GetControlPointWeights();

						for (int i = 0; i < fbx_cluster_control_point_indices_count; ++i)
						{
							BoneInfluence& bone_influence = bone_influences.at(fbx_control_point_indices[i]);

							bone_influence.Add(fbx_cluster_index, static_cast<float>(fbx_control_point_weights[i]));
						}
					}

					// ボーン変換行列用の逆行列の計算をする?
					{
						// メッシュ空間からワールド空間への変換行列
						FbxAMatrix fbx_mesh_space_transform;
						fbx_cluster->GetTransformMatrix(fbx_mesh_space_transform);

						// ボーン空間からワールド空間への変換行列
						FbxAMatrix fbx_bone_space_transform;
						fbx_cluster->GetTransformLinkMatrix(fbx_bone_space_transform);

						// ボーン逆行列を計算する
						FbxAMatrix fbx_inverse_transform = fbx_bone_space_transform.Inverse() * fbx_mesh_space_transform * fbx_geometric_transform;

						DirectX::XMFLOAT4X4 inverse_transform = FbxAMatrixToFloat4x4(fbx_inverse_transform);
						mesh.inverse_transforms.emplace_back(inverse_transform);

						int node_index = FindNodeIndex(model, fbx_cluster->GetLink()->GetName());
						mesh.node_indices.emplace_back(node_index);
					}
				}
			}
		}

		// ジオメトリ行列
		FbxAMatrix fbxGeometricTransform(
			fbx_node->GetGeometricTranslation(FbxNode::eSourcePivot),
			fbx_node->GetGeometricRotation(FbxNode::eSourcePivot),
			fbx_node->GetGeometricScaling(FbxNode::eSourcePivot)
		);
		DirectX::XMFLOAT4X4 geometricTransform = FbxAMatrixToFloat4x4(fbxGeometricTransform);
		DirectX::XMMATRIX GM = DirectX::XMLoadFloat4x4(&geometricTransform);

		// UVセット名
		FbxStringList fbx_uv_set_names;
		fbx_mesh->GetUVSetNames(fbx_uv_set_names);

		// 頂点データ
		mesh.vertices.resize(fbx_polygon_count * 3);
		mesh.indices.resize(fbx_polygon_count * 3);

		int vertex_count = 0;
		const FbxVector4* fbx_control_points = fbx_mesh->GetControlPoints();
		for (int fbx_polygon_index = 0; fbx_polygon_index < fbx_polygon_count; ++fbx_polygon_index)
		{
			// ポリゴンに適用されているマテリアルインデックスを取得する
			int fbx_material_index = 0;
			if (fbx_material_count > 0)
			{
				fbx_material_index = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(fbx_polygon_index);
			}

			ModelData::Subset& subset = mesh.subsets.at(fbx_material_index);
			const int index_offset = subset.start_index + subset.index_count;

			// Deserialize時の処理
			for (int fbx_vertex_index = 0; fbx_vertex_index < 3; ++fbx_vertex_index)
			{
				ModelData::Vertex vertex;

				int fbx_control_point_index = fbx_mesh->GetPolygonVertex(fbx_polygon_index, fbx_vertex_index);
				// Position
				{ // モデルのローカル座標を取得
					//vertex.position = FbxDouble4ToFloat4(fbx_control_points[fbx_control_point_index]);
					//vertex.position.w = 1;

					DirectX::XMFLOAT4 position = FbxDouble4ToFloat4(fbx_control_points[fbx_control_point_index]);
					DirectX::XMVECTOR V = DirectX::XMLoadFloat4(&position);
					V = DirectX::XMVector3TransformCoord(V, GM);
					DirectX::XMStoreFloat4(&vertex.position, V);
				}

				// Weight
				{
					BoneInfluence& bone_influence = bone_influences.at(fbx_control_point_index);
					vertex.bone_index.x = bone_influence.indices[0];
					vertex.bone_index.y = bone_influence.indices[1];
					vertex.bone_index.z = bone_influence.indices[2];
					vertex.bone_index.w = bone_influence.indices[3];
					vertex.bone_weight.x = bone_influence.weights[0];
					vertex.bone_weight.y = bone_influence.weights[1];
					vertex.bone_weight.z = bone_influence.weights[2];
					vertex.bone_weight.w = 1.0f - (vertex.bone_weight.x + vertex.bone_weight.y + vertex.bone_weight.z);
					//vertex.bone_weight.w = bone_influence.weights[3];
				}

				// Normal
				if (fbx_mesh->GetElementNormalCount() > 0)
				{
					FbxVector4 fbx_normal;
					fbx_mesh->GetPolygonVertexNormal(fbx_polygon_index, fbx_vertex_index, fbx_normal);
					vertex.normal = FbxDouble4ToFloat3(fbx_normal);
				}
				else
				{
					vertex.normal = DirectX::XMFLOAT3(0, 0, 0);
				}

				// Texcoord
				if (fbx_mesh->GetElementUVCount() > 0)
				{
					bool fbx_unmapped_uv;
					FbxVector2 fbx_uv;
					fbx_mesh->GetPolygonVertexUV(fbx_polygon_index, fbx_vertex_index, fbx_uv_set_names[0], fbx_uv, fbx_unmapped_uv);
					fbx_uv[1] = 1.0 - fbx_uv[1];
					vertex.tex = FbxDouble2ToFloat2(fbx_uv);
				}
				else
				{
					vertex.tex = DirectX::XMFLOAT2(0, 0);
				}

				mesh.indices.at(index_offset + fbx_vertex_index) = vertex_count;
				mesh.vertices.at(vertex_count) = vertex;
				vertex_count++;
			}

			subset.index_count += 3;
		}
	}

	void ModelLoader::BuildMaterials(
		AssetManager& asset_manager,
		Model& model,
		const std::filesystem::path& parent_path, FbxScene* fbx_scene)
	{
		const int fbx_material_count = fbx_scene->GetMaterialCount();

		if (fbx_material_count > 0)
		{
			for (int fbx_material_index = 0; fbx_material_index < fbx_material_count; ++fbx_material_index)
			{
				FbxSurfaceMaterial* fbx_surface_material = fbx_scene->GetMaterial(fbx_material_index);

				BuildMaterial(asset_manager, model, parent_path, fbx_surface_material);
			}
		}
		else
		{
			// デフォルトマテリアルの使用
			model.GetModelData().GetMaterialsID().emplace_back("");
		}
	}

	void ModelLoader::BuildMaterial(
		AssetManager& asset_manager,
		Model& model,
		const std::filesystem::path& parent_path,
		FbxSurfaceMaterial* fbx_surface_material)
	{
		bool ret = false;
		MaterialData material{};

		// ディフューズカラー
		const FbxProperty fbx_diffuse_property = fbx_surface_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
		const FbxProperty fbx_normal_property = fbx_surface_material->FindProperty(FbxSurfaceMaterial::sNormalMap);
		const FbxProperty fbx_diffuse_factor_property = fbx_surface_material->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);
		if (fbx_diffuse_property.IsValid() && fbx_diffuse_factor_property.IsValid())
		{// .IsValid()でFbxPropertyの有無を確認
			const FbxDouble  fbx_factor = fbx_diffuse_factor_property.Get<FbxDouble>();
			const FbxDouble3 fbx_color  = fbx_diffuse_property.Get<FbxDouble3>();

			material.color = DirectX::SimpleMath::Vector4{
				static_cast<float>(fbx_color[0] * fbx_factor),
				static_cast<float>(fbx_color[1] * fbx_factor),
				static_cast<float>(fbx_color[2] * fbx_factor),
				1.0f };
		}

		std::filesystem::path texture_path{""};
		// ディフューズテクスチャ
		if (fbx_diffuse_property.IsValid())
		{
			const int fbx_texture_count = fbx_diffuse_property.GetSrcObjectCount<FbxFileTexture>();
			if (fbx_texture_count > 0)
			{
				FbxFileTexture* fbx_texture = fbx_diffuse_property.GetSrcObject<FbxFileTexture>();
				const std::string texture_filename = std::filesystem::path{ fbx_texture->GetFileName() }.filename().string();
				texture_path = SearchTextureFilePath(parent_path, texture_filename);
				// テクスチャロード
				asset_manager.GetLoader<TextureLoader>()->CreateTexture(asset_manager, texture_path);
				const mapping::rename_type::UUID id = asset_manager.GetAssetSheetManager().Search<Texture>(texture_path);
				material.albedo_id = id;
			}
		}
		if(fbx_normal_property.IsValid())
		{
			const int fbx_texture_count = fbx_normal_property.GetSrcObjectCount<FbxFileTexture>();
			if (fbx_texture_count > 0)
			{
				FbxFileTexture* fbx_texture = fbx_normal_property.GetSrcObject<FbxFileTexture>();
				const std::string texture_filename = std::filesystem::path{ fbx_texture->GetFileName() }.filename().string();
				texture_path = SearchTextureFilePath(parent_path, texture_filename);
				// テクスチャロード
				asset_manager.GetLoader<TextureLoader>()->CreateTexture(asset_manager, texture_path);
				const mapping::rename_type::UUID id = asset_manager.GetAssetSheetManager().Search<Texture>(texture_path);
				material.normal_id = id;
			}
		}

		// マテリアルの作成&アセットシート(更新後)の保存
		auto mat_id = asset_manager.GetLoader<MaterialLoader>()->CreateMaterial(
																	asset_manager, parent_path,
																	material, std::filesystem::path{ texture_path }.filename().replace_extension().string());
		model.GetModelData().GetMaterialsID().emplace_back(mat_id);
	}

	void ModelLoader::BuildAnimations(Model& model, FbxScene* fbx_scene)
	{
		// すべてのアニメーション名を取得
		FbxArray<FbxString*> fbx_anim_stack_names;
		fbx_scene->FillAnimStackNameArray(fbx_anim_stack_names);

		int fbx_animation_count = fbx_anim_stack_names.Size();
		for (int fbx_animation_index = 0; fbx_animation_index < fbx_animation_count; ++fbx_animation_index)
		{
			model.GetModelData().GetAnimations().emplace_back(ModelData::Animation());
			ModelData::Animation& animation = model.GetModelData().GetAnimations().back();

			// アニメーションデータのサンプリング設定
			FbxTime::EMode fbx_time_mode = fbx_scene->GetGlobalSettings().GetTimeMode();
			FbxTime fbx_frame_time;
			fbx_frame_time.SetTime(0, 0, 0, 1, 0, fbx_time_mode);	// 1フレーム当たりの時間を算出?

			const float sampling_rate = static_cast<float>(fbx_frame_time.GetFrameRate(fbx_time_mode));
			const float sampling_time = 1.0f / sampling_rate;
			animation.sampling_rate = sampling_rate;
			animation.sampling_time = sampling_time;

			FbxString* fbx_anim_stack_name	= fbx_anim_stack_names.GetAt(fbx_animation_index);
			animation.animation_name		= fbx_anim_stack_name->Buffer();
			FbxAnimStack* fbx_anim_stack	= fbx_scene->FindMember<FbxAnimStack>(fbx_anim_stack_name->Buffer());

			// 再生するアニメーションを指定する。
			fbx_scene->SetCurrentAnimationStack(fbx_anim_stack);

			// アニメーションの再生開始時間と再生終了時間を取得する
			FbxTakeInfo* fbx_take_info	= fbx_scene->GetTakeInfo(fbx_anim_stack_name->Buffer());
			FbxTime fbx_start_time		= fbx_take_info->mLocalTimeSpan.GetStart();
			FbxTime fbx_end_time		= fbx_take_info->mLocalTimeSpan.GetStop();

			// 抽出するデータは60フレーム基準でサンプリングする
			FbxTime fbx_sampling_step;
			fbx_sampling_step.SetTime(0, 0, 1, 0, 0, fbx_time_mode);	// 1秒当たりの時間を算出?
			fbx_sampling_step = static_cast<FbxLongLong>(fbx_sampling_step.Get() * sampling_time);

			int start_frame = static_cast<int>(fbx_start_time.Get() / fbx_sampling_step.Get());
			int end_frame = static_cast<int>(fbx_end_time.Get() / fbx_sampling_step.Get());
			const int frame_count = static_cast<int>((fbx_end_time.Get() - fbx_start_time.Get()) / fbx_sampling_step.Get());
			animation.num_key_frame = frame_count;

			// アニメーションの対象となるノードを列挙する
			std::vector<FbxNode*> fbx_nodes;
			FbxNode* fbx_root_node = fbx_scene->GetRootNode();
			for (ModelData::Node& node : model.GetModelData().GetNodes())
			{
				FbxNode* fbx_node = fbx_root_node->FindChild(node.name.c_str());
				fbx_nodes.emplace_back(fbx_node);
			}

			// アニメーションデータを抽出する
			animation.seconds_length = frame_count * sampling_time;
			animation.keyframes.resize(frame_count + 1);

			float seconds = 0.0f;
			ModelData::Keyframe* keyframe = animation.keyframes.data();
			size_t fbx_node_count = fbx_nodes.size();
			for (FbxTime fbx_current_time = fbx_start_time; fbx_current_time < fbx_end_time; fbx_current_time += fbx_sampling_step, ++keyframe)
			{
				// キーフレーム毎の姿勢データを取り出す。
				keyframe->seconds = seconds;
				keyframe->node_keys.resize(fbx_node_count);
				for (size_t fbx_node_index = 0; fbx_node_index < fbx_node_count; ++fbx_node_index)
				{
					ModelData::NodeKeyData& key_data = keyframe->node_keys.at(fbx_node_index);
					FbxNode* fbx_node = fbx_nodes.at(fbx_node_index);
					if (fbx_node == nullptr)
					{
						// アニメーション対象のノードがなかったのでダミーデータを設定
						ModelData::Node& node = model.GetModelData().GetNodes().at(fbx_node_index);
						key_data.scale = node.scale;
						key_data.rotate = node.rotate;
						key_data.translate = node.translate;
					}
					else if (fbx_node_index == -1)
					{
						// ルートモーションは無視する
						ModelData::Node& node = model.GetModelData().GetNodes().at(fbx_node_index);
						key_data.scale = DirectX::XMFLOAT3(1, 1, 1);
						key_data.rotate = DirectX::XMFLOAT4(0, 0, 0, 1);
						key_data.translate = DirectX::XMFLOAT3(0, 0, 0);
					}
					else
					{
						// 指定時間のローカル行列からスケール値、回転値、移動値を取り出す。
						const FbxAMatrix& fbxLocalTransform = fbx_node->EvaluateLocalTransform(fbx_current_time);

						key_data.scale = FbxDouble4ToFloat3(fbxLocalTransform.GetS());
						key_data.rotate = FbxDouble4ToFloat4(fbxLocalTransform.GetQ());
						key_data.translate = FbxDouble4ToFloat3(fbxLocalTransform.GetT());
					}
				}
				seconds += sampling_time;
			}
		}

		// 後始末
		for (int i = 0; i < fbx_animation_count; i++)
		{
			delete fbx_anim_stack_names[i];
		}
	}

	int ModelLoader::FindNodeIndex(Model& model, const char* name)
	{
		for (size_t i = 0; i < model.GetModelData().GetNodes().size(); ++i)
		{
			if (model.GetModelData().GetNodes()[i].name == name)
			{
				return static_cast<int>(i);
			}
		}
		return -1;
	}

	int ModelLoader::FindMaterialIndex(
		Model& model,
		FbxScene* fbx_scene,
		const FbxSurfaceMaterial* fbx_surface_material)
	{
		const int fbx_material_count = fbx_scene->GetMaterialCount();

		for (int i = 0; i < fbx_material_count; ++i)
		{
			if (fbx_scene->GetMaterial(i) == fbx_surface_material)
			{
				return i;
			}
		}
		return -1;
	}
} // cumulonimbus::asset
