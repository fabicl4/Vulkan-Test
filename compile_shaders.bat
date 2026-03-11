@echo off
echo Compiling shaders...

set GLSLC=%VULKAN_SDK%\Bin\glslc.exe

%GLSLC% shaders\shader.vert -o shaders\vert.spv
%GLSLC% shaders\shader.frag -o shaders\frag.spv

echo Done.
pause