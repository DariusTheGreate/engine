import os
import subprocess 

os.system('cmd /k msbuild E:/own/programming/engine/Build/engine/engine.sln')

subprocess.Popen('E:/own/programming/engine/Build/engine/x64/Debug/engine.exe')

