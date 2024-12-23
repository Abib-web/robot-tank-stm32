# robot-tank-stm32

This project is an end-of-session assignment designed to implement multiple functionalities for a tracked vehicle using a Nucleo microcontroller board. Below is a detailed breakdown of the project:

<img src="https://github.com/Abib-web/robot-tank-stm32/blob/main/images/WhatsApp%20Image%202024-12-12%20at%2010.45.16.jpeg?raw=true" alt="Tank Robot Front View" width="600">


---

## Step 1: Motor Control via PWM and Movement Management

The primary objective of this step is to control the motors of a tracked vehicle using PWM (Pulse Width Modulation) signals with a recommended frequency of 2 kHz. By adjusting the duty cycle of the PWM:

- A duty cycle of **1** represents maximum speed.
- A duty cycle of **0** corresponds to a stop.

Additionally, the vehicle's ability to turn left or right depends on the speed difference between the two motors. Note that identical duty cycles may not produce the same speed due to motor variability. The setup includes:

1. **DC Motor Control**: Using the compact L298N motor driver module. [L298N Datasheet](https://components101.com/modules/l293n-motor-driver-module)
2. **Movement Modes**: Forward, reverse, left turn, and right turn.
3. **Power Supply**: Powering the Nucleo board with ~7.5V batteries. Reference section 6.3.2 of the Nucleo datasheet for safe external power supply configurations.

---

## Step 2: Automated Operation Cycles

This step configures three automated operation modes that can be toggled via buttons and a dipswitch connected to the microcontroller. A blue button on the Nucleo serves as an interrupt to activate/deactivate these modes. If a mode is deactivated and reactivated, it should resume where it left off. Timers are used to manage motor speeds for each mode:

1. **Circle Mode**: The vehicle travels in a one-meter diameter circle and stops.
2. **Straight Line Mode**: The vehicle moves one meter forward, performs a U-turn, and returns to the starting point.
3. **Square Mode**: The vehicle navigates a 1x1 meter square, accelerating and decelerating along each segment, reaching maximum speed at the midpoint of each line. It stops after completing the square.
<a href="https://drive.google.com/file/d/1aLY71eFqSGDF4pN38_QJKEeVXK41lRw1/view?usp=sharing">
  <img src="https://github.com/Abib-web/robot-tank-stm32/blob/main/images/WhatsApp%20Image%202024-12-12%20at%2010.45.16.jpeg?raw=true" alt="Tank Robot Video Preview" width="500">
</a>

---

## Step 3: Manual Control via Wii Controller

This step involves implementing manual control for the vehicle using a Wii remote, building upon previous work in Lab #7. Key functionalities include:

1. **Joystick Control**: Controls forward, reverse, left, and right movements.
2. **Speed Adjustment**: The Z button cycles through three speed modes (PWM duty cycles of 0.33, 0.66, and 1) when the joystick is fully pushed.
3. **Manual Mode Toggle**: The C button activates/deactivates manual mode. Manual mode only works when the dipswitch is configured correctly.

---

## Step 4: Real-Time Display on LCD

The final step displays real-time data on an LCD screen, using principles from Lab #6. The displayed information includes:

1. **Current Time and Date**: Using a Real-Time Clock (RTC) module.
2. **Vehicle Mode**: Shows whether the vehicle is in automatic mode (A1, A2, A3) or manual mode (M1, M2, M3).
3. **Real-Time Motor Speeds**: Using Hall effect sensors (encoders) to measure and display motor speeds. Refer to [this guide](https://robu.in/interfacing-hall-effect-sensor-with-arduino-connection-code/) for details on interfacing Hall sensors. Power the sensors with the 5V supply from the Nucleo board.

---

## Conclusion

This project combines advanced motor control, automation, and real-time feedback mechanisms, showcasing a blend of practical electronics and embedded systems programming. The full implementation highlights the application of PWM, RTC modules, interrupts, and sensor integration.
