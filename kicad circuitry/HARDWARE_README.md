# STM32F103 Metronome — Hardware & Schematic Documentation

![Schematic Diagram](pics/photo_2026-11-08_20-55-19.jpg)

*Repository asset: `pics/photo_2026-11-08_20-55-19.jpg` (board photo; no exported schematic image is present). Schematic source: `kicad circuitry/stm32f1xx.kicad_sch`.*

### 1. General Overview & Power Architecture
- **Schematic Reference:** `kicad circuitry/stm32f1xx.kicad_sch` (Rev v1.0, Date: 2026-10-19, Author: Anton Popov).
- **Power Rails:** Dual-rail supply: +5 V from USB / Blue Pill header (`P2.26`); +3V3 from the Blue Pill on-board LDO (`P1.3`) for the MCU and OLED.
- **Decoupling Network:** `C4 = 100 µF` is the LM386 **BYPASS** capacitor (`U1.7` → GND), not a +5 V rail bulk capacitor. `C5`, `C6`, `C7 = 0.1 µF` are button RC filters, not supply bypass capacitors. **No dedicated 0.1 µF supply bypass is fitted**; add 100 nF at `U1.6` and `U2.2` in the next revision.
- **Source of truth:** the KiCad schematic and exported netlist take precedence over legacy pin notes.

### 2. Pinout & Peripheral Mapping
| STM32 Pin | Function | Peripheral / Device | Signal Description | Hardware Filtering / Pull-ups |
| :--- | :--- | :--- | :--- | :--- |
| **PA6** | TIM3_CH1 | LM386 Audio Amp (`U1` pin 3) | Audio PWM pulse input | Series `R4 = 10 kΩ` to IN+ (not 10 Ω) |
| **PB6** | I2C1_SCL | ER_OLED091_1x_I2C (`U2` pin 3) | I2C Clock | `R3 = 330 Ω` pull-up to +3V3 |
| **PB7** | I2C1_SDA | ER_OLED091_1x_I2C (`U2` pin 4) | I2C Data | `R2 = 330 Ω` pull-up to +3V3 |
| **PA10** | GPIO In | SW3 (SW_Start_Push_Button) | Start / Stop control | Active-Low, `0.1 µF` RC filter (`C5`) |
| **PA9** | GPIO In | SW1 (SW_Push_Dual) | BPM Down / Navigation | Active-Low, `0.1 µF` RC filter (`C7`) |
| **PA8** | GPIO In | SW2 (SW_Push_Dual) | BPM Up / Subdivisions | Active-Low, `0.1 µF` RC filter (`C6`) |

> **Pin-map correction:** legacy notes list `PB0`/`PB1` for SW1/SW2. On Rev v1.0 those pins are unconnected; SW1 = `PA9`, SW2 = `PA8`, SW3 = `PA10`. Firmware EXTI uses `PA8`/`PA9`.

### 3. Audio Subsystem (LM386 Amplifier)
- **Gain Stage:** 200× voltage gain configured via `C1 = 10 µF` across LM386 pins 1 & 8.
- **Output Coupling:** AC-coupled output via `C2 = 1000 µF` electrolytic capacitor driving an `8 Ω` loudspeaker (`LS1`); high-pass corner ≈ 20 Hz.
- **Zobel / Boucherot Filter:** Stability network (`R1 = 10 Ω`, `C3 = 0.1 µF`) from the `U1.5` output node to GND (before `C2`) prevents high-frequency oscillations. It is not directly across the speaker terminals.
- **Supply Decoupling:** `C4 = 100 µF` sits on the BYPASS pin (`U1.7`), and `R4 = 10 kΩ` is the input series resistor — neither isolates the audio power rail. For best stability add 100 nF + 100 µF at `U1.6` (+5 V).

### 4. User Interface & Display
- **OLED Display:** ER_OLEDM0.91_1x-I2C (SSD1306 controller, `128 × 32` resolution, I2C address `0x3C`, 3.3 V supply).
- **Control Buttons:** 3× tactile push buttons with dedicated hardware RC debouncing (`C5`/`C6`/`C7 = 0.1 µF`) to prevent contact chatter without blocking CPU cycles; internal pull-ups (~40 kΩ) give τ ≈ 4 ms.
- **Firmware note:** SW1/SW2 (BPM −/+) are implemented; SW3 (Start/Stop) is reserved and not configured in firmware Rev 1.0.0.
- **I2C note:** 330 Ω pull-ups are strong (~10 mA sink); consider 2.2–4.7 kΩ for a PCB revision.

### 5. Future Hardware Roadmap & Planned Improvements
- **Battery & Charging Management:** Integration of an onboard Li-Ion battery charger circuit (TP4056 / DW01A protection) with USB Type-C interface.
- **Amplifier Power Gating:** Addition of a P-Channel MOSFET (PMOS) high-side switch controlled by an MCU GPIO to completely cut off +5 V power to the LM386 amplifier when no audio is playing (eliminating quiescent current consumption).
- **Reverse Polarity & Overvoltage Protection:** Adding a Schottky diode or P-FET for battery protection.
- **Piezo / Speaker Output Selector:** Hardware jumper or switch to toggle between low-power piezo buzzer mode and high-power LM386 speaker mode.
