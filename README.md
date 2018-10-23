# STSMOK001_4022S
4022S code

This is the code I used for my Visual Object detection with stereo vision project

To run the matlab code, you need mexopencv. Instructions on how to install it are here: https://github.com/kyamagu/mexopencv/wiki/Installation-%28Windows%2C-MATLAB%2C-OpenCV-3%29

Instructions on how to install openCV on the raspberry pi are here: http://pklab.net/?id=392&lang=EN


Mexopencv documentation is found here: http://kyamagu.github.io/mexopencv/matlab/

Excuse the bad naming, code descriptions:

Monocular.m: Object detection on given image;

Stereo_disp: live object detection, depth estimation and size estimation (requires two cameras)

webcam.m: live object detection using one camera

dbscan.m: used by all other Matlab functions (written by Peter Kovesi https://www.peterkovesi.com/matlabfns/Misc/dbscan.m )

detector.cpp: final code used for object detection on raspberry pi

dbscan.hpp: used by detector.cpp for clustering

readVid.cpp: used to record video and save to file
