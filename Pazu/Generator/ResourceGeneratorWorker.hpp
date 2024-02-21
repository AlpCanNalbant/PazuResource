// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include <filesystem>
#include <vector>
#include <string>
#include <string_view>

namespace Pazu::Impl
{
    extern const std::filesystem::path DefaultDiskResourceOutputDirectory;
    inline constexpr std::string_view DefaultResourceBaseDirectoryName = "Assets";

    struct ResourceGeneratorWorker
    {
        constexpr ResourceGeneratorWorker() noexcept = default;
        constexpr virtual ~ResourceGeneratorWorker() = default;
        constexpr ResourceGeneratorWorker(const ResourceGeneratorWorker &) noexcept = default;
        constexpr ResourceGeneratorWorker(ResourceGeneratorWorker &&) noexcept = default;
        constexpr ResourceGeneratorWorker &operator=(const ResourceGeneratorWorker &) noexcept = default;
        constexpr ResourceGeneratorWorker &operator=(ResourceGeneratorWorker &&) noexcept = default;

    protected:
        void LoadConfiguration(int &argc, char **&argv) const noexcept;

        inline static std::vector<std::string> BinaryResourceArgumentList, BinaryResourceDirectoryList;
        inline static std::vector<std::string> DiskResourceArgumentList, DiskResourceDirectoryList;
        inline static std::filesystem::path DiskResourceOutputDirectory;
        inline static std::string ResourceBaseDirectoryName;
        inline static const auto ResourceGeneratorConfigurationFile = Wcm::GetBaseDirectory() / "ResourceGenerator.config";
    };
}
