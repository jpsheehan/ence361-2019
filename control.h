#ifndef CONTROL_H_
#define CONTROL_H_

struct control_state_s
{
  /**
   * Gains.
   */
  float kp;
  float ki;
  float kd;

  // add other things that will be required for control stuff
};

/**
 * Represents the internal state of the control system.
 */
typedef struct control_state_s ControlState;

struct control_gains_s
{
  float kp;
  float ki;
  float kd;
};

/**
 * Represents some initial gains for a control system.
 */
typedef struct control_gains_s ControlGains;

/**
 * Initialises the two control systems using the specific gains.
 */
void control_init(ControlGains t_altitude_gains, ControlGains t_yaw_gains);

/**
 * Updates the altitude control system based on the any new data.
 */
void control_update_altitude(void);

/**
 * Updates the yaw control system based on any new data.
 */
void control_update_yaw(void);

#endif