# panohead-ng

Motorized panoramic head based on

- [ESP32 WROOM (dual core) as controller.](https://www.espressif.com/en/products/hardware/esp-wroom-32/overview)
- [TMC429 (ramp generator) for smooth and software independent movement of the stepper motors.](https://www.trinamic.com/fileadmin/assets/Products/ICs_Documents/TMC429_datasheet.pdf)
- [TMC2208 (stepper driver) modules.](https://shop.watterott.com/SilentStepStick-TMC2208_1)
- [Stepper 11HS12-0674D1-PG27 (NEMA 11, ~27:1 gearbox, 0.67A, 5.6ohms)](https://www.omc-stepperonline.com/download/11HS12-0674D1-PG27.pdf)
- Additional GT2 belt gear 1:5 to increase torque and reduce backlash.
- Optocouplers for Focus/Trigger.
- [ADS1115 (I²C 4-channel ADC) for joystick, battery voltage and current.](http://www.ti.com/lit/ds/symlink/ads1115.pdf)
- OLED Display (I²C) for menu and status.
- Rotary encoder with knob for menu.
- Analog console joystick for jogging.
- Some status LED's for focus, trigger, ...
- [MPU9255 (I²C; 3-Axis gyroscope, 3-Axis accelerometer,3-Axis magnetometer) for motion sensing.](https://stanford.edu/class/ee267/misc/MPU-9255-Datasheet.pdf)
- [platformIO / Arduino for software.](https://platformio.org/)
- [maybe Angular (10+ with ivy) and websockets on JSON base for remote interfacing.](https://angular.io/)
