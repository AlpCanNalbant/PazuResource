// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.
// Copyright (c) 2024 Johnny Borov <JohnnyBorov@gmail.com>. Released under MIT License.

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

	constexpr std::optional<std::basic_string_view<unsigned char>> GetResource(const Wcm::ByteCharacterStringLike auto &name) noexcept
	{
		if (const auto pos = Resources.find(Wcm::ToStringView(name).data()); pos != Resources.cend())
		{
			return std::make_optional(pos->second);
		}
		return std::nullopt;
	}
}
