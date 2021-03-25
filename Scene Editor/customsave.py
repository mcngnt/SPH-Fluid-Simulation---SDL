# Translate 1080X720 png into sfs file

# Black : 0, 0, 0
# Red : 255, 0, 0
# Blue : 0, 153, 255

def val_ndarray(ndarr):
	arr = []
	for element in ndarr:
		arr.append(element)
	# print(arr)
	return arr

def is_equal(arr1, arr2):
	for i in range(len(arr2)):
		if arr1[i] != arr2[i]:
			return False
	return True

from PIL import Image
from numpy import asarray
# load the image
image = Image.open('target.png')
# convert image to numpy array
data = asarray(image)
print(type(data))
# summarize shape
print(data.shape)

# create Pillow image
image2 = Image.fromarray(data)
print(type(image2))

# summarize image details
print(image2.mode)
print(image2.size)

f = open("final.sfs", "w")

finalstr = ""

# print(type(data[0][100]))

for x in range(1080):
	for y in range(720):
		if is_equal(val_ndarray(data[y][x]), [255, 0, 0, 255]):
			finalstr += "{} {} 0 1 0 0\n".format(x, y)
		if is_equal(val_ndarray(data[y][x]), [0, 153, 255, 255]):
			finalstr += "{} {} 0 0 0 0\n".format(x, y)

finalstr = finalstr[:-1]

f.write(finalstr)
f.close()
