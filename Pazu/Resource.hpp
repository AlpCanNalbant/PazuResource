// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include <string_view>
#include <unordered_map>
#include <optional>
#include "WinAPICommon.hpp"

namespace Pazu
{
	[[nodiscard]] constexpr std::optional<std::basic_string_view<unsigned char>> GetResource(const Wcm::ByteCharacterStringLike auto &name) noexcept;

	namespace
	{
		inline const std::unordered_map<std::string_view, std::basic_string_view<unsigned char>> Resources;
	}
}

#include "Resource.inl"
