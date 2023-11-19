import cv2

image_path = "/home/owaisk4/Win_backup/FAST NU assignments/Parallel and Distributed Computing/Project/Image Processing/Code/Images"

# img = cv2.imread(image_path + "/" + "mayuri_suoh.ppm")
# hsv_img = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
# cv2.imwrite(image_path + "/" + "hsv_image.jpg", hsv_img)

img = cv2.imread(image_path + "/" + "hsv_image.jpg")
rgb_img = cv2.cvtColor(img, cv2.COLOR_HSV2BGR)
cv2.imwrite(image_path + "/" + "rgb_image.jpg", rgb_img)
