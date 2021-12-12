#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "loader.h"

namespace cumulonimbus::asset
{
	class Texture;
} // cumulonimbus::asset

namespace cumulonimbus::asset
{
	class TextureLoader final : public Loader
	{
	public:
		explicit TextureLoader();
		~TextureLoader() override = default;

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
		 * @remark : idに登録されているアセットの作成のみを行う
		 *			 ※フォルダのコピー、作成などは行わない
		 */
		void Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id) override;

		/**
		 * @brief : アセットの名前変更
		 * @remark : 変更先のクラスでasset_idが存在しない場合処理を抜ける
		 * @remark : テクスチャアセットにはテクスチャファイルの変更する
		 * @param asset_manager : AssetManagerクラスの参照
		 * @param asset_id : リネーム対象のアセットID
		 * @param changed_name : 変更後のファイル名(※ファイルパスや拡張子は含まない)
		 */
		void Rename(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id, const std::string& changed_name) override;

		/**
		 * @brief : アセットの削除
		 * @remark : ※caution : 削除したいパスが存在しない場合処理を飛ばす
		 * @param asset_manager : AssetManagerクラスの参照
		 * @param path : 削除したいファイルパス
		 */
		void Delete(AssetManager& asset_manager, const std::filesystem::path& path) override;
		/**
		 * @breief : アセットの削除
		 * @remark :  ※caution : 削除したいIDが存在しない場合処理を飛ばす
		 * @param asset_manager : AssetManagerクラスの参照
		 * @param asset_id : 削除したいアセットのID(UUID)
		 */
		void Delete(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id) override;

		/**
		 * @brief : 指定された拡張子はロード可能か
		 * @param extension : ファイルの拡張子
		 * @return	: true -> サポートしている
		 * @return  : false -> サポートしていない
		 */
		bool Supported(std::filesystem::path extension) override;

		/**
		 * @brief : テクスチャアセットを保持しているかを
		 *			テクスチャIDから判別
		 * @param tex_id : テクスチャID
		 */
		[[nodiscard]]
		bool HasTexture(const mapping::rename_type::UUID& tex_id) const;

		/**
		 * @brief : ファイルパスからテクスチャの作成
		 * @remark : ※Load関数との違い
		 * @remark : フォルダのコピーを行わずpathに指定された
		 *			 ファイルパスを元にテクスチャを作成する
		 * @param asset_manager : AssetManagerクラスの参照
		 * @param path : テクスチャまでのファイルパス(※拡張子含む)
		 */
		void CreateTexture(AssetManager& asset_manager, const std::filesystem::path& path);

		/**
		 * @brief : 取得したいテクスチャのID(UUID)を元にテクスチャを取得
		 * @param id : 取得したいテクスチャのID(UUID)
		 * @return : 取得したテクスチャの参照
		 */
		Texture& GetTexture(const mapping::rename_type::UUID& id);

		/**
		 * @brief : TextureLoaderが持つテクスチャ全ての取得
		 */
		const std::map<mapping::rename_type::UUID, std::unique_ptr<Texture>>& GetTextures() const;

		/**
		 * @brief : TextureLoaderが持つテクスチャ全ての取得
		 */
		std::map<mapping::rename_type::UUID, std::unique_ptr<Texture>>& GetTextures();

		/**
		 * @brief : ImGui上でのテクスチャ選択関数
		 * @remark : ※caution(1) : ImGuiを使用する関数内で使用すること
		 * @remark : ※caution(2) : ImGui::Begin()の中でこの関数を呼ぶこと
		 * @param asset_manager :
		 * @param tex_id : 格納されるテクスチャID(UUID)
		 */
		void SelectableTexture(AssetManager& asset_manager, mapping::rename_type::UUID& tex_id);
	private:
		std::map<mapping::rename_type::UUID, std::unique_ptr<Texture>> textures{};
		std::unique_ptr<Texture> dummy_texture{};
		//-- ImGui用変数 --//
		DirectX::SimpleMath::Vector2 selectable_image_size{ 100.f,100.f };	// テクスチャ選択の画像サイズ
		DirectX::SimpleMath::Vector2 selectable_size{ 500,50 }; // ImGui::Selectableサイズ
		float selectable_magnification = 1.0f; // テクスチャ選択項目のサイズ倍率

		mapping::rename_type::UUID Convert(
			AssetManager& asset_manager,
			const std::filesystem::path& from,
			const std::filesystem::path& to) override;

		/**
		 * @brief : テクスチャアセット、ファイルの削除
		 * @param tex_id : 削除したいテクスチャID(UUID)
		 * @param delete_path : 削除したいテクスチャパス
		 */
		void DeleteTexture(
			const mapping::rename_type::UUID& tex_id,
			const std::filesystem::path& delete_path);
	};
} // cumulonimbus::asset