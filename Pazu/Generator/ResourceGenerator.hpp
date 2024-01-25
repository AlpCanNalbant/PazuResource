// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.
// Copyright (c) 2024 Johnny Borov <JohnnyBorov@gmail.com>. Released under MIT License.

#pragma once
#include <string_view>
#include "WinAPICommon.hpp"

namespace Pazu
{
    namespace Impl
    {
        struct ResourceGenerator
        {
        protected:
            template <typename... Types>
            std::unique_ptr<const char> ModifyFileName(Types... args) const noexcept
                requires Wcm::IsLessEqual<2, Types...> && std::conjunction_v<std::is_convertible<Types, const char *>...> ||
                         Wcm::IsEqual<1, Types...> && Wcm::CharacterStringView<Wcm::At<0, Types...>>;
            void ReplaceMatchCandicate(size_t &i, const char mc, char *const &result) const noexcept;
        };

        namespace Sub
        {
            struct GenerateResourceFile : ResourceGenerator
            {
                int operator()(char *const resourceName, const char *const resourceFile, const char *const outputFile) const noexcept;
            };
            struct GenerateResourceHandle : ResourceGenerator
            {
                int operator()(std::string_view resourceNames, std::string_view resourceHandleFile) const noexcept;
            };
        }

        struct GenerateResourceFiles : ResourceGenerator
        {
            int operator()(int argc, char *argv[]) const noexcept;
        };

        inline constexpr Sub::GenerateResourceFile GenerateResourceFile{};
        inline constexpr Sub::GenerateResourceHandle GenerateResourceHandle{};
    }

    inline constexpr Impl::GenerateResourceFiles GenerateResourceFiles{};
}

#include "ResourceGenerator.inl"
