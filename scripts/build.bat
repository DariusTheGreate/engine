vcvars64
# DONT WORK YET
cl *.cpp /std:c++20  /I ../src/ /I ../vendor/imgui/ /I ../vendor/assimp/include/ /I ../vendor/glfw64/include/ /I ../vendor/stb/ /I ../vendor/glm/ /I ../vendor/glad/include/ /I ../vendor/ImGuizmo/ /link /LIBPATH:../vendor/assimp/build/contrib/zlib/Debug/ /LIBPATH:../vendor/assimp/build/lib/Debug/ /LIBPATH:../vendor/glfw64/lib-vc2022/ glfw3.lib assimp-vc143-mtd.lib zlibstaticd.lib  opengl32.lib ole32.lib oleaut32.lib uuid.lib  comdlg32.lib advapi32.lib

msbuild Build/engine/engine.sln

