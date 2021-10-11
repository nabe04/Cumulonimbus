#include "system.h"

#include <fstream>
#include <functional>

#include "camera_component.h"
#include "file_path_helper.h"
#include "sky_box.h"

CEREAL_CLASS_VERSION(cumulonimbus::system::System, 1)

namespace
{
	const std::string default_path_and_name{ "./Data/System/system" };
}

namespace cumulonimbus::system
{
	//template <class Archive>
	//void System::serialize(Archive&& archive ,std::uint32_t const version)
	//{
	//	archive(
	//		CEREAL_NVP(camera_texture)
	//	);

	//	if(version == 1)
	//	{
	//		archive(
	//			CEREAL_NVP(sky_box)
	//		);
	//	}
	//}

	template <class Archive>
	void System::save(Archive&& archive, std::uint32_t const version) const
	{
		archive(
			CEREAL_NVP(camera_texture)
		);

		if(version >= 0)
		{
			archive(
				CEREAL_NVP(sky_box),
				CEREAL_NVP(current_scene_path),
				CEREAL_NVP(default_scene_path)
			);
		}
	}

	template<class Archive>
	void System::load(Archive&& archive, std::uint32_t const version)
	{
		archive(
			CEREAL_NVP(camera_texture)
		);

		if(version == 1)
		{
			archive(
				CEREAL_NVP(sky_box)
			);
		}
		if(version == 2)
		{
			archive(
				CEREAL_NVP(sky_box),
				CEREAL_NVP(current_scene_path),
				CEREAL_NVP(default_scene_path)
			);
		}
	}

	System::System()
	{
		camera_texture	= std::make_unique<camera::CameraTexture>(*this);
		sky_box			= std::make_unique<graphics::SkyBox>(*this, locator::Locator::GetDx11Device()->device.Get());
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

		// Load時の初期設定
		auto Initialize = [&]()
		{
			if(!camera_texture.get())
				camera_texture = std::make_unique<camera::CameraTexture>(*this);
			if(!sky_box.get())
				sky_box = std::make_unique<graphics::SkyBox>(*this, locator::Locator::GetDx11Device()->device.Get());

			// System::Render内関数の再登録(再読込されたもののみ)
			RegisterRenderFunction(utility::GetHash<camera::CameraTexture>(),
								   [&](ecs::Registry* registry) {camera_texture->RenderImGui(registry); });

			sky_box->Load(*this);
		};

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
				Initialize();
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
		Initialize();
	}

	void System::Update(const float dt)
	{
		sky_box->Update(dt);
		auto a = current_scene_path;
		int b;
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
		return *(camera_texture.get());
	}

	graphics::SkyBox& System::GetSkyBox() const
	{
		return *(sky_box.get());
	}
} // cumulonimbus::system