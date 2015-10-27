# AVC
Autonomous Vehicle Contest (from sparkfun) Rover code.

This is a nifty piece of C code to control an RC car as it goes around a course.
## Its main features are:
* kahlman filtering of sensor input (mathemetically simplified to fit need and computational budget)
* telemetry to graphical interface 

## Hardware:
I have a Traxxis "slash" RC car that has been modified to run automatically around a pre defined track.
There are several components added to the vehicle:
* An Arduino Mega is the basic brains behind the system.
* GPS (from adafruit) 
* Bluetooth (bluesmirf) for telemetry. see telemetry browser android code (https://github.com/holstine/TelemetryBrowser)
* 9dof orientation (accel, gyro, compass)
* magnetic odometer (built into transmission)
* control mux to switch between automous and remote drive
* 2 line LCD for status
* knob/button/encoder for input/mode change ( go button)
* voltage meter (under-volt is a major problem!)
* ultrasonic range finder
* extra batery (ground plane issues between gps and motor controller)
* sd card reader to record run.
* transmitter modified with Autonomous/Manual switch



##TODO for next year:

###Arduino:
- UltraSonic
- landfall
- avoidance
- get loose if stuck (no movement)
- Lots of waypoints automatically distribute
- Speed vs power control
- Break / backup
- Slow down for waypoint turning
- upgrade bluetooth

- upgrade IMU
- Re-do 9dof

###Android:
- Long range connection (transmitter to bluetooth device?)
- Easy waypoint changeover
- heartbeat for connection / auto reconnect
###Car:
*Check Mechanics for bent struts, broken parts
- New batteries
- reinstall LCD
- New Buttons
- Re-Attach Components
- Larson scanner
- Inflatable top?
###Calibration
- Steering bias solved
- Compass tilt bias fixed
- Speed run
- Calibration via GPS then remove GPS
- Dynamic Calibration of IMU

New This year
- Optical Mouse chip motion sensor (needs lenses)
- Attach Lidar
- Lidar based landfall (SLAM)
 

*Steering control via Computer or transmitter
*Extra switch on transmitter
*Placement of components
*Validate each Component
*Odometer
*verify odometer doesnt skip
*find maxspeed before skip
*use pin interrupt for odo count
- Figure traxxis odometer
- Differential odometry
*heading
*tablet calibrated compass
*averaging compass readings works across 360
*find true north
*integrate GPS course and speed
- integrate steering into heading
*auto calibrated compass
*tilt compensation
- Ultra sonic
- mount Ultrasonic
- verify ultrasonic
- Pivot ultra sonic
- Avoidance algorithm
- disable ultrasonic via tablet
*GPS
*Speed up GPS
*reset origin
*Only use good gps points
*use Antenna
- remove antenna magnet
*install Ground Plane
- Multiple GPS s
*Remove blatantly awful points

- Gyro/compass/accelerometer unit
*make i2c bus
*install multi compass, gyro
*use IMU for sensor values
- use software i2c for second compass
*calibrate compass
*Multi compass, gyro compass, steering algorithm
- Accelerometer turn off if upside down
*Buttons and knobs
*placement of knob
- light it up
*button working
*encoder no skip
*LCD
*Place LCD
- Steering
*Speed up steering reaction time
- Pde steering
*tablet calibrated steering
*Steering severity
- Motor
*controlled by computer*controlled by manual via computer*tablet controlled speed
- speed controlled at waypoint approach
- Stop and backup

## Finished features
*Telemetry
*Input from tablet
*Waypoint input via tablet
*Graph position
*Graph GPS*Graph Compass Calibration
*Graph waypoints*Graph heading
*speed up bluetooth
*Show all variables
*Sliders for variable input
- make android program usable on phone
*Waypoint changeover
*verify pointing at waypoint
*Calculate if waypoint is reachable
*modified covariance used to allow fudge factor
*change waypoints on the fly (one off bug)
*Stop on finish line
*Save logs
*fileIO
*Save serial or bluetooth
- save file based on date and time
*telemetry based logging on / off
*choose file to replay
*replay telemetry to tablet
- biases and standard deviations
*initial biases
*Odo initial estimate
*compass offset
*steering bias
*remove left turn wonkyness
*Straight line and square track bias
*save settings
- startup calibration- fail if gyro not working
*compass calibration function
*determine appropriate variances
*odo variance
*gps variance ( course variance 5-8)
*compass variance
*gyro variance ( .1 or so)
- Save variances between runs
- reject bad sensors
- Bias algorithm
- Checklist for running
*run button
*competition mode
*Secure cables- Style
*Decals
- Fpv Camera
- Larson scanner

