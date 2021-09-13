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
		 * @brief : �A�Z�b�g�̃��[�h
		 * @param : asset_manager : AssetManager�N���X�̎Q��
		 * @param path : ���[�h���ꂽ���f���̃t�@�C���p�X
		 */
		void Load(AssetManager& asset_manager, const std::filesystem::path& path) override;
		/**
		 * @brief : �A�Z�b�g�̃��[�h
		 * @param : asset_manager : AssetManager�N���X�̎Q��
		 * @param from: ���[�h���ꂽ���f���̃t�@�C���p�X
		 * @param to : �R�s�[��̃t�@�C���p�X
		 */
		void Load(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) override;
		bool Supported(std::filesystem::path extension) override;
	private:
		std::map<mapping::rename_type::UUID, std::unique_ptr<Model>> models{};

		/**
		 * @brief : �t�H���_�̃R�s�[�ƃ��f���ǂݍ���
		 * @remark : �u.fbx�v�`���̏ꍇ�I�����ꂽ���f�����܂ރt�H���_���ƃR�s�[����
		 * @param from : �I�����ꂽ���f���܂ł̃p�X(�g���q���܂�)
		 * @param to : �R�s�[��̃t�H���_�܂ł̃p�X
		 */
		mapping::rename_type::UUID Convert(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) override;

		/**
		 * @brief : �I�[�o�[���[�h����Ă���Load�֐��̋��ʏ����L�q��
		 */
		void Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id) override;

		/**
		 * @brief : �u.fbx�v�t�@�C�����u.model�v�`���ɕϊ�
		 * @return : �u.model�v�t�@�C���܂ł̑��΃p�X
		 */
		std::filesystem::path BuildModel(AssetManager& asset_manager, const std::filesystem::path& path);

		/**
		 * @brief : Node Data�̍쐬
		 */
		void BuildNodes(Model& model, FbxNode* fbx_node, int parent_node_index);
		void BuildNode(Model& model, FbxNode* fbx_node, int parent_node_index);

		/**
		 * @brief : Mesh Data�̍쐬
		 */
		void BuildMeshes(Model& model, FbxNode* fbx_node);
		void BuildMesh(Model& model, FbxNode* fbx_node, FbxMesh* fbx_mesh);

		/**
		 * Material Data�̍쐬
		 */
		void BuildMaterials(AssetManager& asset_manager, Model& model, const std::filesystem::path& parent_path, FbxScene* fbx_scene);
		void BuildMaterial(AssetManager& asset_manager, Model& model, const std::filesystem::path& parent_path, FbxSurfaceMaterial* fbx_surface_material);

		/**
		 * @brief : Animation Data�̍쐬
		 */
		void BuildAnimations(Model& model, FbxScene* fbx_scene);

		/**
		 * @brief : �C���f�b�N�X�̌���
		 */
		int FindNodeIndex(Model& model, const char* name);
		int FindMaterialIndex(Model& model, FbxScene* fbx_scene, const FbxSurfaceMaterial* fbx_surface_material);
	};
} // cumulonimbus::asset
