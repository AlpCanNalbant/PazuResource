// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#include <fstream>
#include <mutex>
#include "WinAPICommon.hpp"
#include "ResourceGeneratorWorker.hpp"

namespace Pazu::Impl
{
    extern const std::filesystem::path DefaultDiskResourceOutputDirectory = Wcm::GetBaseDirectory();

	void ResourceGeneratorWorker::LoadConfiguration(int &argc, char **&argv) const noexcept
	{
        static std::once_flag loadConfigOnceFlag;
        std::call_once(loadConfigOnceFlag, [&argc, &argv]
        {
            // First try get the configuration values from the file if it's exists.
            if (Wcm::IsFileExists(ResourceGeneratorConfigurationFile))
            {
		        std::ifstream ifs;
		        ifs.open(ResourceGeneratorConfigurationFile);
		        if (ifs.is_open())
                {
		            constexpr std::string_view outputDirSetting{"DiskResourceOutputDirectory="}, resBaseDirNameSetting{"ResourceBaseDirectoryName="};
		            std::string line;
		            while (std::getline(ifs, line))
		            {
		            	if (const auto pos = line.find(outputDirSetting); pos != std::string::npos)
		            	{
		            		DiskResourceOutputDirectory = line.substr(pos + outputDirSetting.size());
		            	}
		            	else if (const auto pos = line.find(resBaseDirNameSetting); pos != std::string::npos)
		            	{
		            		ResourceBaseDirectoryName = line.substr(pos + resBaseDirNameSetting.size());
		            	}
		            }
                    if (ifs.close(); ifs.bad())
                    {
			            Wcm::Log->Error("Error occurred while reading from resource generator configuration file.").Sub("ConfigurationFile", ResourceGeneratorConfigurationFile);
                    }
                }
                else
                {
                    Wcm::Log->Error("Resource generator configuration file stream is cannot opened.").Sub("ConfigurationFile", ResourceGeneratorConfigurationFile);
                }
            }
            // Second try overwrite the configuration values with the argument values.
            if (std::memcmp(*argv, "--", 2) == 0)
            {
		        constexpr std::string_view outputDirArgFlag = "--OutDir";
                constexpr std::string_view resBaseDirNameArgFlag = "--BaseDirName";
                for (int i = 0; i < 2; ++i)
                {
                    if (std::memcmp(*argv, outputDirArgFlag.data(), outputDirArgFlag.size()) == 0)
                    {
                        DiskResourceOutputDirectory = *(++argv)++;
                        ----argc;
                    }
                    else if (std::memcmp(*argv, resBaseDirNameArgFlag.data(), resBaseDirNameArgFlag.size()) == 0)
                    {
                        ResourceBaseDirectoryName = *(++argv)++;
                        ----argc;
                    }
                }
            }
            // Third assign the default values for un-assigned ones.
            if (DiskResourceOutputDirectory.empty())
            {
                DiskResourceOutputDirectory = DefaultDiskResourceOutputDirectory;
            }
            if (ResourceBaseDirectoryName.empty())
            {
                ResourceBaseDirectoryName = DefaultResourceBaseDirectoryName;
            }
        });
	}
}
