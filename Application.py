import serial as s 
import tkinter as tk 
import matplotlib.pyplot as plt  
from time import sleep as sle 
rp_port = s.Serial('COM3',115200) 
while True:
    valores = rp_port.readline().decode('ascii').strip().split(',')
    a1= [int(s) for s in valores]
    sle(0.1)