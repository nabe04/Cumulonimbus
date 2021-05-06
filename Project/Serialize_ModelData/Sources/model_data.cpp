#include <stdlib.h>
#include <assert.h>
#include <fstream>
#include <functional>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include "model_data.h"

// CEREALバージョン定義
CEREAL_CLASS_VERSION(ModelData::Node, 1)
CEREAL_CLASS_VERSION(ModelData::Material, 1)
CEREAL_CLASS_VERSION(ModelData::Subset, 1)
CEREAL_CLASS_VERSION(ModelData::Vertex, 1)
CEREAL_CLASS_VERSION(ModelData::Mesh, 1)
CEREAL_CLASS_VERSION(ModelData::NodeKeyData, 1)
CEREAL_CLASS_VERSION(ModelData::Keyframe, 1)
CEREAL_CLASS_VERSION(ModelData::Animation, 1)
CEREAL_CLASS_VERSION(ModelData, 1)


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

//template<class Archive>
//void ModelData::Node::serialize(Archive& archive, int version)
//{
//	archive(
//		CEREAL_NVP(name),
//		CEREAL_NVP(parent_index),
//		CEREAL_NVP(scale),
//		CEREAL_NVP(rotate),
//		CEREAL_NVP(translate)
//		);
//}
//
//template<class Archive>
//void ModelData::Material::serialize(Archive& archive, int version)
//{
//	archive(
//		CEREAL_NVP(color),
//		CEREAL_NVP(texture_filename)
//		);
//}
//
//template<class Archive>
//void ModelData::Subset::serialize(Archive& archive, int version)
//{
//	archive(
//		CEREAL_NVP(start_index),
//		CEREAL_NVP(index_count)
//		);
//}
//
//template<class Archive>
//void ModelData::Mesh::serialize(Archive& archive, int version)
//{
//	archive(
//		CEREAL_NVP(subsets),
//		CEREAL_NVP(node_index),
//		CEREAL_NVP(node_indices),
//		CEREAL_NVP(inverse_transforms)
//		);
//}
//
//template<class Archive>
//void ModelData::NodeKeyData::serialize(Archive& archive, int version)
//{
//	archive(
//		CEREAL_NVP(scale),
//		CEREAL_NVP(rotate),
//		CEREAL_NVP(translate)
//		);
//}
//
//template<class Archive>
//void ModelData::Keyframe::serialize(Archive& archive, int version)
//{
//	archive(
//		CEREAL_NVP(seconds),
//		CEREAL_NVP(node_keys)
//		);
//}
//
//template<class Archive>
//void ModelData::Animation::serialize(Archive& archive, int version)
//{
//	archive(
//		CEREAL_NVP(seconds_length),
//		CEREAL_NVP(keyframes)
//		);
//}

