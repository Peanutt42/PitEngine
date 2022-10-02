@echo off
echo "Compiling shader files[.vert, .frag] into .spv files"

C:\VulkanSDK\1.3.224.1\Bin\glslc C:\dev\cpp\PitEngine\Sandbox\shaders\simple_shader.vert -o C:\dev\cpp\PitEngine\Sandbox\shaders\simple_shader.vert.spv
C:\VulkanSDK\1.3.224.1\Bin\glslc C:\dev\cpp\PitEngine\Sandbox\shaders\simple_shader.frag -o C:\dev\cpp\PitEngine\Sandbox\shaders\simple_shader.frag.spv

pause