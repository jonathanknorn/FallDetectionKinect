#g++-4.7 hello.cpp -o  hello -I /usr/local/include/opencv -L/opt/vc/lib -lmmal -lmmal_core -lmmal_util -lopencv_core -lopencv_highgui

g++-4.7 BackgroundSubtraction.cpp -o  BackgroundSubtraction -L/usr/local/lib -lopencv_nonfree -lopencv_objdetect -lopencv_features2d -lopencv_imgproc -lopencv_highgui -lopencv_core   -lopencv_video

#g++-4.7 hello2.cpp -o  hello2 -L/usr/local/lib -lopencv_nonfree -lopencv_objdetect -lopencv_features2d -lopencv_imgproc -lopencv_highgui -lopencv_core   -lopencv_video
