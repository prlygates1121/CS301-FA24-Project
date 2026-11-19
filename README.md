# CS301 Embedded Systems Final Project (Fall 2024)

A wireless smart car and touch-screen remote control system built on two **STM32F103RCTx** boards for **CS301 (Embedded Systems and Microcomputer Principle)**, Fall 2024. The **car (`car_board`)** and the **handheld controller (`control_board`)** communicate bidirectionally over 2.4 GHz **NRF24L01** transceivers.

---

## System Architecture

```text
+-------------------------------+      NRF24L01 (SPI1 + EXTI)      +--------------------------------+
|        control_board          |  <============================>  |           car_board            |
|  - TFT-LCD + Touch Screen     |     Commands / Telemetry &       |  - 4-Ch Motor PWM (TIM4)       |
|  - 3 Interactive UI Modes     |         Map State Updates        |  - ATK-MS6050 IMU (DMP Yaw)    |
|  - On-board Keys (Confirm/Go) |                                  |  - Servo + Ultrasonic Sensor   |
+-------------------------------+                                  |  - Ground & Obstacle IR Sensors|
                                                                   +--------------------------------+
```

---

## Key Features

### 1. Remote Controller (`control_board`)
Switch between three UI modes using the bottom navigation bar:
- **CONTROL**: Touch D-pad (forward, backward, left/right strafe, left/right spin, stop), `TRACE` / `HAND` toggles for IR line-following mode, and live ultrasonic distance telemetry in the top-left corner.
- **ROUTE**: Interactive 4×4 grid editor (`START`, `END`, `TRAP`, `ROUTE`) with on-board buttons (`KEY0` to confirm selection, `KEY1` to dispatch the drawn path, `KEY_WKUP` to clear).
- **AUTO**: Select `START` and `END` cells on the 4×4 grid and tap `ROUTE` to trigger autonomous exploration; obstacles, clear cells, and DFS/final path progress streamed from the car are rendered on the grid in real time.

### 2. Smart Car (`car_board`)
- **Motion Control**: 4-channel PWM motor control (`TIM4`) supporting forward/backward driving, lateral strafing, and spinning.
- **IMU & Grid Navigation**: Uses **ATK-MS6050** DMP yaw readings (`get_yaw()`) for target-angle rotation and the center ground IR sensor to detect grid boundaries for cell-by-cell movement.
- **Obstacle Detection & Pathfinding**: Sweeps a servo-mounted (`TIM5`) ultrasonic sensor (`TIM2` + EXTI) across left, front, and right directions, performs **DFS exploration** on a 4×4 grid while reporting map updates back to the controller, and traverses the discovered path to the goal.
- **Line Following**: Implements `SearchRun()` using left/center/right ground IR sensors to track black lines.

---

## Repository Structure

```text
.
├── car_board/                  # Car firmware (STM32CubeIDE / HAL)
│   ├── car_tracing.ioc         # STM32CubeMX configuration
│   ├── Core/                   # Application logic & peripheral setup (main.c, utils.c, 24l01.c, etc.)
│   └── Drivers/                # STM32F1xx HAL & ATK-MS6050 (eMPL DMP) drivers
└── control_board/              # Controller firmware (STM32CubeIDE / HAL)
    ├── control_board.ioc       # STM32CubeMX configuration
    ├── Core/                   # Application logic, LCD UI & touch handling (main.c, lcd_v4.c, utils.c, etc.)
    └── Drivers/                # STM32F1xx HAL & touch screen / 24Cxx drivers
```

---

## Build & Flash

1. Import `car_board/` and `control_board/` into **STM32CubeIDE**.
2. Build both projects and flash them onto the corresponding **STM32F103RCTx** boards.
3. Power on both boards; they perform an automatic NRF24L01 handshake on startup (`Testing data transmission: OK` on the controller LCD) before entering the main UI.
