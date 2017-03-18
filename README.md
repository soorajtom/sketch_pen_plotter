# sketch_pen_plotter
The aim of this project is to make a plotter that can plot bitmap images with a Sketch pen using the frame from a discarded inkjet printer. The plotter is controlled by an Arduino developement board. It can print images sent through a Serial Interface or from a microSD card. The sketch Serial_SD is used to drive the printer from the Arduino board. The serial_print.cpp file is used to interface the arduino with the computer. The bitmap image is converted into a binary matrix using a freeware, and the image is sent line by line to the microcontroller. The microcontroller recieves the line and stores it temporarily in the microSD card. It will start the printing as soon as the whole picture is recieved.

video : https://youtu.be/REvqXE7EdGY
