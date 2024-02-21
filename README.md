# Pazu Resource
Pazu resource project is native CMake independent version of the [ResourceManager](https://github.com/Johnnyborov/ResourceManager) project.\
You can check out [Pazu Engine 2D](https://github.com/AlpCanNalbant/PazuEngine2D.git), with Pazu Resource integrated out of the box.

#
1) After linked the PazuResource library to your own project, you can run the executable file with your content files as arguments.
```
// "--OutDir" "...an/absoulete/output/path/for/disk/resources..." // Optional when you have a resource generator configuration file or default value is sufficient.
// "--BaseDirName" "yourAssetsFolderName" // Optional when its name is already "Assets" (default value) or you have a resource generator configuration file.
// The remaining arguments listed below are optional when you have a pre-created resource list file to read from (resource list file only mode).
// "--ResList" // Optional when you do not have any resource to list (directory list only).
// "-SrcPath" // When this is written, only the source paths should be sent to the arguments for disk and binary resources. In such a case,
//    for binary resources, the conversion of source paths to destination paths is done by cutting down to the resource base directory name.
//    The same routine is applied for disk resources and after the path is trimmed, the resulting path is added to front of the output path.
// "-DstSrcPath" // Optional if "-SrcPath" was never written (enabled by default). Otherwise, it acts as a toggle switch. In such a case,
//    paths for both disk and binary resources should be specified in the order of first destination and then source path.
// "-Bin" "binResFile1Dest" "binResFile1" "binResFile2Dest" "binResFile2" ...  -OR-  "-Bin" "binResFile1" "binResFile2" ... // Optional when
//    it is written first in the arguments or you do not have any binary resource.
// "-Disk" "diskResFile1Dest" "diskResFile1" "diskResFile2Dest" "diskResFile2" ...  -OR-  "-Disk" "diskResFile1" "diskResFile2" ... // Optional when
//    you do not have any disk resource.
// "--DirList" "resDir1" "resDir2" ... // Optional when you do not have any directory of resources to list (resource list only). It will be
//    search recursively within each directory. You should create "*.res" extensioned files with the same name for each resource you have in the directory.
ResourceGenerator.exe "--ResList" "Resource/Res.txt" "data/Res.txt" "Resource/Res2.txt" "data/Res2.txt"
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
