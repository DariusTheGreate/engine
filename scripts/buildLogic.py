import os
import subprocess 


os.rename('E:/own/programming/engine/logicScripts/EngineLogic/x64/Debug/EngineLogic.dll', 'E:/own/programming/engine/logicScripts/EngineLogic/x64/Debug/EngineLogicRen.dll')

os.system('cmd /k msbuild E:/own/programming/engine/logicScripts/EngineLogic/EngineLogic.sln')

os.remove('E:/own/programming/engine/logicScripts/EngineLogic/x64/Debug/EngineLogicRen.dll')

