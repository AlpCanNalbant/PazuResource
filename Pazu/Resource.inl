// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace Pazu
{
	constexpr std::optional<std::basic_string_view<unsigned char>> GetResource(const Wcm::ByteCharacterStringLike auto &name) noexcept
	{
		if (const auto pos = Resources.find(Wcm::ToStringView(name).data()); pos != Resources.cend())
		{
			return std::make_optional(pos->second);
		}
		return std::nullopt;
	}
}
