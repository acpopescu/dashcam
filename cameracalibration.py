# square size: 9-9.5mm
# IMX415 8MP Sensor for front Cam
# Electronic rolling shutter, 1.45 UM pixel size, 8.4 MP, 2176 vertical res, 3864 horiz rezolution
# Rolling Shutter sensor has a native resolution of 8.4 [MP] and an optical format of 1/2.8 inch at a pixel size of 1.45 x 1.45 µm.
# variable integration time
# Exmor R / STARVIS BSI, DOL-HDR, rolling shutter. CSI-2 Data output (90 FPS at 4k! on 4 lanes)
# Scan direaction normal as viewed from the optical direction, not FPGA pin direction. These CAN BE INVERTED
# ^
# |
#------>
# alternates red lines with blue lines, uses a color filter:
#  Gb B
#  R  Gr
# shutter speed is configurable (based on exposure, ambient light :()
# ITime = 1frame period - SHR0x(1H period) + TOffset. Toffset is 1.79 us 10 bit, 2.68 us 12 bit
# frame period = VMAX x 1H
# SHR0 can be set from 8 to #lines/frame -4 (initial setto 0x66) e-shutter timing
# VMAX (max vertical lines)  set initially to 0x8ca (2250)
# for 4k30hz 1H in seconds is ~ 1022 * 1/(74.25 MHZ) or 1016 * (1/72 MHZ)




# F1.6 
# Novatek 966670
# millimeters
#mtx = np.array([[2.07794887e+03,0.00000000e+00,1.84670431e+03], [0.00000000e+00,2.08092007e+03,9.68652791e+02], [0.00000000e+00,0.00000000e+00,1.00000000e+00]], np.float32)
#dist = np.array([[-3.96446557e-01, 1.93910372e-01,-2.63593840e-04, 3.73261657e-05,-4.91082365e-02]], np.float32)

import numpy as np
import cv2 as cv
import glob
import yaml

def calibrate_cam():
    # termination criteria
    criteria = (cv.TERM_CRITERIA_EPS + cv.TERM_CRITERIA_MAX_ITER, 30, 0.001)
    # prepare object points, like (0,0,0), (1,0,0), (2,0,0) ....,(6,5,0)
    objp = np.zeros((6*7,3), np.float32)
    objp[:,:2] = np.mgrid[0:7,0:6].T.reshape(-1,2)
    # 19.5 mm
    objp = objp * 19.5

    # Arrays to store object points and image points from all the images.
    objpoints = [] # 3d point in real world space
    imgpoints = [] # 2d points in image plane.
    images = glob.glob('calib/*.png')

    for fname in images:
        
        img = cv.imread(fname)
        gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
        # Find the chess board corners
        ret, corners = cv.findChessboardCorners(gray, (7,6), None)
        # If found, add object points, image points (after refining them)
        if ret == True:
            corners2 = cv.cornerSubPix(gray,corners, (11,11), (-1,-1), criteria)
            if corners2 is not None:
                print(fname+'---> OK')
                objpoints.append(objp)
                imgpoints.append(corners2)
                # Draw and display the corners
                cv.drawChessboardCorners(img, (7,6), corners2, ret)
                cv.imshow('img', img)
                cv.waitKey(500)
            else:
                print(fname+'---> BAD')
        else:
            print(fname+'---> BAD')

    cv.destroyAllWindows()

    ret, mtx, dist, rvecs, tvecs = cv.calibrateCamera(objpoints, imgpoints, gray.shape[::-1], None, None)

    print(ret)
    print(mtx)
    print(dist)
    print(rvecs)
    print(tvecs)

    cameradata_yaml={
        'matrix': mtx,
        'distortion' : dist
    }
    yaml.dump(cameradata_yaml, "camdata.yml")
    
    

#calibrate_cam()

#with open('camdata.yml', 'r') as file:
#    cameradata_yaml = yaml.safe_load(file)
# IMX415 8MP Sensor for front Cam
# F1.6 
# Novatek 966670
mtx = np.array([[2.07794887e+03,0.00000000e+00,1.84670431e+03], [0.00000000e+00,2.08092007e+03,9.68652791e+02], [0.00000000e+00,0.00000000e+00,1.00000000e+00]], np.float32)
dist = np.array([[-3.96446557e-01, 1.93910372e-01,-2.63593840e-04, 3.73261657e-05,-4.91082365e-02]], np.float32)

cv.destroyAllWindows()
images = glob.glob('test/*.png')

for fname in images:
    print(fname)
    img = cv.imread(fname)
    h,  w = img.shape[:2]
    newcameramtx, roi = cv.getOptimalNewCameraMatrix(mtx, dist, (w,h), 1, (w,h))

    dst = cv.undistort(img, mtx, dist, None, newcameramtx)
    # crop the image
    x, y, w, h = roi
    dst = dst[y:y+h, x:x+w]
    cv.imshow('img', img)
    cv.imshow('dst', dst)
    cv.waitKey()

cv.destroyAllWindows()
    