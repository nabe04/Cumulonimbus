#include "build_fbx.h"

#include <cassert>
#include <fstream>

// シリアライズ
namespace DirectX	// namespaceをそろえる
{
	template<class Archive>
	void serialize(Archive& archive, XMUINT4& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z),
			cereal::make_nvp("w", v.w)
		);
	}

	template<class Archive>
	void serialize(Archive& archive, XMFLOAT2& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y)
		);
	}

	template<class Archive>
	void serialize(Archive& archive, XMFLOAT3& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z)
		);
	}

	template<class Archive>
	void serialize(Archive& archive, XMFLOAT4& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z),
			cereal::make_nvp("w", v.w)
		);
	}

	template<class Archive>
	void serialize(Archive& archive, XMFLOAT4X4& m)
	{
		archive(
			cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12), cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
			cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22), cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
			cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32), cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
			cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42), cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
		);
	}
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

BulidFBX::BulidFBX(const char* fbx_filename, const char* ignore_root_motion_node_name)
{
	FbxManager* fbx_manager = FbxManager::Create();

	// FBXに対する入出力を定義する
	FbxIOSettings* fbxIOS = FbxIOSettings::Create(fbx_manager, IOSROOT);	// 特別な理由がない限りIOSROOTを指定
	fbx_manager->SetIOSettings(fbxIOS);

	// インポータを生成
	FbxImporter* fbx_importer = FbxImporter::Create(fbx_manager, "");
	bool result = fbx_importer->Initialize(fbx_filename, -1, fbx_manager->GetIOSettings());	// -1でファイルフォーマット自動判定
	if (!result)
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

	// ディレクトリパス取得
	char dirname[256];
	::_splitpath_s(fbx_filename, nullptr, 0, dirname, 256, nullptr, 0, nullptr, 0);

	// モデル構築
	FbxNode* fbx_root_node = fbx_scene->GetRootNode();
	BuildMaterials(dirname, fbx_scene);
	BuildNodes(fbx_root_node, -1);
	BuildMeshes(fbx_root_node);

	// 無視するルートモーションを検索
	if (ignore_root_motion_node_name != nullptr)
	{
		root_motion_node_index = -1;
		for (size_t i = 0; i < model_data.nodes.size(); ++i)
		{
			if (model_data.nodes.at(i).name == ignore_root_motion_node_name)
			{
				root_motion_node_index = static_cast<int>(i);
				break;
			}
		}
	}

	// アニメーション構築
	BuildAnimations(fbx_scene);

	// マネージャ解放
	fbx_manager->Destroy();		// 関連するすべてのオブジェクトが解放される
}

BulidFBX::~BulidFBX()
{

}

void BulidFBX::AddAnimation(const char* fbx_filename)
{
	FbxManager* fbx_manager = FbxManager::Create();

	// FBXに対する入出力を定義する
	FbxIOSettings* fbxIOS = FbxIOSettings::Create(fbx_manager, IOSROOT);	// 特別な理由がない限りIOSROOTを指定
	fbx_manager->SetIOSettings(fbxIOS);

	// インポータを生成
	FbxImporter* fbx_importer = FbxImporter::Create(fbx_manager, "");
	bool result = fbx_importer->Initialize(fbx_filename, -1, fbx_manager->GetIOSettings());	// -1でファイルフォーマット自動判定
	if (!result)
		assert(!"FbxImporter::Initialize() : Failed!!\n");


	// SceneオブジェクトにFBXファイル内の情報を流し込む
	FbxScene* fbx_scene = FbxScene::Create(fbx_manager, "scene");
	fbx_importer->Import(fbx_scene);
	fbx_importer->Destroy();	// シーンを流し込んだらImporterは解放してOK

	// アニメーション構築
	BuildAnimations(fbx_scene);

	// マネージャ解放
	fbx_manager->Destroy();		// 関連するすべてのオブジェクトが解放される
}

// FBXノードを再帰的に辿ってデータを構築する
void BulidFBX::BuildNodes(FbxNode* fbx_node, int parent_node_index)
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
		BuildNode(fbx_node, parent_node_index);
		break;
	}

	// 再帰的に子ノードを処理する
	parent_node_index = static_cast<int>(model_data.nodes.size() - 1);
	for (int i = 0; i < fbx_node->GetChildCount(); ++i)
	{
		BuildNodes(fbx_node->GetChild(i), parent_node_index);
	}
}

// FBXノードからノードデータを構築する
void BulidFBX::BuildNode(FbxNode* fbx_node, int parent_node_index)
{
	FbxAMatrix reverse_x;
	reverse_x.SetIdentity();
	reverse_x.SetRow(0, { -1,0,0,0 });

	// ノードのローカル変換行列を取得
	FbxAMatrix fbx_local_transform = reverse_x * fbx_node->EvaluateLocalTransform();

	ModelData::Node node;
	node.name = fbx_node->GetName();
	node.parent_index = parent_node_index;
	node.scale = FbxDouble4ToFloat3(fbx_local_transform.GetS());
	node.rotate = FbxDouble4ToFloat4(fbx_local_transform.GetQ());
	node.translate = FbxDouble4ToFloat3(fbx_local_transform.GetT());

	model_data.nodes.emplace_back(node);
}

// FBXノードを再帰的に辿ってメッシュデータを構築する
void BulidFBX::BuildMeshes(FbxNode* fbx_node)
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
		BuildMesh(fbx_node, static_cast<FbxMesh*>(fbx_node_attribute));
		break;
	}

	// 再帰的に子ノードを処理する
	for (int i = 0; i < fbx_node->GetChildCount(); ++i)
	{
		BuildMeshes(fbx_node->GetChild(i));
	}
}

// FBXメッシュからメッシュデータを構築する
void BulidFBX::BuildMesh(FbxNode* fbx_node, FbxMesh* fbx_mesh)
{
	int fbx_control_points_count = fbx_mesh->GetControlPointsCount(); // 頂点数取得
	//int fbxPolygonVertexCount = fbx_mesh->GetPolygonVertexCount();
	//const int* fbxPolygonVertices = fbx_mesh->GetPolygonVertices();

	int fbx_material_count = fbx_node->GetMaterialCount(); // マテリアル数取得
	int fbx_polygon_count  = fbx_mesh->GetPolygonCount();  // ポリゴン数取得

	model_data.meshes.emplace_back(ModelData::Mesh());
	ModelData::Mesh& mesh	= model_data.meshes.back();
	mesh.mesh_name			= fbx_mesh->GetName();		// mesh名取得
	mesh.material_count		= fbx_material_count;
	mesh.subsets.resize(fbx_material_count > 0 ? fbx_material_count : 1);
	mesh.node_index			= FindNodeIndex(fbx_node->GetName());

	// サブセットのマテリアル設定
	for (int fbx_material_index = 0; fbx_material_index < fbx_material_count; ++fbx_material_index)
	{
		// fbx_nodeが持っているfbxMaterialIndex番目のマテリアルを取得
		const FbxSurfaceMaterial* fbx_surface_material = fbx_node->GetMaterial(fbx_material_index);

		ModelData::Subset& subset = mesh.subsets.at(fbx_material_index);

		// HACK : indexに変更
		// 現在のマテリアルが「vector<Material> materials」の何番目にあるかを検索
		int index = FindMaterialIndex(fbx_node->GetScene(), fbx_surface_material);
		subset.material = &model_data.materials.at(index); // 初めに読み込んだマテリアルのリンク & std::vector<Material> materialsの「index」番目の要素にアクセス
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
						BoneInfluence& boneInfluence = bone_influences.at(fbx_control_point_indices[i]);

						boneInfluence.Add(fbx_cluster_index, static_cast<float>(fbx_control_point_weights[i]));
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

					int node_index = FindNodeIndex(fbx_cluster->GetLink()->GetName());
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
				vertex.bone_index.x  = bone_influence.indices[0];
				vertex.bone_index.y  = bone_influence.indices[1];
				vertex.bone_index.z  = bone_influence.indices[2];
				vertex.bone_index.w  = bone_influence.indices[3];
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

// FBXシーン内のFBXマテリアルからマテリアルデータを構築する
void BulidFBX::BuildMaterials(const char* dirname, FbxScene* fbx_scene)
{
	int fbx_material_count = fbx_scene->GetMaterialCount();

	if (fbx_material_count > 0)
	{
		for (int fbx_material_index = 0; fbx_material_index < fbx_material_count; ++fbx_material_index)
		{
			FbxSurfaceMaterial* fbx_surface_material = fbx_scene->GetMaterial(fbx_material_index);

			BuildMaterial(dirname, fbx_surface_material);
		}
	}
	else
	{
		ModelData::Material material;
		material.color = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
		model_data.materials.emplace_back(material);
	}
}

// FBXマテリアルからマテリアルデータを構築する & 追加
void BulidFBX::BuildMaterial(const char* dirname, FbxSurfaceMaterial* fbx_surface_material)
{
	bool ret = false;

	ModelData::Material material{};

	// ディフューズカラー
	FbxProperty fbx_diffuse_property = fbx_surface_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
	FbxProperty fbx_diffuse_factor_property = fbx_surface_material->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);
	if (fbx_diffuse_property.IsValid() && fbx_diffuse_factor_property.IsValid())
	{// .IsValid()でFbxPropertyの有無を確認
		FbxDouble fbx_factor = fbx_diffuse_factor_property.Get<FbxDouble>();
		FbxDouble3 fbx_color = fbx_diffuse_property.Get<FbxDouble3>();

		material.color.x = static_cast<float>(fbx_color[0] * fbx_factor);
		material.color.y = static_cast<float>(fbx_color[1] * fbx_factor);
		material.color.z = static_cast<float>(fbx_color[2] * fbx_factor);
		material.color.w = 1.0f;
	}
#if 0
	FbxProperty fbx_transparency_factor_property = fbxSurfaceMaterial->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
	if (fbx_transparency_factor_property.IsValid())
	{
		FbxDouble fbxFactor = fbx_transparency_factor_property.Get<FbxDouble>();

		material.color.w = static_cast<float>(fbxFactor);
	}
#endif
	// ディフューズテクスチャ
	if (fbx_diffuse_property.IsValid())
	{
		int fbx_texture_count = fbx_diffuse_property.GetSrcObjectCount<FbxFileTexture>();
		if (fbx_texture_count > 0)
		{
			FbxFileTexture* fbx_texture = fbx_diffuse_property.GetSrcObject<FbxFileTexture>();

			// 相対パスの解決
			char filename[256];
			::_makepath_s(filename, 256, nullptr, dirname, fbx_texture->GetRelativeFileName(), nullptr);

			// マルチバイト文字からワイド文字へ変換
			size_t length;
			wchar_t wfilename[256];
			::mbstowcs_s(&length, wfilename, 256, filename, _TRUNCATE);

			// テクスチャ読み込み
			//material.texture = TextureManager::GetInstance()->CreateTexture(filename);
			material.texture_filename = filename;
		}
	}

	model_data.materials.emplace_back(material);
}

// アニメーションデータを構築
void BulidFBX::BuildAnimations(FbxScene* fbx_scene)
{
	// すべてのアニメーション名を取得
	FbxArray<FbxString*> fbx_anim_stack_names;
	fbx_scene->FillAnimStackNameArray(fbx_anim_stack_names);

	int fbx_animation_count = fbx_anim_stack_names.Size();
	for (int fbx_animation_index = 0; fbx_animation_index < fbx_animation_count; ++fbx_animation_index)
	{
		model_data.animations.emplace_back(ModelData::Animation());
		ModelData::Animation& animation = model_data.animations.back();

		// アニメーションデータのサンプリング設定
		FbxTime::EMode fbx_time_mode = fbx_scene->GetGlobalSettings().GetTimeMode();
		FbxTime fbx_frame_time;
		fbx_frame_time.SetTime(0, 0, 0, 1, 0, fbx_time_mode);	// 1フレーム当たりの時間を算出?

		float sampling_rate = static_cast<float>(fbx_frame_time.GetFrameRate(fbx_time_mode));
		float sampling_time = 1.0f / sampling_rate;

		FbxString* fbx_anim_stack_name = fbx_anim_stack_names.GetAt(fbx_animation_index);
		FbxAnimStack* fbx_anim_stack = fbx_scene->FindMember<FbxAnimStack>(fbx_anim_stack_name->Buffer());

		// 再生するアニメーションを指定する。
		fbx_scene->SetCurrentAnimationStack(fbx_anim_stack);

		// アニメーションの再生開始時間と再生終了時間を取得する
		FbxTakeInfo* fbx_take_info = fbx_scene->GetTakeInfo(fbx_anim_stack_name->Buffer());
		FbxTime fbx_start_time = fbx_take_info->mLocalTimeSpan.GetStart();
		FbxTime fbx_end_time = fbx_take_info->mLocalTimeSpan.GetStop();

		// 抽出するデータは60フレーム基準でサンプリングする
		FbxTime fbx_sampling_step;
		fbx_sampling_step.SetTime(0, 0, 1, 0, 0, fbx_time_mode);	// 1秒当たりの時間を算出?
		fbx_sampling_step = static_cast<FbxLongLong>(fbx_sampling_step.Get() * sampling_time);

		int start_frame = static_cast<int>(fbx_start_time.Get() / fbx_sampling_step.Get());
		int end_frame = static_cast<int>(fbx_end_time.Get() / fbx_sampling_step.Get());
		int frame_count = static_cast<int>((fbx_end_time.Get() - fbx_start_time.Get()) / fbx_sampling_step.Get());

		// アニメーションの対象となるノードを列挙する
		std::vector<FbxNode*> fbx_nodes;
		FbxNode* fbx_root_node = fbx_scene->GetRootNode();
		for (ModelData::Node& node : model_data.nodes)
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
		FbxTime fbxCurrentTime = fbx_start_time;
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
					ModelData::Node& node = model_data.nodes.at(fbx_node_index);
					key_data.scale = node.scale;
					key_data.rotate = node.rotate;
					key_data.translate = node.translate;
				}
				else if (fbx_node_index == root_motion_node_index)
				{
					// ルートモーションは無視する
					ModelData::Node& node = model_data.nodes.at(fbx_node_index);
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

// ノードインデックスを取得する
int BulidFBX::FindNodeIndex(const char* name)
{
	for (size_t i = 0; i < model_data.nodes.size(); ++i)
	{
		if (model_data.nodes[i].name == name)
		{
			return static_cast<int>(i);
		}
	}
	return -1;
}

// マテリアルインデックスを取得する
int BulidFBX::FindMaterialIndex(FbxScene* fbx_scene, const FbxSurfaceMaterial* fbx_surface_material)
{
	int fbx_material_count = fbx_scene->GetMaterialCount();

	for (int i = 0; i < fbx_material_count; ++i)
	{
		if (fbx_scene->GetMaterial(i) == fbx_surface_material)
		{
			return i;
		}
	}
	return -1;
}


void BulidFBX::Export(const char* filename)
{
	// シリアライズ
	std::ofstream ostream(filename, std::ios::binary);
	if (ostream.is_open())
	{
		cereal::BinaryOutputArchive archive(ostream);

		try
		{
			archive(
				CEREAL_NVP(model_data.nodes),
				CEREAL_NVP(model_data.materials),
				CEREAL_NVP(model_data.meshes),
				CEREAL_NVP(model_data.animations)
			);
		}
		catch (...)
		{
			assert(!"model deserialize failed.\n%s\n", filename);
			return;
		}
	}
}

void BulidFBX::Load(const char* filename)
{
	std::ifstream istream(filename, std::ios::binary);

	cereal::BinaryInputArchive archive(istream);

	archive(
		CEREAL_NVP(model_data.nodes),
		CEREAL_NVP(model_data.materials),
		CEREAL_NVP(model_data.meshes),
		CEREAL_NVP(model_data.animations)
	);
}

