// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace Pazu::Impl
{
    template <typename... Types>
    std::unique_ptr<const char> ResourceGenerator::ModifyFileName(Types... args) const noexcept
        requires Wcm::IsLessEqual<2, Types...> && std::conjunction_v<std::is_convertible<Types, const char *>...> ||
                 Wcm::IsEqual<1, Types...> && Wcm::CharacterStringView<Wcm::At<0, Types...>>
    {
        char *result = reinterpret_cast<char *>(malloc(sizeof(char[MAX_PATH])));
        auto j = 0uz;
        if constexpr (auto &&t = std::forward_as_tuple(args...);
                      Wcm::IsEqual<1, Types...>)
        {
            if constexpr (const auto fileName = std::get<0>(t);
                          !Wcm::ByteCharacterStringView<Wcm::At<0, Types...>>)
            {
                for (auto i = 0uz; fileName[i] != '\0'; ++i, ++j)
                {
                    ReplaceMatchCandicate(j, fileName[i], result); // Operate on string.
                }
            }
            else
            {
                for (auto begin = fileName.begin(); begin != fileName.cend(); ++begin, ++j)
                {
                    ReplaceMatchCandicate(j, *begin, result); // Operate on string_view.
                }
            }
        }
        else
        {
            for (auto begin = std::get<0>(t); begin != std::get<1>(t); ++begin, ++j)
            {
                ReplaceMatchCandicate(j, *begin, result); // Operate on iterators.
            }
        }
        result[j] = '\0';
        return std::unique_ptr<const char>{result};
    }

	bool GenerateResourceFilesT::operator()(std::forward_iterator auto beginItr, const std::forward_iterator auto endItr) const noexcept
        requires requires { { std::ranges::cdata(*beginItr) } -> std::convertible_to<const char *>; }
	{
		std::vector<const char *> argValues;
		for (; beginItr != endItr; ++beginItr)
		{
			argValues.emplace_back(std::ranges::cdata(*beginItr));
		}
		return this->operator()(std::ranges::ssize(argValues), const_cast<char **>(argValues.data()));
	}
}
