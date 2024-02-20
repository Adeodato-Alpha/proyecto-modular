import serial, time , collections
import matplotlib.pyplot as plt 
import matplotlib.animation as anime 
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from tkinter import Tk, Frame, StringVar, Label, Button,Entry
#Empezar la figura/Plot
def animate(i, dataList, ser):                                    
    arduinoData_string = ser.readline().decode('ascii').strip().split(',')
    a1= [int(s) for s in arduinoData_string]                 

    try:
        arduinoData_float = float(a1[0])   
        dataList.append(arduinoData_float)              

    except:                                                                          
        pass

    dataList = dataList[-50:]                           
    
    ax.clear()                                          
    ax.plot(dataList)                                  
    
    ax.set_ylim([0, 35000])                              
    ax.set_title("RP2040")                        
    ax.set_ylabel("Lectura")                              

dataList = []                                           
                                                        
fig = plt.figure()                                      
ax = fig.add_subplot(111)                               

ser = serial.Serial('COM3',115200)                      
time.sleep(2)                                           
ani = anime.FuncAnimation(fig, animate, frames=100, fargs=(dataList, ser), interval=100) 

                                            



#Crear la GUI
root = Tk()
root.title("Modular Graphics ")


grafic = FigureCanvasTkAgg(fig, master = root)
grafic.get_tk_widget().pack(side='bottom', fill='both', expand=1)
root.mainloop()
ser.close()