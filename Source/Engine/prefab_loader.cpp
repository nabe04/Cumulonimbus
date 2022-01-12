#include "prefab_loader.h"

#include <ranges>

#include "ecs.h"
#include "arithmetic.h"
#include "generic.h"
#include "locator.h"

namespace
{
	const std::string save_parent_path{ "./Data/Assets/Prefabs" };
}

namespace cumulonimbus::asset
{
	mapping::rename_type::UUID PrefabLoader::Convert(
		AssetManager& asset_manager,
		const std::filesystem::path& from,
		const std::filesystem::path& to)
	{
		if (!equivalent(from.parent_path(), to))
		{
			// コピー先のフォルダ作成&コピー
			std::filesystem::copy(
				from, to,
				std::filesystem::copy_options::recursive |
				std::filesystem::copy_options::overwrite_existing);
		}

		const std::string copy_str = to.string() + "/" + from.filename().string();
		const std::filesystem::path copy_path{ copy_str };

		for (const auto& [key, value] : asset_manager.GetAssetSheetManager().GetSheet<Prefab>().sheet)
		{// 同じアセットが存在していた場合そのアセットのID(UUID)を返す
			if (copy_path.compare(value) == 0)
				return key;
		}

		mapping::rename_type::UUID id;
		while (true)
		{// ID(UUID)の重複がないか確認
			id = utility::GenerateUUID();
			if (asset_manager.GetAssetSheetManager().GetSheet<Prefab>().sheet.contains(id))
				continue;
			break;
		}

		// アセットシートの登録
		asset_manager.GetAssetSheetManager().GetSheet<Prefab>().sheet.insert(std::make_pair(id, copy_path.string()));
		return id;
	}

	void PrefabLoader::Load(
		AssetManager& asset_manager,
		const std::filesystem::path& path)
	{
		const auto id = Convert(asset_manager, path, save_parent_path);
		Load(asset_manager, id);
	}

	void PrefabLoader::Load(
		AssetManager& asset_manager,
		const std::filesystem::path& from,
		const std::filesystem::path& to)
	{
		const auto id = Convert(asset_manager, from, to);
		Load(asset_manager, id);
	}

	void PrefabLoader::Load(
		AssetManager& asset_manager,
		const mapping::rename_type::UUID& id)
	{
		// すでにプレファブが存在する場合は処理を抜ける
		if (prefabs.contains(id))
			return;
		const std::string& asset_path = asset_manager.GetAssetSheetManager().GetAssetFilename<Prefab>(id);
		// Prefab作成
		std::unique_ptr<Prefab> prefab = std::make_unique<Prefab>();
		prefab->Load(asset_path);

		prefabs.emplace(id, std::move(prefab));
		// アセットシート(更新後)の保存
		asset_manager.Save();
	}

	void PrefabLoader::Rename(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id, const std::string& changed_name)
	{
		// アセットが存在しない場合処理を抜ける
		if (!prefabs.contains(asset_id))
			return;

		// 変更前のファイルパス(拡張子を含まない)   ./Data/Assets/Prefabs/"変更前のプレハブ名"/"変更前のプレハブ名"
		const std::filesystem::path	before_path = std::filesystem::path{ asset_manager.GetAssetSheetManager().GetAssetFilename<Prefab>(asset_id) }.replace_extension();
		// 変更前の親ファイルパス(拡張子を含まない) ./Data/Assets/Prefabs/"変更前のプレハブ名"
		const std::filesystem::path before_parent_path = before_path.parent_path();

		// 重複確認済みのファイル名取得(拡張子を含まない)
		// asset_name = 重複確認済みのシーン名(ファイル名のみ、拡張子を含まない)
		const std::string asset_name = CompareAndReName<Prefab>(asset_manager,
																before_parent_path.parent_path().string() + "/" +
																changed_name + "/" + changed_name +
																file_path_helper::GetPrefabExtension()
																).filename().replace_extension().string();

		// 変更後のファイルパス(拡張子を含まない)   ./Data/Assets/Prefabs/"変更後のプレハブ名"/"変更後のプレハブ名"
		const std::filesystem::path after_path = before_parent_path.parent_path().string() + "/" + asset_name + "/" + asset_name;
		//
		// -- ファイル & フォルダ名の変更 --//
		// 「.prefab」ファイルのファイル名変更
		// 例 : ./Data/Assets/Prefabs/"変更前のプレハブ名"/"変更前のプレハブ名.prefab" -> ./Data/Assets/Prefabs/"変更前のプレハブ名"/"変更後のプレハブ名".prefab
		std::filesystem::rename(before_path.string()		+ file_path_helper::GetPrefabExtension(),
								before_parent_path.string() + "/" + asset_name + file_path_helper::GetPrefabExtension());
		// フォルダ名の変更(プレハブフォルダ)
		// 例 : ./Data/Assets/"変更前のプレハブ名" -> ./Data/Assets/"変更後のプレハブ名"
		std::filesystem::rename(before_parent_path, after_path.parent_path());

		// アセットシート側のファイルパス変更(例 : ./Data/Materials/"変更後のプレハブ名"/"変更後のプレハブ名.mat")
		asset_manager.GetAssetSheetManager().GetSheet<Prefab>().sheet.at(asset_id) = after_path.string() + file_path_helper::GetPrefabExtension();
		// アセットシートの保存
		asset_manager.Save();
	}

	void PrefabLoader::Delete(AssetManager& asset_manager, const std::filesystem::path& path)
	{
		const mapping::rename_type::UUID prefab_id = asset_manager.GetAssetSheetManager().Search<Prefab>(path);
		// アセット(ID)が存在していなければ処理を抜ける
		if (!prefabs.contains(prefab_id))
			return;

		DeletePrefab(prefab_id, path);
		asset_manager.Save();
	}

	void PrefabLoader::Delete(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id)
	{
		const std::filesystem::path path = asset_manager.GetAssetSheetManager().GetAssetFilename<asset::Prefab>(asset_id);
		// アセット(ID)が存在していなければ処理を抜ける
		if (!prefabs.contains(asset_id))
			return;

		DeletePrefab(asset_id, path);
		asset_manager.Save();
	}

	bool PrefabLoader::Supported(std::filesystem::path extension)
	{
		static const std::set<std::filesystem::path> extensions
		{
			file_path_helper::GetPrefabExtension()
		};

		return extensions.contains(extension);
	}

	mapping::rename_type::UUID PrefabLoader::CreatePrefab(
		AssetManager& asset_manager, ecs::Registry* registry,
		const mapping::rename_type::Entity& ent,
		const bool is_once, const std::string& prefab_name)
	{
		std::filesystem::path save_path{};
		if (prefab_name.empty())
		{
			// 保存先のパスを指定(既に存在していた場合は名前を変更する)
			const std::string path =  save_parent_path + "/" +
									  "New Prefab" + "/" +
									  "New Prefab" +
									  file_path_helper::GetPrefabExtension();
			if(is_once)
			{// アセットシートに既に存在していれば処理を抜ける
				if (asset_manager.GetAssetSheetManager().GetSheet<Prefab>().sheet.contains(path))
					return {};
			}

			// 保存するファイル名の決定
			const std::string save_name  = CompareAndReName<Prefab>(asset_manager, path).filename().replace_extension().string();
			// 保存先のパス(ファイル名、拡張子含む)決定
			save_path = save_parent_path + "/" +
						save_name + "/" +
						save_name +
						file_path_helper::GetPrefabExtension();
		}
		else
		{
			// 保存先のパスを指定(既に存在していた場合は名前を変更する)
			const std::string path = save_parent_path + "/" +
									 prefab_name + "/" +
									 prefab_name +
									 file_path_helper::GetPrefabExtension();
			if (is_once)
			{// アセットシートに既に存在していれば処理を抜ける
				if (asset_manager.GetAssetSheetManager().GetSheet<Prefab>().sheet.contains(path))
					return {};
			}
			// 保存するファイル名の決定
			const std::string save_name = CompareAndReName<Prefab>(asset_manager, path).filename().replace_extension().string();
			// 保存先のパス(ファイル名、拡張子含む)決定
			save_path = save_parent_path + "/" +
						save_name + "/" +
						save_name +
						file_path_helper::GetPrefabExtension();
		}

		std::unique_ptr<Prefab> prefab = std::make_unique<Prefab>();
		// プレファブ作成(エンティティが保持しているコンポーネントの追加)
		//prefab->CreatePrefab(registry, ent, save_path);

		mapping::rename_type::UUID id;
		while (true)
		{
			id = utility::GenerateUUID();
			if (asset_manager.GetAssetSheetManager().GetSheet<Prefab>().sheet.contains(id))
				continue;
			break;
		}

		// すでにプレファブが存在する場合は処理を抜ける
		if (prefabs.contains(id))
			return id;

		prefabs.emplace(id, std::move(prefab));

		// アセットシートの登録
		asset_manager.GetAssetSheetManager().GetSheet<Prefab>().sheet.emplace(id, save_path.string());
		// アセットシート(更新後)の保存
		asset_manager.Save();

		return id;
	}

	mapping::rename_type::UUID PrefabLoader::CreatePrefab(
		AssetManager& asset_manager,
		ecs::Registry* const registry,
		const std::vector<mapping::rename_type::Entity>& entities,
		const bool is_once, const std::string& prefab_name)
	{
		std::filesystem::path save_path{};
		if (prefab_name.empty())
		{
			// 保存先のパスを指定(既に存在していた場合は名前を変更する)
			const std::string path = save_parent_path + "/" +
				"New Prefab" + "/" +
				"New Prefab" +
				file_path_helper::GetPrefabExtension();
			if (is_once)
			{// アセットシートに既に存在していれば処理を抜ける
				if (asset_manager.GetAssetSheetManager().GetSheet<Prefab>().sheet.contains(path))
					return {};
			}

			// 保存するファイル名の決定
			const std::string save_name = CompareAndReName<Prefab>(asset_manager, path).filename().replace_extension().string();
			// 保存先のパス(ファイル名、拡張子含む)決定
			save_path = save_parent_path + "/" +
				save_name + "/" +
				save_name +
				file_path_helper::GetPrefabExtension();
		}
		else
		{
			// 保存先のパスを指定(既に存在していた場合は名前を変更する)
			const std::string path = save_parent_path + "/" +
				prefab_name + "/" +
				prefab_name +
				file_path_helper::GetPrefabExtension();
			if (is_once)
			{// アセットシートに既に存在していれば処理を抜ける
				if (!asset_manager.GetAssetSheetManager().Search<asset::Prefab>(path).empty())
					return {};
			}
			// 保存するファイル名の決定
			const std::string save_name = CompareAndReName<Prefab>(asset_manager, path).filename().replace_extension().string();
			// 保存先のパス(ファイル名、拡張子含む)決定
			save_path = save_parent_path + "/" +
				save_name + "/" +
				save_name +
				file_path_helper::GetPrefabExtension();
		}

		std::unique_ptr<Prefab> prefab = std::make_unique<Prefab>();
		// プレファブ作成(エンティティが保持しているコンポーネントの追加)
		prefab->CreatePrefab(registry, entities, save_path);

		mapping::rename_type::UUID id;
		while (true)
		{// アセットIDの重複を防ぐ
			id = utility::GenerateUUID();
			if (asset_manager.GetAssetSheetManager().GetSheet<Prefab>().sheet.contains(id))
				continue;
			break;
		}

		// すでにプレファブが存在する場合は処理を抜ける
		if (prefabs.contains(id))
			return id;

		prefabs.emplace(id, std::move(prefab));

		// アセットシートの登録
		asset_manager.GetAssetSheetManager().GetSheet<Prefab>().sheet.emplace(id, save_path.string());
		// アセットシート(更新後)の保存
		asset_manager.Save();

		return id;
	}

	mapping::rename_type::Entity PrefabLoader::Instantiate(ecs::Registry* registry, const mapping::rename_type::UUID& prefab_id)
	{
		// Prefabに登録されたIDを持っていなければ処理を抜ける
		if (!prefabs.contains(prefab_id))
			assert(!"Don't have prefab(PrefabLoader::Instanciate)");

		return prefabs.at(prefab_id)->Instantiate(registry);
	}

	void PrefabLoader::ImSelectablePrefab(AssetManager& asset_manager, mapping::rename_type::UUID& prefab_id)
	{
		std::filesystem::path prefab_filename{};
		bool is_dummy = false;
		const asset::AssetSheet& prefab_sheet = asset_manager.GetAssetSheetManager().GetSheet<asset::Prefab>();
		if (prefab_sheet.sheet.contains(prefab_id))
		{
			prefab_filename = prefab_sheet.sheet.at(prefab_id);
			prefab_filename.filename().replace_extension();
			is_dummy = true;
		}
		else
		{
			prefab_filename = "None";
		}

		if (ImGui::BeginCombo("Prefabs", prefab_filename.string().c_str()))
		{
			{// ダミーテクスチャ用
				ImGui::SameLine();
				if (ImGui::Selectable(prefab_filename.string().c_str(),
					is_dummy, 0))
				{
					prefab_id = { "" };
				}
				if (is_dummy)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			for (const auto& [id, tex_filepath] : prefab_sheet.sheet)
			{
				const bool is_selected = (prefab_id == id);

				if (std::filesystem::path asset_filename = tex_filepath;
					ImGui::Selectable(asset_filename.filename().replace_extension().string().c_str(),
									  is_selected, 0))
				{
					prefab_id = id;
				}
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}

	void PrefabLoader::DeletePrefab(const mapping::rename_type::UUID& prefab_id, const std::filesystem::path& delete_path)
	{
		/*
		 * ※注意 : 「delete_path」には拡張子まで含まれたパスが入っている
		 *			 ので必ずこの親階層のパスから削除する
		 */

		// 登録したプレファブ情報の削除
		prefabs.erase(prefab_id);
		// 現プレファブのファイルごと削除
		std::filesystem::remove_all(delete_path.parent_path());
	}
} // cumulonimbus::asset
