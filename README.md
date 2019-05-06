# Helicopter Project

Written by Manu Hamblyn, Will Cowper and Jesse Sheehan.

## Milestone Progress:

- [x] **Milestone 1**
  - [x] Sample helicopter altitude
  - [x] Store samples in circular buffer
  - [x] Store the initial altitude reference
  - [x] Altitude should be displayed as a percentage
  - [x] The "LEFT" button should recalibrate the altitude reference
  - [x] The "UP" button should cycle through the display states (percentage altitude > mean ADC value > off)
  - [x] The source should be pushed to the GitLab on the day of lab

- [x] **Milestone 2**
  - [x] All tasks from Milestone 1
  - [x] Continuous yaw monitoring
  - [x] Calculate yaw in degrees relative to the initial position
  - [x] The yaw (in degrees) should be displayed along with the averaged altitude

- [ ] **Milestone 3**
  - [x] Helicopter yaw should be processed by interrupt-driven or polled decoding.
  - [ ] At the start of the sortie, the helicopter should rotate to a known reference position, indicated by a HIGH input from the independent yaw reference signal.
  - [ ] A reference minimum altitude reading should be taken before take-off.
  - [ ] Two output PWM signals should be generated. The PWM signals should be in the range from 150 Hz to 300 Hz:
      - [ ] One for the main rotor (module 0, PWM 7).
      - [ ] One for the tail rotor (module 1, PWM 5).
      - [ ] The duty cycles (as percentages) for each motor should be displayed on the OLED display.
  - [ ] The SW1 slider switch on the Orbit board should control the mode of the helicopter (DOWN position = landed/in the process of landing, changing from DOWN to UP when the helicopter is landed should cause the helicopter to take off).
      - [ ] The helicopter should land smoothly, facing the reference orientation. When landing is complete, the rotors should stop.
      - [ ] At take off the rotors should start and the helicopter should rotate to the reference orientation.
      - [ ] While the helicopter is landing, any change to the switch position should be ignored until landing is complete.
      - [ ] When the program is started/restarted/reset, the helicopter should be landed with the motors off regardless of the SW1 position.
  - [ ] The four buttons on the Tiva-Orbit board should be programmed to interactively control the altitude and yaw of the helicopter:
      - [ ] UP button: Increase altitude by 10% of its total vertical range. If the helicopter is already within 10% of its maximum altitude, pressing the UP button should take it to its maximum altitude. When the helicopter is at its maximum altitude, any UP presses should be ignored.
      - [ ] DOWN button: Decrease altitude by 10%. If within 10% of minumum altitude, pushing the DOWN button should take the helicopter to its minimum vertical range. DOWN button presses when already at minumum range should be ignored.
      - [ ] LEFT button: Rotate the helicopter counter clockwise 15 degrees.
      - [ ] RIGHT button: Rotate the helicopter clockwise 15 degrees.
      - [ ] Pressing LEFT/RIGHT buttons should not affect the altitude.
  - [ ] The virtual button signals from the web interface are asserted for 200 ms. The virtual slider UP position is HIGH.
  - [ ] The program should have a real-time foreground/background kernel operating on a round-robin basis. Robust behaviour should be maintained at all times.
  - [ ] A PI (preferred) or PID (acceptable) controller should be used to acheive flight.
  - [ ] Information on the status of the helicopter should be transmitted via a serial link from UART0 at 9600 baud, with 1 stop bit and no parity bit in each transmitted byte. The information should be concise but in an easily readable format that is compatible with the scrolling display of the remote lab web page. Updates should be transmitted regularly at least 4 times per second. Status information should include:
      - [ ] The desired yaw (degrees).
      - [x] The actual yaw (degrees).
      - [ ] The desired altitude (percentage of maximum alt.).
      - [x] The actual altitude (percentage of maximum alt.).
      - [ ] The duty cycle of each of the PWM signals controlling the rotors (%, 0 meaning off).
      - [ ] The current operating mode.
  - [ ] The program should use the pin-map as outlined [below](#pin-map).

## Hardware Setup:

### Test Rig:

| **Wire Color** | **Purpose**   | **Header Pin** | **Jumper Pin** |
| -------------- | ------------- | -------------- | -------------- |
| Black          | Ground        | GND            | GND            |
| White          | Altitude      | PE4            | J1-05          |
| Purple         | Yaw (Phase A) | PB0            | J1-03          |
| Grey           | Yaw (Phase B) | PB1            | J1-04          |

### Pin Map:

| **Tiva Pin** | **Tiva Function** | **In/Out** | **Helicopter Signal** | **Notes**                                              |
| ------------ | ----------------- | ---------- | --------------------- | ------------------------------------------------------ |
| J1-10        | PA7               | In         | MODE                  | Slider switch (UP = HIGH) & virtual signal             |
| J2-03        | PE0               | In         | UP                    | Active High; virtual signal pulses HIGH on operation   |
| J3-05        | PD2               | In         | DOWN                  | Active High; virtual signal pulses HIGH on operation   |
| J4-10        | PF4               | In         | CCW                   | Active Low; virtual signal pulses LOW on operation     |
| J2-04        | PF0               | In         | CW                    | Active Low; virtual signal pulses LOW on operation     |
| J1-09        | PA6               | In         | RESET                 | Active Low; virtual signal pulses LOW on operation     |
| J3-10        | PF1 (M1PWM5)      | Out        | Tail Rotor Motor      | 2% <= duty cycle <= 98%, otherwise off                 |
| J1-03        | PB0               | In         | Yaw Channel A         | Channel B leads channel A when yaw increases clockwise |
| J1-04        | PB1               | In         | Yaw Channel B         |                                                        |
| J4-04        | PC4               | In         | Yaw Reference         | LOW indicates helicopter is at reference position      |
| J4-05        | PC5 (M0PWM7)      | Out        | Main Rotor Motor      | 2% <= duty cycle <= 98%, otherwise off                 |
| J1-05        | PE4 (M0AIN9)      | In         | Altitude (analogue)   | Approx. range 1 - 2 V                                  |

