// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace Pazu::Impl
{
	template <Wcm::StringLiteral TargetList, Wcm::StringLiteral SourceList>
	bool GenerateResourceListT::AddResource(const std::string &destPath, const std::string &sourcePath) const noexcept
	{
		/*static*/ const auto listAdd = [&destPath, &sourcePath] (auto &list, const auto &list2) -> bool
		{
#ifdef WCM_CPP23
			if (!std::ranges::contains(list, destPath) && !std::ranges::contains(list2, destPath))
#else
			if (std::ranges::find(list, destPath) == list.cend() && std::ranges::find(list2, destPath) == list2.cend())
#endif
			{
				list.insert(list.cend(), {destPath, sourcePath});
				return true;
			}
			return false;
		};
		if constexpr (TargetList == "BinaryList")
		{
			if constexpr (SourceList == "ArgumentList")
			{
				return listAdd(BinaryResourceArgumentList, BinaryResourceDirectoryList);
			}
			else if constexpr (SourceList == "DirectoryList")
			{
				return listAdd(BinaryResourceDirectoryList, BinaryResourceArgumentList);
			}
		}
		else if constexpr (TargetList == "DiskList")
		{
			if constexpr (SourceList == "ArgumentList")
			{
				return listAdd(DiskResourceArgumentList, DiskResourceDirectoryList);
			}
			else if constexpr (SourceList == "DirectoryList")
			{
				return listAdd(DiskResourceDirectoryList, DiskResourceArgumentList);
			}
		}
	}

    void GenerateResourceListT::IterateDirectory(const std::filesystem::path &assetsDir, auto &&onSetting) const noexcept
    {
		constexpr std::string_view binLoadSetting{"LoadFromBinary="}, diskLoadSetting{"LoadFromDisk="},
								   binPathSetting{"BinaryResourceLoadPath="}, diskPathSetting{"DiskResourceLoadPath="};
		std::ifstream ifs;
		std::string line;
		for (const auto &item : std::filesystem::recursive_directory_iterator(assetsDir))
		{
			if (item.is_regular_file())
			{
				if (const auto resInfoFile = item.path().native() + L".res"; std::filesystem::is_regular_file(resInfoFile))
				{
					ifs.open(resInfoFile.data());
					if (!ifs.is_open())
					{
						Wcm::Log->Error("Resource information file is cannot opened.").Sub("ResourceInfoFile", resInfoFile);
						continue;
					}
					std::unordered_map<DiskResourceSetting, std::string> diskResSettingPaths;
					while (std::getline(ifs, line))
					{
						if (const auto pos = line.find(binLoadSetting); pos != std::string::npos)
						{
							if (Wcm::IsSameString(line.substr(pos + binLoadSetting.size(), 4), "True"))
							{
								diskResSettingPaths[DiskResourceSetting::LoadFromBinary] = item.path().string();
							}
						}
						else if (const auto pos = line.find(diskLoadSetting); pos != std::string::npos)
						{
							if (Wcm::IsSameString(line.substr(pos + diskLoadSetting.size(), 4), "True"))
							{
								diskResSettingPaths[DiskResourceSetting::LoadFromDisk] = item.path().string();
							}
						}
						else if (const auto pos = line.find(binPathSetting); pos != std::string::npos)
						{
							if (const auto binResLoadPath = line.substr(pos + binPathSetting.size()); !binResLoadPath.empty())
							{
								diskResSettingPaths[DiskResourceSetting::BinaryResourceLoadPath] = binResLoadPath;
							}
						}
						else if (const auto pos = line.find(diskPathSetting); pos != std::string::npos)
						{
							if (const auto diskResLoadPath = line.substr(pos + diskPathSetting.size()); !diskResLoadPath.empty())
							{
								diskResSettingPaths[DiskResourceSetting::DiskResourceLoadPath] = diskResLoadPath;
							}
						}
					}
					if (ifs.close(); ifs.bad())
        			{
						Wcm::Log->Error("Error occurred while reading from resource information file.").Sub("ResourceInfoFile", resInfoFile);
        			}
					onSetting(diskResSettingPaths);
				}
			}
		}
    }
}
