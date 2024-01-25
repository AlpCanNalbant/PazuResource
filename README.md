# Pazu Resource
Pazu resource project is native CMake independent version of the [ResourceManager](https://github.com/Johnnyborov/ResourceManager) project.

#
1) After linked the PazuResource library to your own project, you can run the executable file with your content files as arguments.
```
// GeneratorExec "FirstResourceName" "FirstResourceFile(relative or absolute path)" "SecondResourceName" "SecondResourceFile" ...
ResourceGenerator.exe "Resource/Res.txt" "data/Res.txt" "Resource/Res2.txt" "data/res2.txt"
```
2) Then, after adding the Resource.hpp file to your project, you can access the data this way.
```
std::optional<std::basic_string_view<unsigned char>> res = Pazu::GetResource("Resource/Res.txt");
if (res)
{
   const unsigned char *data = res.data();
   size_t dataSizeInBytes = res.size();
}
auto res2 = Pazu::GetResource("Resource/Res2.txt");
// ... operations on the second resource ...
```
