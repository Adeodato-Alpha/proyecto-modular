import serial as s 
import tkinter as tk 
import matplotlib.pyplot as plt  
from time import sleep as sle 
rp_port = s.Serial('COM3',115200) 
while True:
    print(rp_port.readline())
    sle(0.1)