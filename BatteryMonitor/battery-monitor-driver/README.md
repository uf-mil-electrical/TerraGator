## Battery Monitor Board Driver

The following code contains the means for operating TerraGator's Battery Monitor Board. The current project structure is listed below:

battery_monitor/
│
├── CMakeLists.txt
├── pico_sdk_import.cmake
│
├── src/
│   ├── main.c
│   ├── board_config.c / board_config.h
│   ├── drivers/
│   │   ├── current_sens.c / current_sens.h
│   │   ├── led.c / led.h
│   │   ├── rpi5_comm.c / rpi5_comm.h
│   │   ├── sseg.c / sseg.h
│   │   └── voltage_sens.c / voltage_sens.h
│   └── 
└── 


