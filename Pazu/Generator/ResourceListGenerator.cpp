// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#include <fstream>
#include "ResourceListGenerator.hpp"

namespace Pazu::Impl
{
	bool GenerateResourceListT::operator()(int argc, char *argv[]) const noexcept
	{
		if (const auto baseDir = Wcm::GetBaseDirectory().string(); Wcm::UnorderedContains(*argv, baseDir.c_str()))
		{
			++argv;
			--argc;
		}
		LoadConfiguration(argc, argv);
		if (const int result = LoadList(); result <= 0)
		{
			if (result == -2)
			{
				return false;
			}
			else if (result == -1 && argc == 0)
			{
				Wcm::Log->Error("Resource list is not exists in resource list only mode.");
				return false;
			}
			else if (result == 0 && argc == 0)
			{
				Wcm::Log->Error("Resource list is empty in an argumentless execution.");
				return false;
			}
		}
		if (argc == 0)
		{
			return true;
		}
		const auto toResLoadPath = [](const std::string &path) -> std::string
		{
			return Wcm::CutPath(path, ResourceBaseDirectoryName, true).string();
		};
		int i{}, updateds{};
		bool isSrcPathOnly = false;
		while (i < argc)
		{
			if (Wcm::IsSameString(argv[i], "--ResList"))
			{
				auto loadMethod = ResourceLoadMethod::FromBinary;
				for (++i; i < argc; ++i)
				{
					if (const auto argView = Wcm::ToStringView(argv[i]); argView.starts_with("--"))
					{
						break;
					}
					else if (argView.starts_with('-'))
					{
						if (Wcm::IsSameString(argView, "-SrcPath"))
						{
							isSrcPathOnly = true;
							continue;
						}
						if (Wcm::IsSameString(argView, "-DstSrcPath"))
						{
							isSrcPathOnly = false;
							continue;
						}
						if (Wcm::IsSameString(argView, "-Bin")) // Or -bin is allowed, it's a case-insensitive check.
						{
							loadMethod = ResourceLoadMethod::FromBinary;
							continue;
						}
						if (Wcm::IsSameString(argView, "-Disk"))
						{
							loadMethod = ResourceLoadMethod::FromDisk;
							continue;
						}
						// Before assign its value as an sub-log message argument, remove its prefix which is '-' via incrementing its address '++argv'.
						Wcm::Log->Error("Invalid resource load setting.").Sub("LoadSetting", ++(argv[i]));
						return false;
					}
					switch (loadMethod)
					{
					case ResourceLoadMethod::FromBinary:
						updateds += AddResource<"BinaryList", "ArgumentList">(isSrcPathOnly ? toResLoadPath(argv[i]) : argv[i], argv[i + !isSrcPathOnly]);
						i += !isSrcPathOnly;
						break;
					case ResourceLoadMethod::FromDisk:
						updateds += AddResource<"DiskList", "ArgumentList">(isSrcPathOnly ? toResLoadPath(argv[i]) : argv[i], argv[i + !isSrcPathOnly]);
						i += !isSrcPathOnly;
						break;
					}
				}
			}
			else if (Wcm::IsSameString(argv[i], "--DirList"))
			{
				for (++i; i < argc; ++i)
				{
					if (Wcm::ToStringView(argv[i]).starts_with("--"))
					{
						break;
					}
					IterateDirectory(argv[i], [this, &updateds, &toResLoadPath] (const std::unordered_map<DiskResourceSetting, std::string> &diskResSettingPaths)
					{
						if (const auto loadFromBinPos = diskResSettingPaths.find(DiskResourceSetting::LoadFromBinary); loadFromBinPos != diskResSettingPaths.cend())
						{
							const auto binResLoadPathPos = diskResSettingPaths.find(DiskResourceSetting::BinaryResourceLoadPath);
							updateds += AddResource<"BinaryList", "DirectoryList">(
								binResLoadPathPos != diskResSettingPaths.cend() ? binResLoadPathPos->second
								: toResLoadPath(loadFromBinPos->second), loadFromBinPos->second);
						}
						if (const auto loadFromDiskPos = diskResSettingPaths.find(DiskResourceSetting::LoadFromDisk); loadFromDiskPos != diskResSettingPaths.cend())
						{
							const auto diskResLoadPathPos = diskResSettingPaths.find(DiskResourceSetting::DiskResourceLoadPath);
							updateds += AddResource<"DiskList", "DirectoryList">(
								diskResLoadPathPos != diskResSettingPaths.cend() ? diskResLoadPathPos->second
								: toResLoadPath(loadFromDiskPos->second), loadFromDiskPos->second);
						}
					});
				}
			}
			else
			{
				Wcm::Log->Error("Invalid list type.").Sub("ListType", ++++(argv[0]));
				return false;
			}
		}
		if (updateds == 0)
		{
			Wcm::Log->Info("Resource lists are up to date.");
		}
		WriteList(); // Producing a non-fatal error, just ignore its result.
		return true;
    }

	int GenerateResourceListT::LoadList() const noexcept
	{
		if (!Wcm::IsFileExists(ResourceListFile))
		{
			return -1;
		}
		std::ifstream ifs;
		ifs.open(ResourceListFile);
		if (!ifs.is_open())
        {
			Wcm::Log->Error("Resource list file stream is cannot opened.").Sub("ResourceListFile", ResourceListFile);
        	return -2;
        }
		std::string line;
		std::getline(ifs, line);
		int updateds = 0;
		do
		{
			if (line.starts_with(BinaryResourceArgumentListVar))
			{
				while (std::getline(ifs, line) && line.starts_with('\t'))
				{
					const auto binResPaths = Wcm::ToUnquoteds(line.substr(1));
					updateds += AddResource<"BinaryList", "ArgumentList">(binResPaths[0], binResPaths[1]);
				}
				continue;
			}
			else if (line.starts_with(BinaryResourceDirectoryListVar))
			{
				while (std::getline(ifs, line) && line.starts_with('\t'))
				{
					const auto binResPaths = Wcm::ToUnquoteds(line.substr(1));
					updateds += AddResource<"BinaryList", "DirectoryList">(binResPaths[0], binResPaths[1]);
				}
				continue;
			}
			else if (line.starts_with(DiskResourceArgumentListVar))
			{
				while (std::getline(ifs, line) && line.starts_with('\t'))
				{
					const auto diskResPaths = Wcm::ToUnquoteds(line.substr(1));
					updateds += AddResource<"DiskList", "ArgumentList">(diskResPaths[0], diskResPaths[1]);
				}
				continue;
			}
			else if (line.starts_with(DiskResourceDirectoryListVar))
			{
				while (std::getline(ifs, line) && line.starts_with('\t'))
				{
					const auto diskResPaths = Wcm::ToUnquoteds(line.substr(1));
					updateds += AddResource<"DiskList", "DirectoryList">(diskResPaths[0], diskResPaths[1]);
				}
				continue;
			}
		} while (std::getline(ifs, line));
		if (ifs.close(); ifs.bad())
        {
			Wcm::Log->Error("Error occurred while reading from resource list file.").Sub("ResourceListFile", ResourceListFile);
        	return (updateds == 0) ? -2 : updateds;
        }
		return updateds;
	}

    bool GenerateResourceListT::WriteList() const noexcept
    {
		std::ofstream ofs;
		ofs.open(ResourceListFile); // Overwrite the list file instead of append to it.
		if (!ofs.is_open())
        {
			Wcm::Log->Error("Resource list file stream is cannot opened.").Sub("ResourceListFile", ResourceListFile);
        	return false;
        }
		ofs << BinaryResourceArgumentListVar << '\n';
		for (auto begin = BinaryResourceArgumentList.begin(); begin != BinaryResourceArgumentList.cend(); ++begin)
		{
			ofs << '\t' << Wcm::ToQuoted(*begin) << ' ' << Wcm::ToQuoted(*(++begin)) << '\n';
		}
		ofs << BinaryResourceDirectoryListVar << '\n';
		for (auto begin = BinaryResourceDirectoryList.begin(); begin != BinaryResourceDirectoryList.cend(); ++begin)
		{
			ofs << '\t' << Wcm::ToQuoted(*begin) << ' ' << Wcm::ToQuoted(*(++begin)) << '\n';
		}
		ofs << DiskResourceArgumentListVar << '\n';
		for (auto begin = DiskResourceArgumentList.begin(); begin != DiskResourceArgumentList.cend(); ++begin)
		{
			ofs << '\t' << Wcm::ToQuoted(*begin) << ' ' << Wcm::ToQuoted(*(++begin)) << '\n';
		}
		ofs << DiskResourceDirectoryListVar << '\n';
		for (auto begin = DiskResourceDirectoryList.begin(); begin != DiskResourceDirectoryList.cend(); ++begin)
		{
			ofs << '\t' << Wcm::ToQuoted(*begin) << ' ' << Wcm::ToQuoted(*(++begin)) << '\n';
		}
		if (ofs.close(); ofs.bad())
		{
			Wcm::Log->Error("Error occurred while writing to resource list file.").Sub("ResourceListFile", ResourceListFile);
			return false;
		}
		return true;
    }
}
