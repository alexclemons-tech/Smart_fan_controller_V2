# Wiring Diagram — Smart Fan Controller V2

```text
                                +------------------------------+
                                |      Fan Power Supply        |
                                |          (+Vfan/GND)         |
                                +---------------+--------------+
                                                |
                                             +Vfan
                                                |
                                                |        Flyback diode
                                                |      +-|<|-+
                                                |      |     |
                                                v      |     |
                                         +-------------+     |
                                         |    FAN +          |
                                         |                   |
ESP32-C3 Super Mini                      |    FAN -----------+--------------------+
--------------------                     +----------------------------------------|
3V3 ---------------------+                                                      Drain
                         |                                                        |
GND ------------------+--+------------------------------------+-------------------+
                      |                                       |                IRLZ44N
GPIO2 (MIC_PIN) ------+--> Microphone analog out             Source               |
GPIO3 (TEMP_PIN) --------> DS18B20 DQ                         |                  GND
                      |                                       |
                      +--[4.7k]---> 3V3 (DS18B20 pull-up)    |

GPIO4 (PWM_PIN) --[100-220R]--> Fan PWM/Control line (if used)
                       |
                       +--[10k]--> Fan-side GND (anti-float pull-down)

GPIO21 (FAN_GND_CUT_PIN) --[100-220R]--> IRLZ44N Gate
                                         |
                                        [10k]
                                         |
                                        GND (gate pulldown)
```

## Connection Checklist

1. Common ground between ESP32 and fan power supply
2. IRLZ44N Source to GND, Drain to fan negative
3. Fan positive directly to +Vfan
4. Gate pulldown installed (10k)
5. Fan control-line pulldown installed (10k)
6. Flyback diode installed across fan terminals

## Firmware Behavior Related to This Diagram

When fan duty is 0:
- PWM is set to 0
- Ground-cut MOSFET is switched OFF
- PWM pin is set to INPUT (tri-state)

When fan duty > 0:
- PWM pin restored to OUTPUT
- Ground-cut MOSFET switched ON
- PWM duty applied
