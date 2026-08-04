# Smart Fan Controller V2 (ESP32-C3 Super Mini)

A Wi-Fi fan controller that balances **noise suppression** and **thermal safety**.

- Hosts a local web dashboard in AP mode (`FanControllerV2`)
- Auto/Manual fan control
- Mic-based fan mute logic
- Temperature-based fan ramping
- **Separate mic-override temperature** (`tempMicOverride`)
- Kickstart logic for reliable fan spin-up
- IRLZ44N low-side MOSFET ground cut for hard-off behavior

---

## Features

- **Auto Mode**
  - If room is noisy and temp is below mic override, fan mutes (off)
  - If temp reaches mic override threshold, microphone can no longer mute fan
  - Fan ramps from `tempStart` to `tempMax` (0→255 duty), with a minimum run floor
- **Manual Mode**
  - Direct speed control via slider
- **Harder OFF behavior**
  - PWM forced to 0
  - Fan ground path disconnected via MOSFET
  - PWM pin tri-stated (`INPUT`) to reduce ghost hum/backfeed

---

## Web UI Parameters

- **Mic Mute Threshold** (`sens`)  
  ADC threshold (0–4095) above which sound is considered “noisy”.
- **Fan Start Temperature** (`tStart`)  
  Temperature where fan begins ramping from 0.
- **Fan 100% Max Temp** (`tMax`)  
  Temperature where fan reaches full speed.
- **Mic Override Temp** (`tMicOv`)  
  Temperature at/above which mic can no longer mute fan.

---

## Pinout (ESP32-C3 Super Mini)

| Function | GPIO | Notes |
|---|---:|---|
| Microphone analog input | `GPIO2` | `MIC_PIN` |
| DS18B20 data | `GPIO3` | `TEMP_PIN` (use 4.7k pull-up to 3.3V) |
| Fan PWM output | `GPIO4` | `PWM_PIN` |
| Fan ground cut MOSFET gate | `GPIO21` | `FAN_GND_CUT_PIN` |
| (Optional alternate gate pin) | `GPIO20` | if needed |

> Available pins you noted: 5–10, 20, 21.  
> This project uses 2, 3, 4, 21 in the provided code. Adjust if your board revision differs.

---

## Wiring Diagram (ASCII)

```text
ESP32-C3 Super Mini                         Fan Supply (+Vfan)
--------------------                        -------------------
3V3 ----------------------------------+     
                                      |     
GND ------------------------------+---+-----------------------------+
                                   \                               |
GPIO4 (PWM_PIN) --[100-220R]-----> Fan PWM/Control (if used)       |
                               |                                    |
                               +--[10k]---> Fan GND (fan side)      |
                                   (pull-down, recommended)         |
                                                                     |
GPIO21 (FAN_GND_CUT_PIN) --[100-220R]--> Gate (IRLZ44N)             |
                                  |                                  |
                                [10k]                                |
                                  |                                  |
Source (IRLZ44N) -----------------+-------------------------------> GND common
Drain  (IRLZ44N) -----------------------------------------------> Fan negative (-)

Fan positive (+) ------------------------------------------------> +Vfan

Flyback diode across fan terminals:
- Cathode -> Fan +
- Anode   -> Fan -
```

---

## DS18B20 Wiring

- DS18B20 `VDD` → 3.3V  
- DS18B20 `GND` → GND  
- DS18B20 `DQ`  → GPIO3  
- **4.7k resistor** between `DQ` and 3.3V

---

## Build Notes / Electrical Safety

1. **Common ground is required** between ESP32 and fan supply.
2. Keep MOSFET gate wiring short.
3. Use:
   - 100–220Ω gate series resistor
   - 10k gate pulldown (gate→source)
4. Add flyback diode physically close to fan leads.
5. If fan still hums while “off”, verify:
   - PWM pin becomes `INPUT` in off-state
   - pull-down exists on fan control node
   - no alternate backfeed path from fan electronics

---

## Default Runtime Settings

- `tempStart = 70°F`
- `tempMax = 105°F`
- `tempMicOverride = 90°F`
- `micRawThreshold = 2000`
- `manualSpeed = 128`
- PWM: `25kHz`, 8-bit

---

## Logic Summary (Auto Mode)

1. Read mic and temperature.
2. If `tempF < tempMicOverride` and noise is above threshold:
   - fan target duty = 0 (mute)
3. Else:
   - ramp duty from `tempStart..tempMax` to `0..255`
   - enforce min run speed floor when duty > 0
4. Apply kickstart when transitioning from 0 to >0.
5. If resulting duty is 0:
   - PWM=0
   - ground cut OFF
   - PWM pin `INPUT` (tri-state)

---

## Accessing the Controller

- Device starts as AP: **`FanControllerV2`**
- Connect phone/laptop to that AP
- Open browser to:
  - `http://192.168.4.1/` (typical ESP softAP default)

---

## File

Main firmware file:
- `V2_ground`

---

## License

Personal project. Add your preferred open-source license if you plan to publish/share broadly.
