// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.
// Copyright (c) 2024 Johnny Borov <JohnnyBorov@gmail.com>. Released under MIT License.

#include "ResourceGenerator.hpp"

int main(int argc, char *argv[])
{
    Wcm::Log->OutputFile = Wcm::GetBaseDirectory() / "ResourceGenerator.log";
    return Pazu::GenerateResourceFiles(argc, argv);
}
