#include "material_loader.h"

#include "asset_sheet_manager.h"
#include "file_path_helper.h"

namespace
{
	// テクスチャを保存するまでのパス
	const std::filesystem::path copy_dir = "./Data/Assets/Materials";
}


namespace cumulonimbus::asset
{
	MaterialLoader::MaterialLoader()
	{
		dummy_material = std::make_unique<Material>();
	}

	void MaterialLoader::Load(AssetManager& asset_manager, const std::filesystem::path& path)
	{
		const auto id = Convert(asset_manager, path, copy_dir);
		Load(asset_manager, id);
	}

	void MaterialLoader::Load(AssetManager& asset_manager, const std::filesystem::path& from,
		const std::filesystem::path& to)
	{
		const auto id = Convert(asset_manager, from, to);
		Load(asset_manager, id);
	}

	void MaterialLoader::Delete(AssetManager& asset_manager, const std::filesystem::path& path)
	{

	}

	bool MaterialLoader::Supported(std::filesystem::path extension)
	{
		static const std::set<std::filesystem::path> extensions
		{
			".mat"
		};
		return extensions.contains(extension);
	}

	mapping::rename_type::UUID MaterialLoader::CreateMaterial(
		AssetManager& asset_manager, const std::filesystem::path& parent_path,
		const MaterialData& material_data = {}, std::string material_name = {""})
	{
		// マテリアル名が無名の場合「New Material」に名前を設定
		if(material_name == "")
		{
			const std::string path = parent_path.string() + "/Materials" + "/New Name" + file_path_helper::GetMaterialExtension();
			material_name = CompareAndReName<Material>(asset_manager, path).filename().replace_extension().string();
		}
		const std::string save_path{
			parent_path.string() + "/"
			+ "Materials" + "/"
			+ material_name + file_path_helper::GetMaterialExtension() };

		// 作成したマテリアルを「.mat」形式で保存
		std::unique_ptr<Material>mat = std::make_unique<Material>(material_data);
		mat->Save(save_path);

		// 保存したマテリアルのIDを作成
		mapping::rename_type::UUID id;
		while (true)
		{
			id = utility::GenerateUUID();
			if (asset_manager.GetAssetSheetManager().GetSheet<Material>().sheet.contains(id))
				continue;
			break;
		}

		// アセットシートの登録
		asset_manager.GetAssetSheetManager().GetSheet<Material>().sheet.insert(std::make_pair(id, save_path));
		// マテリアルの追加
		materials.insert(std::make_pair(
			id,
			std::move(mat))
		);
		// アセットシート(更新後)の保存
		asset_manager.Save();
		return id;
	}

	void MaterialLoader::Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id)
	{
		// すでにテクスチャが存在する場合は処理を抜ける
		if (materials.contains(id))
			return;
		Material m{};
		m.Load(asset_manager.GetAssetSheetManager().GetAssetFilename<Material>(id));
		// マテリアルの作成
		materials.insert(std::make_pair(
			id,
			std::make_unique<Material>(m))
		);
		// アセットシート(更新後)の保存
		asset_manager.Save();
	}

	mapping::rename_type::UUID MaterialLoader::Convert(AssetManager& asset_manager,
		const std::filesystem::path& from, const std::filesystem::path& to)
	{
		// コピー先のフォルダ作成&コピー
		std::filesystem::copy(
			from, to,
			std::filesystem::copy_options::recursive |
			std::filesystem::copy_options::overwrite_existing);

		const std::string copy_str = to.string() + "/" + from.filename().string();
		const std::filesystem::path copy_path{ copy_str };

		auto sheet = asset_manager.GetAssetSheetManager().GetSheet<Material>().sheet;

		for (const auto& [key, value] : asset_manager.GetAssetSheetManager().GetSheet<Material>().sheet)
		{
			if (copy_path.compare(value) == 0)
				return key;
		}

		mapping::rename_type::UUID id;
		while (true)
		{
			id = utility::GenerateUUID();
			if (asset_manager.GetAssetSheetManager().GetSheet<Material>().sheet.contains(id))
				continue;
			break;
		}

		// アセットシートの登録
		asset_manager.GetAssetSheetManager().GetSheet<Material>().sheet.insert(std::make_pair(id, copy_path.string()));
		return id;
	}

	Material& MaterialLoader::GetMaterial(const mapping::rename_type::UUID& id)
	{
		// Todo : ダミーマテリアルを返すようにする
		if (!materials.contains(id))
			return *dummy_material.get();
			//assert(!"Not found material id(MaterialLoader::GetMaterial)");
		return *materials.at(id).get();
	}
} // cumulonimbus::asset
