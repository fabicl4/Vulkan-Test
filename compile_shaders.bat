@echo off
echo Compiling shaders...

set GLSLC=%VULKAN_SDK%\Bin\glslc.exe

%GLSLC% assets\shaders\22_shader_ubo.vert -o assets\shaders\vert.spv
%GLSLC% assets\shaders\22_shader_ubo.frag -o assets\shaders\frag.spv

echo Done.
pause