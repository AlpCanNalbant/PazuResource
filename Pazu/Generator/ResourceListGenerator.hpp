// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include <filesystem>
#include <string_view>
#include "WinAPICommon.hpp"
#include "ResourceGeneratorWorker.hpp"

namespace Pazu
{
    namespace Impl
    {
        struct GenerateResourceListT final : ResourceGeneratorWorker
        {
            bool operator()(int argc, char *argv[]) const noexcept;

        private:
            template <Wcm::StringLiteral TargetList, Wcm::StringLiteral SourceList>
            bool AddResource(const std::string &destPath, const std::string &sourcePath) const noexcept;
            int LoadList() const noexcept;
            bool WriteList() const noexcept;
            void IterateDirectory(const std::filesystem::path &assetsDir, auto &&onSetting) const noexcept;

            inline static constexpr std::string_view BinaryResourceArgumentListVar = "BinaryResourceArgumentList=";
            inline static constexpr std::string_view BinaryResourceDirectoryListVar = "BinaryResourceDirectoryList=";
            inline static constexpr std::string_view DiskResourceArgumentListVar = "DiskResourceArgumentList=";
            inline static constexpr std::string_view DiskResourceDirectoryListVar = "DiskResourceDirectoryList=";
            inline static const auto ResourceListFile = Wcm::GetBaseDirectory() / "ResourceList.txt";
        };

        enum class ResourceLoadMethod
        {
            FromBinary,
            FromDisk
        };
        enum class DiskResourceSetting
        {
            LoadFromBinary,
            LoadFromDisk,
            BinaryResourceLoadPath,
            DiskResourceLoadPath
        };
    }

    inline constexpr Impl::GenerateResourceListT GenerateResourceList{};
}

#include "ResourceListGenerator.inl"
