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

- [x] **Milestone 3**

## Hardware Setup:

### Test Rig:

| **Wire Color** | **Purpose**   | **Header Pin** | **Jumper Pin** |
| -------------- | ------------- | -------------- | -------------- |
| Black          | Ground        | GND            | GND            |
| White          | Altitude      | PE4            | J1-05          |
| Purple         | Yaw (Phase A) | PB0            | J1-03          |
| Grey           | Yaw (Phase B) | PB1            | J1-04          |

