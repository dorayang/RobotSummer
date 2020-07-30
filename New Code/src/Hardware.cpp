#include <Hardware.h>

void Motor::run_motor(short speed)
{
    // Serial1.println(current_speed != speed);
    // if (current_speed != speed)
    // {
    current_speed = speed;
    if (speed > 0)
    {
        pwm_stop(reverse_pin);
        speed = map(speed, 0, 100, 780, 1023);
        pwm_start(forward_pin, PWM_FREQUENCY * 5, speed, RESOLUTION_10B_COMPARE_FORMAT);
        pwm_start(reverse_pin, PWM_FREQUENCY * 5, 0, RESOLUTION_10B_COMPARE_FORMAT);
    }
    else if (speed < 0)
    {
        pwm_stop(forward_pin);
        speed = map(speed, -100, 0, -1023, -780);
        pwm_start(reverse_pin, PWM_FREQUENCY * 5, speed * -1, RESOLUTION_10B_COMPARE_FORMAT);
        pwm_start(forward_pin, PWM_FREQUENCY * 5, 0, RESOLUTION_10B_COMPARE_FORMAT);
    }
    else
    {
        pwm_stop(reverse_pin);
        pwm_stop(forward_pin);
        pwm_start(reverse_pin, PWM_FREQUENCY * 5, 0, RESOLUTION_10B_COMPARE_FORMAT);
        pwm_start(forward_pin, PWM_FREQUENCY * 5, 0, RESOLUTION_10B_COMPARE_FORMAT);
    }
    // }
}

bool run_both(short left_speed, short right_speed, unsigned int ms)
{
    unsigned int current_tick = HAL_GetTick();
    left_motor.run_motor(left_speed);
    right_motor.run_motor(right_speed);

    while (HAL_GetTick() - current_tick < ms)
    {
        if ((left_speed > 0 && right_speed > 0) || (left_speed < 0 && right_speed < 0))
        {
            short L = analogRead(L_SENSOR);
            short R = analogRead(R_SENSOR);

            if (L >= SETPOINT || R >= SETPOINT)
            {
                if (left_speed > 0 && right_speed > 0)
                {
                    left_motor.run_motor(0);
                    right_motor.run_motor(0);
                    pwm_start(ARM_SERVO, 50, ARM_UP, MICROSEC_COMPARE_FORMAT);
                    delay(500);
                    // pwm_stop(ARM_SERVO);

                    // take measurement again/,.
                    L = analogRead(L_SENSOR);
                    R = analogRead(R_SENSOR);

                    if (L >= SETPOINT || R >= SETPOINT)
                    {
                        left_motor.run_motor(-55);
                        right_motor.run_motor(-55);
                        delay(200);
                        away();
                    }
                    else
                    {
                        left_motor.run_motor(-55);
                        right_motor.run_motor(-55);
                        while (analogRead(L_SENSOR) < SETPOINT || analogRead(R_SENSOR < SETPOINT))
                        {
                        }
                        delay(200);
                        away();
                    }
                    return false;
                }
                else
                {
                    left_motor.run_motor(0);
                    right_motor.run_motor(0);
                    pwm_start(ARM_SERVO, 50, ARM_UP, MICROSEC_COMPARE_FORMAT);
                    delay(500);
                    // pwm_stop(ARM_SERVO);

                    // take measurement again/,.
                    L = analogRead(L_SENSOR);
                    R = analogRead(R_SENSOR);

                    if (L >= SETPOINT || R >= SETPOINT)
                    {
                        left_motor.run_motor(55);
                        right_motor.run_motor(55);
                        delay(200);
                        away();
                    }
                    else
                    {
                        left_motor.run_motor(55);
                        right_motor.run_motor(55);
                        while (analogRead(L_SENSOR) < SETPOINT || analogRead(R_SENSOR < SETPOINT))
                        {
                        }
                        delay(200);
                        away();
                    }
                    return false;
                }
            }
        }
    }
    left_motor.run_motor(0);
    right_motor.run_motor(0);
    return true;
}

void away()
{
    int t = HAL_GetTick();
    unsigned int turns = rand() % 800;
    left_motor.run_motor(-55);
    right_motor.run_motor(35);
    while (HAL_GetTick() - t < turns)
    {
    }
    run_both(40, 40, 1000);
}

void pick_up_can(bool correction)
{
    pwm_start(ARM_SERVO, 50, ARM_REST, MICROSEC_COMPARE_FORMAT);
    if (correction)
    {
        run_both(-55, 35, 20);
    }
    delay(500);
    bool mot = run_both(100, 100, 450);
    if (!mot)
    {
        return;
    }
    pwm_start(ARM_SERVO, 50, ARM_H_UP1, MICROSEC_COMPARE_FORMAT);
    mot = run_both(100, 100, 200);
    if (!mot)
    {
        pwm_start(ARM_SERVO, 50, ARM_UP, MICROSEC_COMPARE_FORMAT);
        delay(300);
        return;
    }
    pwm_start(ARM_SERVO, 50, ARM_H_UP2, MICROSEC_COMPARE_FORMAT);
    mot = run_both(100, 100, 200);
    if (!mot)
    {
        pwm_start(ARM_SERVO, 50, ARM_UP, MICROSEC_COMPARE_FORMAT);
        delay(300);
        return;
    }
    pwm_start(ARM_SERVO, 50, ARM_UP, MICROSEC_COMPARE_FORMAT);
    mot = run_both(100, 100, 200);
    if (!mot)
    {
        return;
    }
    mot = run_both(-80, -80, 300);
    if (!mot)
    {
        return;
    }
    run_both(-100, -100, 300);
}

/**
 * Dumps the cans stored in the robot bin. Won't work if the battery isn't 
 * full
*/
void dump()
{
    pwm_start(ARM_SERVO, 50, ARM_REST, MICROSEC_COMPARE_FORMAT);
    delay(500);
    run_both(0, 0, 10);
    pwm_start(BIN_SERVO, 50, BIN_UP, MICROSEC_COMPARE_FORMAT);
    delay(3000);
    pwm_start(BIN_SERVO, 50, BIN_REST, MICROSEC_COMPARE_FORMAT);
    delay(3000);
    pwm_stop(BIN_SERVO);
    pwm_start(ARM_SERVO, 50, ARM_UP, MICROSEC_COMPARE_FORMAT);
    delay(500);
    pwm_stop(ARM_SERVO);
}