// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#include "ResourceListGenerator.hpp"
#include "ResourceGeneratorLauncher.hpp"

int main(int argc, char *argv[])
{
    return Pazu::GenerateResourceList(argc, argv) && Pazu::LaunchResourceGenerator() ? EXIT_SUCCESS : EXIT_FAILURE;
}
