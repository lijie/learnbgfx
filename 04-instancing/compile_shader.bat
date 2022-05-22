..\tools\shaderc.exe -f vs_instancing.sc -o ..\runtime\shaders\dx11\vs_instancing.bin --type vertex -i ../common/shaders --platform windows -p vs_5_0 -O 3
..\tools\shaderc.exe -f fs_instancing.sc -o ..\runtime\shaders\dx11\fs_instancing.bin --type fragment -i ../common/shaders --platform windows -p ps_5_0 -O 3
