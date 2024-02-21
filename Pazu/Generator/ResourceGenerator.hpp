// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include <string_view>
#include <iterator>
#include <ranges>
#include "WinAPICommon.hpp"

namespace Pazu
{
    namespace Impl
    {
        struct ResourceGenerator
        {
            constexpr ResourceGenerator() = default;
            constexpr virtual ~ResourceGenerator() = default;
            constexpr ResourceGenerator(const ResourceGenerator &) noexcept = default;
            constexpr ResourceGenerator(ResourceGenerator &&) noexcept = default;

        protected:
            template <typename... Types>
            std::unique_ptr<const char> ModifyFileName(Types... args) const noexcept
                requires Wcm::IsLessEqual<2, Types...> && std::conjunction_v<std::is_convertible<Types, const char *>...> ||
                         Wcm::IsEqual<1, Types...> && Wcm::CharacterStringView<Wcm::At<0, Types...>>;
            void ReplaceMatchCandicate(size_t &i, const char mc, char *const &result) const noexcept;
            ResourceGenerator &operator=(const ResourceGenerator &) noexcept = default;
            ResourceGenerator &operator=(ResourceGenerator &&) noexcept = default;
        };

        struct GenerateResourceFileT final : ResourceGenerator
        {
            bool operator()(char *const resourceName, const char *const resourceFile, const char *const outputFile) const noexcept;
        };
        struct GenerateResourceHandleT final : ResourceGenerator
        {
            bool operator()(std::string_view resourceNames, std::string_view resourceHandleFile) const noexcept;
        };

        struct GenerateResourceFilesT final : ResourceGenerator
        {
            bool operator()(int argc, char *argv[]) const noexcept;
            bool operator()(std::forward_iterator auto beginItr, const std::forward_iterator auto endItr) const noexcept
                requires requires { { std::ranges::cdata(*beginItr) } -> std::convertible_to<const char *>; };
        };

        inline constexpr GenerateResourceFileT GenerateResourceFile{};
        inline constexpr GenerateResourceHandleT GenerateResourceHandle{};
    }

    inline constexpr Impl::GenerateResourceFilesT GenerateResourceFiles{};
}

#include "ResourceGenerator.inl"
