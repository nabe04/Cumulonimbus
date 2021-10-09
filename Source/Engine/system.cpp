#include "system.h"

#include <fstream>

#include "camera_component.h"
#include "file_path_helper.h"

namespace
{
	const std::string default_path_and_name{ "./Data/System/system" };
}

namespace cumulonimbus::system
{
	template <class Archive>
	void System::serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(camera_texture)
		);
	}

	System::System()
	{
		camera_texture = std::make_unique<camera::CameraTexture>(*this);
	}

	void System::Save(const std::filesystem::path& save_path)
	{
		if (save_path.compare({ "" }) == 0)
		{// デフォルトのパスに保存
			std::ofstream ofs(default_path_and_name + file_path_helper::GetSystemExtension(), std::ios_base::binary);
			cereal::BinaryOutputArchive output_archive(ofs);
			output_archive(*this);
		}
		else
		{
			std::ofstream ofs(save_path, std::ios_base::binary);
			cereal::BinaryOutputArchive output_archive(ofs);
			output_archive(*this);
		}
	}

	void System::Load(const std::filesystem::path& load_path)
	{
		static const std::set<std::filesystem::path> extension
		{
			file_path_helper::GetSystemExtension()
		};

		std::filesystem::create_directory(std::filesystem::path{ default_path_and_name }.parent_path());

		if(load_path.empty())
		{// デフォルトのフォルダにある「.sys」の読み込み
			std::filesystem::path default_path{};
			for (const auto& path : std::filesystem::directory_iterator(std::filesystem::path{ default_path_and_name }.parent_path()))
			{
				default_path = path;
				if (!extension.contains(default_path.extension()))
					continue;
				{
					std::ifstream ifs(default_path, std::ios_base::binary);
					if (!ifs)
						assert(!"Not found file(System::Load)");
					cereal::BinaryInputArchive input_archive(ifs);
					input_archive(*this);
				}
				// System::Render内関数の再登録(再読込されたもののみ)
				RegisterRenderFunction(utility::GetHash<camera::CameraTexture>(),
									   [&](ecs::Registry* registry) {camera_texture->RenderImGui(registry); });
				return;
			}
			// ファイルパスが存在しなかったので現在の情報で保存
			Save();
			return;
		}

		// 任意のフォルダからの読み込み
		if (!extension.contains(load_path.extension()))
			assert(!"The extension is incorrect(System::Load)");
		{
			std::ifstream ifs(load_path);
			if (!ifs)
				assert(!"Not found file(System::Load)");
			cereal::BinaryInputArchive input_archive(ifs);
			input_archive(*this);
		}
		// System::Render内関数の再登録(再読込されたもののみ)
		RegisterRenderFunction(utility::GetHash<camera::CameraTexture>(),
							   [&](ecs::Registry* registry) {camera_texture->RenderImGui(registry); });
	}


	void System::Render(ecs::Registry* registry)
	{
		for(auto& [hash,render_function] : render_functions)
		{
			render_function(registry);
		}
	}

	void System::RegisterRenderFunction(
		const mapping::rename_type::Hash& hash,
		const std::function<void(ecs::Registry* registry)>& render_func)
	{
		// 型がすでに登録されていた場合上書きする
		if (render_functions.contains(hash))
			render_functions.at(hash) = render_func;

		render_functions.emplace(hash, render_func);
	}

	camera::CameraTexture& System::GetCameraTexture() const
	{
		return *camera_texture.get();
	}

} // cumulonimbus::system