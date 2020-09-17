# Remote supervision of high voltage regulator
> :bulb: *The idea of this repository is not to be a complete guide for performing automation and supervision of voltage regulators.*

<p align="center">
  <img src="./img/project.png" alt="schematic of the project" width="700"/>
</p>


## General objective :zap:
In this repository there is the part of the development of a system, to perform the monitoring and change of operational parameters remotely in high voltage regulators.

The system reads data from voltage regulators, through serial communication with the TB-R800 Control, and makes the data available via LPWA network ([Sigfox](https://www.sigfox.com/) network).

To change parameters, communication between the client and the regulator is carried out via the GSM network :satellite::iphone:.

### Reading data
The reading and writing of data on the TB-R800 control is performed by an Atmega328p microcontroller through RS-232 communication.

<p align="center">
  <img src="./img/ATmega328P.png" alt="ATmega328P" width="200"/>
</p>

> <span></span>
> :warning:  <span style="color:#131518"><b> Warning </b></span>
> 
> This is a small part of an P&D project. The information contained here should not be construed as a complete guide. For more information, contact us by email :love_letter:.
> <br>


