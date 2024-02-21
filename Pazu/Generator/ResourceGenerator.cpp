// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.
// Copyright (c) 2024 Johnny Borov <JohnnyBorov@gmail.com>. Released under MIT License.

#include <fstream>
#include <algorithm>
#include <filesystem>
#include "ResourceNames.hpp"
#include "ResourceGenerator.hpp"

namespace Pazu::Impl
{
	bool GenerateResourceFilesT::operator()(int argc, char *argv[]) const noexcept
	{
		if (argc == 0)
		{
			Wcm::Log->Info("No binary resources are specified.");
			return false;
		}
		int updateds = 0;
		std::string resourceNames;
		const auto parentDir = Wcm::GetSourceDirectory().parent_path();
		const auto resourcesDir = parentDir / "Resources";
		for (auto i = 0; i < argc; i += 2)
		{
			std::replace(Wcm::Begin(argv[i]), Wcm::End(argv[i]), '\\', '/');
			std::replace(Wcm::Begin(argv[i + 1]), Wcm::End(argv[i + 1]), '\\', '/');
			const auto resName = argv[i];
			const auto resFile = argv[i + 1];
			const auto resOutFile = (resourcesDir / resName).concat(".hpp").string();
			updateds += GenerateResourceFile(resName, resFile, resOutFile.c_str());
			resourceNames.append(resName).append(";");
		}
		resourceNames.pop_back(); // Remove the last semicolon delimiter.
		if (updateds == 0)
		{
			Wcm::Log->Info("All binary resources are up to date.");
			return false;
		}
		const auto resHandleFile = (parentDir / "Resource.hpp");
		return GenerateResourceHandle(resourceNames.c_str(), resHandleFile.string());
	}

	bool GenerateResourceFileT::operator()(char *const resourceName, const char *const resourceFile, const char *const outputFile) const noexcept
	{
		const auto cacheBackupFile = Wcm::GetSourceDirectory() / "Cache" / resourceName;
		if (!Wcm::MakeDirectories(cacheBackupFile.parent_path()))
		{
			Wcm::Log->Error("Base directory of binary resource cache is cannot created.", GetLastError()).Sub("Cache", cacheBackupFile.string());
		}
		else if (bool isError; !Wcm::UpdateFileContent(resourceFile, cacheBackupFile, isError))
		{
			if (!isError)
			{
				Wcm::Log->Info("Binary resource is up to date.").Sub("Resource", resourceName);
			}
			return false;
		}

		const auto modifiedName = ModifyFileName(resourceName);

		std::ifstream ifs{resourceFile, std::ios::binary};
		if (!ifs.is_open())
		{
			Wcm::Log->Error("Binary resource file is cannot opened.").Sub("ResourceFile", resourceFile);
			return false;
		}

		if (!Wcm::MakeDirectories(Wcm::ToBaseDirectory(outputFile)))
		{
			Wcm::Log->Error("Base directory of binary resource output file is cannot created.", GetLastError()).Sub("OutputFile", outputFile);
			return false;
		}
		std::ofstream ofs{outputFile};
		if (!ofs.is_open())
		{
			Wcm::Log->Error("Binary resource output file is cannot created.").Sub("OutputFile", outputFile);
			return false;
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

		if (ifs.close(); ifs.bad())
		{
			Wcm::Log->Error("Error occurred while reading from binary resource file.").Sub("ResourceFile", resourceFile);
			return false;
		}
		if (ofs.close(); ofs.bad())
		{
			Wcm::Log->Error("Error occurred while writing to binary resource output file.").Sub("OutputFile", outputFile);
			return false;
		}

		Wcm::Log->Info("Output of binary resource file is successfully created.").Sub("ResourceFile", resourceFile).Sub("OutputFile", outputFile);
		return true;
	}

	bool GenerateResourceHandleT::operator()(std::string_view resourceNames, std::string_view resourceHandleFile) const noexcept
	{
		if (resourceNames.size() == ResourceNames.size() && memcmp(resourceNames.data(), ResourceNames.data(), sizeof(char) * resourceNames.size()))
		{
			Wcm::Log->Info("Binary resources are up to date.");
			return false;
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
					   "#include <string_view>\n"
					   "\n"
				   	   "namespace Pazu\n"
				   	   "{\n"
					   "\tinline constexpr std::string_view ResourceNames =\n"
					   "\t\t\"";

				if (resourceNames.length() <= 150)
				{
					ofs << resourceNames; // std::quoted(resourceNames); for supporting backslashes.
				}
				else
				{
					for (auto i = 0uz; i < resourceNames.length(); ++i)
					{
						const auto c = resourceNames[i];
						// if (c != '\\')
						// {
							ofs << c;
						// }
						// else
						// {
							// ofs << R"(\)";
						// }
						if ((i % 150uz) == 0uz && c == ';' && (i + 1uz) < resourceNames.length())
						{
							ofs << "\n"
								   "\t\t";
						}
					}
				}
				ofs << "\";\n"
					   "}\n";
                if (ofs.close(); !ofs.bad())
                {
 					Wcm::Log->Info("Source file of binary resource names is successfully created.").Sub("ResourceNamesFile", resourceNamesFile.string());
                }
				else
				{
			        Wcm::Log->Error("Error occurred while writing to binary resource names file.").Sub("ResourceNamesFile", resourceNamesFile.string());
				}
			}
			else
			{
				Wcm::Log->Error("Source file of binary resource names is cannot created.").Sub("ResourceNamesFile", resourceNamesFile.string());
			}
		}

		std::ofstream ofs{resourceHandleFile.data()};
		if (!ofs.is_open())
		{
			Wcm::Log->Error("Source file of binary resource handle is cannot created.").Sub("ResourceHandleFile", resourceHandleFile);
			return false;
		}

        const auto outAllRes = [resourceNames](auto outResLambda)
		{
			for (auto i = 0uz, j = 0uz; i < resourceNames.length(); j = i + 1)
			{
				const auto pos = resourceNames.find_first_of(';', j);
				i = (pos != std::string_view::npos) ? pos : resourceNames.length();
				outResLambda({resourceNames.begin() + j, resourceNames.begin() + i});
				// As an alternative to above, this one line code is doesn't work for the second resource name.
				// outResLambda(resourceNames.substr(j, i));
			}
		};
		const auto outResIncl = [&](std::string_view name)
        {
#ifdef WCM_CPP23
			if (name.contains('\\'))
#else
			if (name.find('\\') != std::string_view::npos)
#endif
			{
				auto data = const_cast<std::string_view::pointer>(name.data());
				std::replace(Wcm::Begin(data), Wcm::End(data), '\\', '/');
				name = data;
			}
			ofs << "#include \"Resources/" << name << ".hpp" << "\"\n";
        };
		const auto outResElm = [&](std::string_view name)
        {
			ofs << "\t\t\t{" << std::quoted(name) << ", {std::cbegin(_resource_" << ModifyFileName(name.cbegin(), name.cend()) << "_data), "
				   "std::cend(_resource_" << ModifyFileName(name.cbegin(), name.cend()) << "_data) - 1}},\n";
        };

		ofs << "// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.\n"
			   "// Copyright (c) 2024 Johnny Borov <JohnnyBorov@gmail.com>. Released under MIT License.\n"
			   "\n"
			   "#pragma once\n"
			   "#include <string_view>\n"
			   "#include <unordered_map>\n"
			   "#include <optional>\n"
			   "#include \"WinAPICommon.hpp\"\n";

		outAllRes(outResIncl);

		ofs << "\n"
			   "namespace Pazu\n"
			   "{\n";

     	ofs << "\t[[nodiscard]] constexpr std::optional<std::basic_string_view<unsigned char>> GetResource(const Wcm::ByteCharacterStringLike auto &name) noexcept;\n"
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

		if (ofs.close(); ofs.bad())
		{
			Wcm::Log->Error("Error occurred while writing to source file of binary resource handle.").Sub("ResourceHandleFile", resourceHandleFile);
			return false;
		}

		Wcm::Log->Info("Source file of binary resource handle is successfully created.").Sub("ResourceHandleFile", resourceHandleFile);
		return true;
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
		// case '\\':
		// 	result[i] = '_';
		// 	result[++i] = '_';
		// 	result[++i] = 'b';
		// 	result[++i] = 's';
		// 	result[++i] = 'l';
		// 	result[++i] = 'a';
		// 	result[++i] = 's';
		// 	result[++i] = 'h';
		// 	result[++i] = '_';
		// 	result[++i] = '_';
		// 	return;
		default:
			result[i] = mc;
			return;
		}
	}
}
