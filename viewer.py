# python script to display ppm files since it isn't compatible with Mac's preview
import matplotlib.pyplot as plt
import matplotlib.image as mpimg

# Read the PPM file
img = mpimg.imread("image.ppm")


plt.imshow(img)
plt.axis('off')  
plt.show()