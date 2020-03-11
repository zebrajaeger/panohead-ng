# panohead-ng

Motorized panoramic head based on

## Current prototype setup

- [ESP32 WROOM](https://www.espressif.com/en/products/hardware/esp-wroom-32/overview) (dual core) as controller.
- [TMC429](https://www.trinamic.com/fileadmin/assets/Products/ICs_Documents/TMC429_datasheet.pdf) (ramp generator) for smooth and software independent realtime movement of the stepper motors.
  - ESP works as clock source.
- [TMC2208](https://shop.watterott.com/SilentStepStick-TMC2208_1) (stepper driver) modules.
  - Low noises.
  - Low power cause of coolstep.
  - Available as 3D printer compatible module.
- [Stepper 11HS12-0674D1-PG27](https://www.omc-stepperonline.com/download/11HS12-0674D1-PG27.pdf) (NEMA 11, ~27:1 gearbox, 0.67A, 5.6ohms).
- Additional GT2 belt gear 1:5 to increase torque and reduce backlash.
- Optocouplers for Focus/Trigger.
- ~~[ADS1115](http://www.ti.com/lit/ds/symlink/ads1115.pdf) (I²C 4-channel ADC) for joystick, battery voltage and current.~~
  - [INA219](http://www.ti.com/lit/ds/symlink/ina219.pdf) (I²C current and voltage sensor, 12Bit resolution) for battery voltage and current.
  - Joystick read via [ESP32 ADC](https://randomnerdtutorials.com/esp32-adc-analog-read-arduino-ide/).
- OLED Display (I²C) for menu and status.
  - 128x64 Pixel
- Rotary encoder with knob for menu.
- Analog console joystick for jogging.
- Some status LED's for focus, trigger, ...
- [MPU9255](https://stanford.edu/class/ee267/misc/MPU-9255-Datasheet.pdf) (I²C; 3-Axis gyroscope, 3-Axis accelerometer,3-Axis magnetometer) for motion sensing.
- [PlatformIO](https://platformio.org/) / Arduino for software.
- Maybe [Angular](https://angular.io/) (10+ with ivy) and websockets on JSON base for remote interfacing.

## Power supply

- Battery from [4 - 12.6]V (4-8 NiMH or 2-3 LiPo in series).
  - Single NiMH cell [1 - 1.5]V.
  - Single LiPo cell [2.5 - 4.2]V.
- 2 x Step-Up (to ~12V) module based on [MT3608](https://prom-electric.ru/media/MT3608.pdf) to power stepper driver.
- Step Down (to ~3.3V) module based on [MP2315](https://www.monolithicpower.com/en/documentview/productdocument/index/version/2/document_type/Datasheet/lang/en/sku/MP2315/document_id/513/) to power logic.

## TODO

- Consider backlash on movement.
- Integrate TMC2208 UART connection into motor driver class
  - Set Enabled/Disabled
  - Set µSteps to 256
- Menu shows always.
  - Battery level. (Font: battery19)
  - Mode.
- ~~Main Menu to switch between IDLE, LEVELING, JOGGING and PANO mode.~~
- ~~SubMenu for LEVELING.~~
- ~~Sub Menu for JOGGING.~~
  - ~~Show pano bounds.~~
  - ~~Show current pos.~~
  - ~~Show leveling.~~ Display to small...
- ~~JOGGING in velocity Mode!?. Maybe that works better than in position mode.~~
  - Also Encoder for jogging!? For more accurate movement.
- Sub Menu for PANO Settings.
  - Number of shots per position.
  - ~~Focus time per shot.~~
  - ~~Trigger  time per shot.~~
  - ~~Delay after movement.~~
  - Delay between shots.
- Sub Menu for PANO.
  - Pause and Stop for PANO mode.
  - Show current column,row,shot; focus,trigger; current image of whole images.
- SubMenu to show battery Voltage.
  - Edit Battery level warning.
- Shaking sensor.
- ~~Leveling sensor.~~
- SubMenu to calibrate Motor speed and accleration values.
- ~~Scopes for Logger.~~
  - ~~Static map to see/reach all loggers for changing level.~~
  - ~~Check current level to avoid useless calculations. isDebug(), isInfo(), ...~
  - Using Lambdas? LOG.onDebug([]{ LOG.d("foo: %f", myComplicatedCalculation(); )})
- WiFi
  - WS Server.
  - Angular Frontend.

## Technical stuff

### Camera

- Measured on Canon EOS 7d MARK II
  - Trigger and focus voltages are 3.3V.
  - Current to switch is ca. 60µA.
- Nearly every optocoupler with transistor or fet output should be ok.

### Battery

- I plan to use [INR18650-29E](https://irp-cdn.multiscreensite.com/80106371/files/uploaded/file9.pdf) because of its weight and price.
- Two 3S packs. I calculate with 9Wh/Cell. A pack has 27Wh.
  - I destroyed one cell so there is a 2S and a 3S pack...
- Power consumption
  - ca 1W with disabled steppers
  - ca. 5W with stopped steppers (and TMC2208 coolstep2)
  - ca. 17W peak with both stepepers moving.
