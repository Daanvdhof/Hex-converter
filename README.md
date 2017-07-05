# Hex-converter Version 2.0 5-jul-2017
Converts intel hex files to have consecutive adresses and equal record lengths. Software was originally written to use with embedded processor bootloaders.

Contains:
Debug folder with the executable
project folder with the cpp file
visual studio 2015 project


Use: 
Set the desired record data length in the cpp file define
Build the project
Drag a .hex file into the executable
Application should output the resulting hex file with the name [original_name]_trimmed.hex

Note:
Only works with record types 0 and 1, should suffice for 8 bit microcontrollers

