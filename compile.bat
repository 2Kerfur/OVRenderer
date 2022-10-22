C:\VulkanSDK\1.3.224.1\Bin\glslc.exe shaders\simple_shader.vert -o out\build\x64-Debug\shaders\simple_shader.vert.spv
C:\VulkanSDK\1.3.224.1\Bin\glslc.exe shaders\simple_shader.frag -o out\build\x64-Debug\shaders\simple_shader.frag.spv
ROBOCOPY "D:\DEV\MY_GITHUB\OVRenderer\out\build\x64-Debug\shaders" "D:\DEV\MY_GITHUB\OVRenderer\out\build\x64-Release\shaders" /mir
ROBOCOPY "D:\DEV\MY_GITHUB\OVRenderer\out\build\x64-Debug\shaders" "D:\DEV\MY_GITHUB\OVRenderer\out\build\x64-Ship\shaders" /mir
pause