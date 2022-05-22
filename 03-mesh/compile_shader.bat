..\tools\shaderc.exe -f vs_mesh.sc -o ..\runtime\shaders\dx11\vs_mesh.bin --type vertex -i ../common/shaders --platform windows -p vs_5_0 -O 3
..\tools\shaderc.exe -f fs_mesh.sc -o ..\runtime\shaders\dx11\fs_mesh.bin --type fragment -i ../common/shaders --platform windows -p ps_5_0 -O 3
