// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#include "WinAPICommon.hpp"
#include "ResourceGenerator.hpp"
#include "ResourceGeneratorLauncher.hpp"

namespace Pazu::Impl
{
	bool LaunchResourceGeneratorT::operator()() const noexcept
	{
		int updateds = 0;
#if defined(WCM_CPP23) && defined(__cpp_lib_containters_ranges)
		if (DiskResourceArgumentList.append_range(DiskResourceDirectoryList); !DiskResourceArgumentList.empty())
#else
		if (DiskResourceArgumentList.insert(DiskResourceArgumentList.cend(), DiskResourceDirectoryList.cbegin(), DiskResourceDirectoryList.cend()); !DiskResourceArgumentList.empty())
#endif
		{
			for (auto pos = DiskResourceArgumentList.begin(); pos != DiskResourceArgumentList.cend(); ++pos)
			{
				auto destFile = (DiskResourceOutputDirectory / *pos).string();
				auto &srcFile = *(++pos);
				std::replace(destFile.begin(), destFile.end(), '/', '\\');
				std::replace(srcFile.begin(), srcFile.end(), '/', '\\');
				if (bool isError; !Wcm::UpdateFileContent(srcFile, destFile, isError) && !isError)
				{
					Wcm::Log->Info("Disk resource file is up to date.").Sub("ResourceFile", destFile);
				}
				else if (!isError)
				{
					++updateds;
				}
			}
		}
		if (updateds == 0)
		{
			Wcm::Log->Info("All disk resources are up to date.");
		}
#if defined(WCM_CPP23) && defined(__cpp_lib_containters_ranges)
		if (BinaryResourceArgumentList.append_range(BinaryResourceDirectoryList); !BinaryResourceArgumentList.empty())
#else
		if (BinaryResourceArgumentList.insert(BinaryResourceArgumentList.cend(), BinaryResourceDirectoryList.cbegin(), BinaryResourceDirectoryList.cend()); !BinaryResourceArgumentList.empty())
#endif
		{
			updateds += GenerateResourceFiles(BinaryResourceArgumentList.begin(), BinaryResourceArgumentList.cend());
		}
		return (updateds > 0);
    }
}
