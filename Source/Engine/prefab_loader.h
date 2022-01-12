#pragma once
#include <map>
#include <memory>

#include "loader.h"
#include "rename_type_mapping.h"
#include "prefab.h"

namespace cumulonimbus::ecs
{
	class Registry;
} // cumulonimbus::ecs

namespace cumulonimbus::asset
{
	class PrefabLoader final : public Loader
	{
	public:
		explicit PrefabLoader()  = default;
		~PrefabLoader() override = default;

		/**
		 * @brief : アセットのロード
		 * @param asset_manager : AssetManagerクラスの参照
		 * @param path : ロードされたモデルのファイルパス
		 */
		void Load(AssetManager& asset_manager, const std::filesystem::path& path) override;
		/**
		 * @brief : アセットのロード
		 * @param asset_manager : AssetManagerクラスの参照
		 * @param from: ロードされたモデルのファイルパス
		 * @param to : コピー先のファイルパス
		 */
		void Load(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) override;
		/**
		 * @brief : オーバーロードされているLoad関数の共通処理記述部
		 */
		void Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id) override;

		/**
		 * @brief : アセットの名前変更
		 * @remark : 変更先のクラスでasset_idが存在しない場合処理を抜ける
		 * @remark : プレハブアセットには「.prefab」とプレハブ名の
		 *			 フォルダが存在するのでこれらのファイル名を変更する
		 * @param asset_manager : AssetManagerクラスの参照
		 * @param asset_id : リネーム対象のアセットID
		 * @param changed_name : 変更後のファイル名(※ファイルパスや拡張子は含まない)
		 */
		void Rename(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id, const std::string& changed_name) override;

		/**
		 * @brief : 削除したいファイルパスからアセットを削除
		 * @param asset_manager :
		 * @param path : 削除したいファイルパス(拡張子「.prefab」までを含む)
		 */
		void Delete(AssetManager& asset_manager, const std::filesystem::path& path) override;
		/**
		 * @brief : 削除したいプレファブアセットIDからアセットを削除
		 * @param asset_manager :
		 * @param asset_id : 削除したいアセットID
		 */
		void Delete(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id) override;

		/**
		 * @brief : 指定された拡張子はロード可能か
		 * @param extension : ファイルの拡張子
		 * @return	: true -> サポートしている
		 * @return  : false -> サポートしていない
		 */
		[[nodiscard]]
		bool Supported(std::filesystem::path extension) override;

		/**
		 * @brief : プレファブの作成
		 * @ramark : is_once(第4引数)をfalseにした場合アセットシート
		 *			 に登録されているパスと保存したいパスが同じ場合
		 *			 プレファブの追加は行われない
		 * @param asset_manager : AssetManagerクラスの参照
		 * @param registry :
		 * @param ent : プレファブ化したいエンティティ
		 * @param is_once : プレファブの重複を許すか
		 * @param prefab_name : プレファブ名
		 * @return : 作成されたプレファブのID(作成されなかった場合は空文字を返す)
		 */
		mapping::rename_type::UUID CreatePrefab(
			AssetManager& asset_manager, ecs::Registry* registry,
			const mapping::rename_type::Entity& ent,
			bool is_once = {false},
			const std::string& prefab_name = {""});

		/**
		 * @brief : プレファブの作成
		 * @ramark : is_once(第4引数)をfalseにした場合アセットシート
		 *			 に登録されているパスと保存したいパスが同じ場合
		 *			 プレファブの追加は行われない
		 * @param asset_manager : AssetManagerクラスの参照
		 * @param registry :
		 * @param entities : プレファブ化したいエンティティ郡
		 * @param is_once : プレファブの重複を許すか
		 * @param prefab_name : プレファブ名
		 * @return : 作成されたプレファブのID(作成されなかった場合は空文字を返す)
		 */
		mapping::rename_type::UUID CreatePrefab(
			AssetManager& asset_manager, ecs::Registry* registry,
			const std::vector<mapping::rename_type::Entity>& entities,
			bool is_once = {false},
			const std::string& prefab_name = {""});

		/**
		 * @brief : プレファブからコンポーネントの追加
		 * @remark : プレファブIDがなければ処理を中断
		 * @param registry :
		 * @param prefab_id : プレファブのアセットID(UUID)
		 * @return : 作成された
		 */
		[[nodiscard]]
		mapping::rename_type::Entity Instantiate(
			ecs::Registry* registry,
			const mapping::rename_type::UUID& prefab_id);

		/**
		 * @brief : ImGui上でのプレハブ選択関数
		 * @remark : ※caution(1) : ImGuiを使用する関数内で使用すること
		 * @remark : ※caution(2) : ImGui::Begin()の中でこの関数を呼ぶこと
		 * @param asset_manager :
		 * @param prefab_id : 格納されるプレハブID(UUID)
		 */
		void ImSelectablePrefab(AssetManager& asset_manager, mapping::rename_type::UUID& prefab_id);
	private:
		std::map<mapping::rename_type::UUID, std::unique_ptr<Prefab>> prefabs{};

		mapping::rename_type::UUID Convert(
			AssetManager& asset_manager,
			const std::filesystem::path& from,
			const std::filesystem::path& to) override;

		/**
		 * @brief : プレファブファイルの削除
		 * @remark : 「.prefab」の持つフォルダごと削除する
		 * @param prefab_id : アセットシートに登録されているプレファブID
		 * @param delete_path : 削除したいファイルパス(「.prefab」拡張子までを含むパス)
		 */
		void DeletePrefab(
			const mapping::rename_type::UUID& prefab_id,
			const std::filesystem::path& delete_path);
	};
} // cumulonimbus::asset