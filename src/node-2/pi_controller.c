/*
 * pi_controller.c - PID Position Controller Implementation
 * 
 * PID controller for motor position control
 */

#include "pi_controller.h"
#include <stdio.h>
#include <stdlib.h>  // For abs()

void pi_controller_init(pi_controller_t *controller, float Kp, float Ki, float Kd, float integral_max) {
    controller->Kp = Kp;
    controller->Ki = Ki;
    controller->Kd = Kd;
    controller->integral = 0.0f;
    controller->integral_max = integral_max;
    controller->target = 0;
    controller->last_error = 0;
    
    printf("PID Controller initialized: Kp=%.3f, Ki=%.3f, Kd=%.3f, integral_max=%.1f\n", 
           Kp, Ki, Kd, integral_max);
}

void pi_controller_reset(pi_controller_t *controller) {
    controller->integral = 0.0f;
    controller->last_error = 0;
}

void pi_controller_set_target(pi_controller_t *controller, int16_t target) {
    controller->target = target;
}

int8_t pi_controller_update(pi_controller_t *controller, int16_t current_position) {
    // Sample time for discrete PID (0.1ms = 0.0001s)
    const float T = 0.0001f;
    
    // 1. Calculate error (both target and position should be in same 0-100 range)
    float error = (float)(controller->target - current_position);
    
    // 2. Accumulate error (NO time scaling in the sum - that comes later!)
    controller->integral += error;
    
    // 3. Clamp integral to prevent windup
    if (controller->integral > controller->integral_max) {
        controller->integral = controller->integral_max;
    } else if (controller->integral < -controller->integral_max) {
        controller->integral = -controller->integral_max;
    }
    
    // 4. Calculate PID terms - apply T here, matching working repo formula:
    // u_p = Kp * error
    // u_i = T * Ki * sum_error  ← Time applied when computing term, not in sum!
    // u_d = (Kd / T) * (error - prev_error)  ← Time in denominator
    float p_term = controller->Kp * error;
    float i_term = T * controller->Ki * controller->integral;  
    float d_term = (controller->Kd / T) * (error - (float)controller->last_error);
    
    // Store error for next iteration
    controller->last_error = (int16_t)error;
    
    // 5. Calculate total output (PID)
    float output = p_term + i_term + d_term;
    
    // 6. Clamp output to motor limits (-100 to +100)
    int8_t motor_cmd = 0;
    if (output > 100.0f) {
        motor_cmd = 100;
    } else if (output < -100.0f) {
        motor_cmd = -100;
    } else {
        motor_cmd = (int8_t)output;
    }
    
    return motor_cmd;
}

int16_t pi_controller_get_error(pi_controller_t *controller) {
    return controller->last_error;
}
