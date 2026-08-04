# Pinout Reference — Smart Fan Controller V2

Board: **ESP32-C3 Super Mini**  
Firmware file: `V2_ground`

## Active Pin Assignments

| Signal | GPIO | Direction | Purpose |
|---|---:|---|---|
| `MIC_PIN` | 2 | Input (ADC) | Microphone analog level sampling |
| `TEMP_PIN` | 3 | 1-Wire I/O | DS18B20 data line |
| `PWM_PIN` | 4 | Output / Input (tri-state when OFF) | PWM control output for fan control line |
| `FAN_GND_CUT_PIN` | 21 | Output | Gate drive for IRLZ44N low-side ground cut |

## Support Components

- **DS18B20 pull-up:** 4.7k between GPIO3 and 3.3V
- **MOSFET gate series resistor:** 100–220Ω between GPIO21 and IRLZ44N gate
- **MOSFET gate pulldown:** 10k between gate and source (GND)
- **Fan control line pulldown:** 10k from fan PWM/control node to fan-side GND
- **Flyback diode across fan:** cathode to fan +, anode to fan −

## Shared Grounding

ESP32 GND and fan supply GND **must** be common.

## Notes

- GPIO21 selected because it is available on your ESP32-C3 Super Mini.
- GPIO20 is an acceptable alternative for `FAN_GND_CUT_PIN` if rewiring is needed.
