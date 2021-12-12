#include "material_loader.h"

#include "asset_sheet_manager.h"
#include "file_path_helper.h"
#include "locator.h"

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

	void MaterialLoader::Rename(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id, const std::string& changed_name)
	{
		// アセットが存在しない場合処理を抜ける
		if (!materials.contains(asset_id))
			return;

		// 変更前のファイルパス(拡張子を含まない)   ./Data/Materials/"変更前のマテリアル名"
		const std::filesystem::path before_path			= std::filesystem::path{ asset_manager.GetAssetSheetManager().GetAssetFilename<asset::Material>(asset_id) }.replace_extension();
		// 変更前の親ファイルパス(拡張子を含まない) ./Data/Materials/
		const std::filesystem::path before_parent_path	= before_path.parent_path();

		// 重複確認済みのファイル名取得(拡張子を含まない)
		// asset_name = 重複確認済みのマテリアル名(ファイル名のみ、拡張子を含まない)
		const std::string asset_name = CompareAndReName<Material>(asset_manager,
																  before_parent_path.string() + "/" +
																  changed_name +file_path_helper::GetMaterialExtension()
																  ).filename().replace_extension().string();

		// 変更後のファイルパス(拡張子を含まない)   ./Data/Materials/"変更後のマテリアル名"
		const std::filesystem::path after_path			= before_parent_path.string() + "/" + asset_name;

		// -- ファイル名の変更 --//
		// 「.mat」ファイルのファイル名変更
		//  例 : ./Data/Materials/"変更前のマテリアル名.mat" ->  ./Data/Materials/"変更後のマテリアル名.mat"
		std::filesystem::rename(before_path.string() + file_path_helper::GetMaterialExtension(),
								after_path.string()  + file_path_helper::GetMaterialExtension());
		// 「.json」ファイルのファイル名変更
		// 例 : ./Data/Materials/"変更前のマテリアル名.json" ->  ./Data/Materials/"変更後のマテリアル名.json"
		std::filesystem::rename(before_path.string() + file_path_helper::GetJsonExtension(),
								after_path.string()  + file_path_helper::GetJsonExtension());

		// アセットシート側のファイルパス変更(例 : ./Data/Materials/"変更後のマテリアル名"/"変更後のマテリアル名.mat")
		asset_manager.GetAssetSheetManager().GetSheet<Material>().sheet.at(asset_id) = after_path.string() + file_path_helper::GetMaterialExtension();
		// アセットシートの保存
		asset_manager.Save();
	}

	void MaterialLoader::Delete(AssetManager& asset_manager, const std::filesystem::path& path)
	{
		const mapping::rename_type::UUID mat_id = asset_manager.GetAssetSheetManager().Search<asset::Material>(path);
		// アセット(ID)が存在していなければ処理を抜ける
		if (!materials.contains(mat_id))
			return;

		DeleteMaterial(mat_id, path);
		asset_manager.Save();
	}

	void MaterialLoader::Delete(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id)
	{
		const std::filesystem::path path = asset_manager.GetAssetSheetManager().GetAssetFilename<asset::Material>(asset_id);
		// アセット(ID)が存在していなければ処理を抜ける
		if (!materials.contains(asset_id))
			return;

		DeleteMaterial(asset_id, path);
		asset_manager.Save();
	}

	bool MaterialLoader::Supported(const std::filesystem::path extension)
	{
		static const std::set<std::filesystem::path> extensions
		{
			".mat"
		};
		return extensions.contains(extension);
	}

	mapping::rename_type::UUID MaterialLoader::CreateMaterial(
		AssetManager& asset_manager, const std::filesystem::path& parent_path,
		const MaterialData& material_data, std::string material_name)
	{
		// マテリアル名が無名の場合「New Material」に名前を設定
		if(material_name == "")
		{
			const std::string path = parent_path.string() + "/Materials" + "/New Material" + file_path_helper::GetMaterialExtension();
			material_name = CompareAndReName<Material>(asset_manager, path).filename().replace_extension().string();
		}
		const std::string save_path{
			parent_path.string() + "/"
			+ "Materials" + "/"
			+ material_name + file_path_helper::GetMaterialExtension() };


		// 保存したマテリアルのIDを作成
		mapping::rename_type::UUID id;
		while (true)
		{
			id = utility::GenerateUUID();
			if (asset_manager.GetAssetSheetManager().GetSheet<Material>().sheet.contains(id))
				continue;
			break;
		}

		// 作成したマテリアルを「.mat」形式で保存
		std::unique_ptr<Material>mat = std::make_unique<Material>(id, material_data);
		mat->Save(save_path);

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
		Material m{ id };
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

	Material& MaterialLoader::GetMaterial(const mapping::rename_type::UUID& mat_id)
	{
		if (!materials.contains(mat_id))
			return *dummy_material.get();
			//assert(!"Not found material id(MaterialLoader::GetMaterial)");
		return *materials.at(mat_id).get();
	}

	std::string MaterialLoader::GetMaterialName(const mapping::rename_type::UUID& mat_id) const
	{
		if (!materials.contains(mat_id))
			return { "" };

		const std::filesystem::path mat_path = locator::Locator::GetAssetManager()->GetAssetSheetManager().GetAssetFilename<Material>(mat_id);

		return mat_path.filename().replace_extension().string();
	}


	void MaterialLoader::DeleteMaterial(const mapping::rename_type::UUID& mat_id, const std::filesystem::path& delete_path)
	{
		// 登録したマテリアル情報の削除
		materials.erase(mat_id);
		// ファイル(.mat)の削除
		std::filesystem::remove(delete_path);
		// ファイル(.json)の削除
		std::filesystem::path p = delete_path;
		p.replace_extension();
		std::filesystem::remove(p.string() + file_path_helper::GetJsonExtension());
	}

} // cumulonimbus::asset
