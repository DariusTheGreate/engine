from PIL import Image
import os
from os import listdir

folder_dir = "E:/own/programming/engine/textures/Warrior-V1.3/pipino"
for imageName in os.listdir(folder_dir):
	if (imageName.endswith(".png")):
		print(folder_dir + '/' + imageName)
		img = Image.open(folder_dir + '/' + imageName)
		width, height = img.size

		img2 = img.resize((width * 3, height * 3), resample=Image.NEAREST)
		img2.save(folder_dir + '/' + imageName)
