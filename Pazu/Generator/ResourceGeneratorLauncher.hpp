// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include "ResourceGeneratorWorker.hpp"

namespace Pazu
{
    namespace Impl
    {
        struct LaunchResourceGeneratorT final : ResourceGeneratorWorker
        {
            bool operator()() const noexcept;
        };
    }

    inline constexpr Impl::LaunchResourceGeneratorT LaunchResourceGenerator{};
}
