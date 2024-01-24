// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.
// Copyright (c) 2024 Johnny Borov <JohnnyBorov@gmail.com>. Released under MIT License.

#include <fstream>
#include <algorithm>
#include <filesystem>
#include "WinAPICommon.hpp"
#include "ResourceGenerator.hpp"
#include "ResourceNames.hpp"

namespace Pazu::Impl
{
	int GenerateResourceFiles::operator()(int argc, char *argv[]) const noexcept
	{
		int updateds = 0;
		std::string resourceNames;
		const auto parentDir = Wcm::GetSourceDirectory().parent_path();
		const auto resourcesDir = parentDir / "Resources";
		for (auto i = 1; i < argc; i += 2)
		{
			const auto resName = argv[i];
			const auto resFile = argv[i + 1];
			const auto resOutFile = (resourcesDir / resName).concat(".hpp").string();
			updateds += GenerateResourceFile(resName, resFile, resOutFile.c_str());
			resourceNames.append(resName).append(";");
		}
		resourceNames.pop_back(); // Remove the last semicolon delimiter.
		if (updateds == 0)
		{
			Wcm::Log->Info("All resources are up to date.");
			return EXIT_FAILURE;
		}
		const auto resHandleFile = (parentDir / "Resource.hpp");
		return GenerateResourceHandle(resourceNames.c_str(), resHandleFile.string());
	}

	namespace Sub
	{
		int GenerateResourceFile::operator()(char *const resourceName, const char *const resourceFile, const char *const outputFile) const noexcept
		{
			const auto cacheBackupFile = Wcm::GetSourceDirectory() / "Cache" / resourceName;
			if (!Wcm::MakeDirectories(cacheBackupFile.parent_path()))
			{
				Wcm::Log->Error("Base directory of resource cache is cannot created.", GetLastError()).Sub("Cache", cacheBackupFile.string());
			}
			else if (!Wcm::UpdateFileContent(resourceFile, cacheBackupFile))
			{
				Wcm::Log->Info("Resource is up to date.").Sub("Resource", resourceName);
				return 0;
			}

			const auto modifiedName = ModifyFileName(resourceName);

			std::ifstream ifs{resourceFile, std::ios::binary};
			if (!ifs.is_open())
			{
				Wcm::Log->Error("Resource file is cannot opened.").Sub("ResourceFile", resourceFile);
				return 0;
			}

			if (!Wcm::MakeDirectories(Wcm::ToBaseDirectory(outputFile)))
			{
				Wcm::Log->Error("Base directory of resource output file is cannot created.", GetLastError()).Sub("OutputFile", outputFile);
				return 0;
			}
			std::ofstream ofs{outputFile};
			if (!ofs.is_open())
			{
				Wcm::Log->Error("Resource output file is cannot created.").Sub("OutputFile", outputFile);
				return 0;
			}

			ofs << "// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.\n"
				   "// Copyright (c) 2024 Johnny Borov <JohnnyBorov@gmail.com>. Released under MIT License.\n"
				   "\n"
				   "#pragma once\n"
				   "\n"
				   "namespace Pazu\n"
				   "{\n"
				   "\tinline constexpr unsigned char _resource_" << modifiedName.get() << "_data[]\n"
				   "\t{\n"
				   "\t\t";

			int lineCount = 0;
			char c;
			while (ifs.get(c))
			{
				ofs << "0x" << std::hex << (c & 0xff) << ", ";

				if (++lineCount == 40)
				{
					ofs << "\n"
						   "\t\t";
					lineCount = 0;
				}
			}

			ofs << "\'\\0\'\n"
				   "\t};\n"
				   "}\n";

			if (ifs.bad())
			{
				Wcm::Log->Error("Error occurred while reading from resource file.").Sub("ResourceFile", resourceFile);
				return 0;
			}
			if (ofs.bad())
			{
				Wcm::Log->Error("Error occurred while writing to resource output file.").Sub("OutputFile", outputFile);
				return 0;
			}

			Wcm::Log->Info("Output of resource file is successfully created.").Sub("ResourceFile", resourceFile).Sub("OutputFile", outputFile);
			return 1;
		}

		int GenerateResourceHandle::operator()(std::string_view resourceNames, std::string_view resourceHandleFile) const noexcept
		{
			if (resourceNames.size() == ResourceNames.size() && memcmp(resourceNames.data(), ResourceNames.data(), sizeof(char) * resourceNames.size()))
			{
				Wcm::Log->Info("Resources are up to date.");
				return EXIT_FAILURE;
			}
			else
			{
				const auto resourceNamesFile = Wcm::GetSourceDirectory() / "ResourceNames.hpp";
				std::ofstream ofs{resourceNamesFile};
				if (ofs.is_open())
				{
					ofs << "// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.\n"
						   "// Copyright (c) 2024 Johnny Borov <JohnnyBorov@gmail.com>. Released under MIT License.\n"
						   "\n"
						   "#pragma once\n"
						   "\n"
				   		   "namespace Pazu\n"
				   		   "{\n"
						   "\tinline constexpr std::string_view ResourceNames =\n"
						   "\t\t\"";

					if (resourceNames.length() <= 150)
					{
						ofs << resourceNames;
					}
					else
					{
						for (auto i = 0uz; i < resourceNames.length(); ++i)
						{
							const auto c = resourceNames[i];
							ofs << c;
							if ((i % 150uz) == 0uz && c == ';' && (i + 1uz) < resourceNames.length())
							{
								ofs << "\n"
									   "\t\t";
							}
						}
					}
					ofs << "\";\n"
						   "}\n";

 					Wcm::Log->Info("Source file of resource names is successfully created.").Sub("ResourceNamesFile", resourceNamesFile.string());
				}
				else
				{
					Wcm::Log->Error("Source file of resource names is cannot created.").Sub("ResourceNamesFile", resourceNamesFile.string());
				}
			}

			std::ofstream ofs{resourceHandleFile.data()};
			if (!ofs.is_open())
			{
				Wcm::Log->Error("Source file of resource handle is cannot created.").Sub("ResourceHandleFile", resourceHandleFile);
				return EXIT_FAILURE;
			}

            const auto outAllRes = [resourceNames](auto outResLambda)
			{
				for (auto i = 0uz, j = 0uz ; i < resourceNames.length(); j = i + 1)
				{
					i = resourceNames.find_first_of(';', j);
					outResLambda(resourceNames.substr(j, i));
				}
			};
			const auto outResIncl = [&](std::string_view name)
            {
				if (name.contains('\\'))
				{
					auto data = const_cast<std::string_view::pointer>(name.data());
					std::replace(Wcm::Begin(data), Wcm::End(data), '\\', '/');
					name = data;
				}
				ofs << "#include \"Resources/" << name << ".hpp" << "\"\n";
            };
			/*
			const auto outResDecl = [&](std::string_view name)
            {
				ofs << "\tinline constexpr unsigned char _resource_" << ModifyFileName(name) << "_data[];\n";
            };
			*/
			const auto outResElm = [&](std::string_view name)
            {
				ofs << "\t\t\t{\"" << name << "\", _resource_" << ModifyFileName(name.cbegin(), name.cend()) << "_data},\n";
            };

			ofs << "// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.\n"
				   "// Copyright (c) 2024 Johnny Borov <JohnnyBorov@gmail.com>. Released under MIT License.\n"
				   "\n"
				   "#pragma once\n"
				   "#include <string_view>\n"
				   "#include <unordered_map>\n"
				   "#include <optional>\n";

			outAllRes(outResIncl);

			ofs << "\n"
				   "namespace Pazu\n"
				   "{\n";

			// outAllRes(outResDecl);

			ofs << // "\n"
				   "\t[[nodiscard]] constexpr std::optional<std::basic_string_view<unsigned char>> GetResource(const Wcm::ByteCharacterStringLike auto &name) noexcept;\n"
				   "\n"
				   "\tnamespace\n"
				   "\t{\n"
				   "\t\tinline const std::unordered_map<std::string_view, std::basic_string_view<unsigned char>> Resources\n"
				   "\t\t{\n";

			outAllRes(outResElm);

			ofs << "\t\t};\n"
				   "\t}\n"
				   "\n"
				   "\tconstexpr std::optional<std::basic_string_view<unsigned char>> GetResource(const Wcm::ByteCharacterStringLike auto &name) noexcept\n"
				   "\t{\n"
				   "\t\tif (const auto pos = Resources.find(Wcm::ToStringView(name).data()); pos != Resources.cend())\n"
				   "\t\t{\n"
				   "\t\t\treturn std::make_optional(pos->second);\n"
				   "\t\t}\n"
				   "\t\treturn std::nullopt;\n"
				   "\t}\n"
				   "}\n";

			if (ofs.bad())
			{
				Wcm::Log->Error("Error occurred while writing to source file of resource handle.").Sub("ResourceHandleFile", resourceHandleFile);
				return EXIT_FAILURE;
			}
			ofs.close(); // Temporarily... delete this.

			Wcm::Log->Info("Source file of resource handle is successfully created.").Sub("ResourceHandleFile", resourceHandleFile);
			return EXIT_SUCCESS;
		}
	}

	void ResourceGenerator::ReplaceMatchCandicate(size_t &i, const char mc, char *const &result) const noexcept
	{
		switch (mc)
		{
		case '.':
			result[i] = '_';
			result[++i] = '_';
			result[++i] = 'd';
			result[++i] = 'o';
			result[++i] = 't';
			result[++i] = '_';
			result[++i] = '_';
			return;
		case '-':
			result[i] = '_';
			result[++i] = '_';
			result[++i] = 'd';
			result[++i] = 'a';
			result[++i] = 's';
			result[++i] = 'h';
			result[++i] = '_';
			result[++i] = '_';
			return;
		case ' ':
			result[i] = '_';
			result[++i] = '_';
			result[++i] = 's';
			result[++i] = 'p';
			result[++i] = 'a';
			result[++i] = 'c';
			result[++i] = 'e';
			result[++i] = '_';
			result[++i] = '_';
			return;
		case '/':
			result[i] = '_';
			result[++i] = '_';
			result[++i] = 's';
			result[++i] = 'l';
			result[++i] = 'a';
			result[++i] = 's';
			result[++i] = 'h';
			result[++i] = '_';
			result[++i] = '_';
			return;
		default:
			result[i] = mc;
			return;
		}
	}
}
