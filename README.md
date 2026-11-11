# STM32F103 Bare-Metal Metronome

Bare-metal metronome for the **STM32F103C8T6** (72 MHz ARM Cortex-M3), built with a
**zero-HAL CMSIS driver design** — no STM32 HAL, no SPL, no RTOS, no dynamic allocation.


## Key Technical Highlights

- **Non-blocking SysTick timing architecture** — 1 ms monotonic time base; every peripheral poll runs through `system_wait_for_flag()` for deterministic hardware timeouts (no unbounded spinning).
- **OLED SSD1306 (128x32) via I2C1 + DMA** — `PB6`/`PB7`; DMA1 Channel 6 streams the frame buffer and the CPU is released immediately after transfer start.
- **Audio click via TIM3 PWM + LM386** — 1 kHz, 50% duty square wave on `PA6`, gated per beat by TIM2, feeding an LM386 power amplifier (200x gain) into an 8 Ω speaker.
- **RC-filtered hardware button inputs** — active-low push buttons with 0.1 µF debounce capacitors to GND.

## Repository Structure

```text
stm32-metronome/
├── Inc/                    # Driver / application headers (CMSIS, zero-HAL)
│   ├── main.h
│   ├── rcc_conf.h
│   ├── timeout_handler.h
│   ├── i2c_driver.h
│   ├── dma_driver.h
│   ├── ssd1306.h
│   ├── tim2_bpm_clock_control.h
│   ├── pwm_buzzer_driver.h
│   └── IWDG.h
├── Src/                    # Implementations
│   ├── main.c
│   ├── rcc_conf.c
│   ├── timeout_handler.c
│   ├── i2c_driver.c
│   ├── dma_driver.c
│   ├── ssd1306.c
│   ├── tim2_bpm_clock_control.c
│   ├── pwm_buzzer_driver.c
│   └── IWDG.c
├── Startup/                # core.S, vector_table.S
├── CMSIS/                  # Vendor CMSIS device / core headers
├── kicad circuitry/        # Schematic (Rev v1.0) + ERC report
├── Makefile                # Primary GNU Make build
├── CMakeLists.txt          # Optional CMake build
└── STM32F103C8T6.ld        # Linker script (64 KB flash / 20 KB RAM)
```

## Quick Start — Build & Flash

Prerequisites: `arm-none-eabi-gcc`, `make`, and `stlink-tools` or `openocd`.

```bash
# Build (GNU Make) -> build/main.elf, build/main.bin
make clean && make

# Build (CMake, optional)
cmake -S . -B build-cmake && cmake --build build-cmake -j"$(nproc)"

# Flash via ST-Link
st-flash --reset write build/main.bin 0x08000000



Toolchain flags: `-mcpu=cortex-m3 -mthumb --specs=nosys.specs -nostdlib`.

## Future Roadmap & Planned Software Improvements

- **Power Management & Energy Efficiency** — implement Cortex-M3 Sleep/Standby modes, peripheral clock gating, and auto-timeout display blanking to minimize battery drain.
- **Enhanced Button Configuration & Logic** — software-configurable Start/Stop button behavior, long-press handling for rapid BPM changes, and a non-blocking debouncing state machine.
- **Non-Blocking UI State Machine** — decouple OLED screen rendering completely from the audio timing generator.
- **Settings Persistence** — save current BPM, volume, and beat subdivisions to STM32 Backup Registers (BKP RAM) or external EEPROM.
