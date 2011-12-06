#!/bin/sh

hcitool cc 00:18:E4:0C:67:DD
hciattach -b 00:18:E4:0C:67:DD
rfcomm connect rfcomm0 00:18:E4:0C:67:DD 4
rfcomm bind rfcomm0 00:18:E4:0C:67:DD 4
rfcomm release rfcomm0 00:18:E4:0C:67:DD 4
rfcomm -a 
