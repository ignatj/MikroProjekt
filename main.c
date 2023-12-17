#include <stdio.h>
#include <homelab/delay.h>
#include <homelab/timer.h>
//#include <homelab/module/lcd_gfx.h>
#include <homelab/pin.h>
#include <homelab/module/sensors.h>
//#include <homelab/module/segment_display.h>
#include <homelab/module/motors.h>
#include <homelab/adc.h>

#define STATE_OTSI 1
#define STATE_GAZGAZ 2

#define VASAK_MOOTOR 1
#define PAREM_MOOTOR 2

#define PARIPAEVA 1
#define VASTUPAEVA -1
#define SEIS 0

// Funktsioonide loetelu
unsigned short QTR_RC_measure(pin rc_in_pin);

// Joon Andur parem
pin pin_rc_in_1_p = PIN(B, 2);
pin pin_rc_in_2_p = PIN(B, 1);
pin pin_rc_in_3_p = PIN(B, 0);
pin pin_rc_in_4_p = PIN(A, 7);

// Joon Andur vasak
pin pin_rc_in_1 = PIN(A, 6);
pin pin_rc_in_2 = PIN(A, 5);
pin pin_rc_in_3 = PIN(A, 4);
pin pin_rc_in_4 = PIN(A, 3);

// Ultraheli andur
pin pin_trigger = PIN(A, 1);
pin pin_echo    = PIN(A, 2);

// Nupp
pin_setup_input(S1);


int main(void) {
    int state = STATE_OTSI;

    int speed = 60;

    unsigned short distance = 80000;

    // MOOTORITE SEADISTUS
    adc_init(ADC_REF_AVCC, ADC_PRESCALE_8);
    dcmotor_drive_pwm_init(VASAK_MOOTOR, TIMER_NO_PRESCALE);
    dcmotor_drive_pwm_init(PAREM_MOOTOR, TIMER_NO_PRESCALE);

    // JOON ANDURI SEADISTUS
    unsigned short reading_1_p;
    unsigned short reading_2_p;
    unsigned short reading_3_p;
    unsigned short reading_4_p;
    unsigned short reading_1;
    unsigned short reading_2;
    unsigned short reading_3;
    unsigned short reading_4;

    // parast nuppu vajutamist oota 5 sek ja siis toimi
    while (!button_read(S1)) {
        hw_delay_ms(10);
    }
    hw_delay_ms(5000);

    while(1) {

        if (state == STATE_OTSI) {
            speed = 60;
            dcmotor_drive_pwm(VASAK_MOOTOR, PARIPAEVA, speed);
            dcmotor_drive_pwm(PAREM_MOOTOR, VASTUPAEVA, speed);
            while(true) {
                distance = ultrasonic_measure_srf04(pin_trigger, pin_echo)/10;
                if (distance != 0 && distance < 14) {
                    dcmotor_drive_pwm(VASAK_MOOTOR, SEIS, speed);
                    dcmotor_drive_pwm(PAREM_MOOTOR, SEIS, speed);
                    state = STATE_GAZGAZ;
                    break;
                }
                hw_delay_ms(60);
            }
        }

        if (state == STATE_GAZGAZ) {
            speed = 250;
            dcmotor_drive_pwm(VASAK_MOOTOR, PARIPAEVA, speed);
            dcmotor_drive_pwm(PAREM_MOOTOR, PARIPAEVA, speed);
            while (true) {
                hw_delay_ms(20);
                reading_1_p = QTR_RC_measure(pin_rc_in_1_p);
           		reading_2_p = QTR_RC_measure(pin_rc_in_2_p);
                reading_3_p = QTR_RC_measure(pin_rc_in_3_p);
                reading_4_p = QTR_RC_measure(pin_rc_in_4_p);
                reading_1 = QTR_RC_measure(pin_rc_in_1);
                reading_2 = QTR_RC_measure(pin_rc_in_2);
                reading_3 = QTR_RC_measure(pin_rc_in_3);
                reading_4 = QTR_RC_measure(pin_rc_in_4);
                if (reading_1_p > 100 && reading_2_p > 100 && reading_3_p > 100 && reading_4_p > 100) {
                    reading_p = (reading_1_p + reading_2_p + reading_3_p + reading_4_p)/4;
                    if (reading_p > 1100) {
                        state = STATE_OTSI;
                        dcmotor_drive_pwm(VASAK_MOOTOR, VASTUPAEVA, speed);
                        dcmotor_drive_pwm(PAREM_MOOTOR, VASTUPAEVA, speed);
                        hw_delay_ms(500);
                        break;
                    }
                }
                if (reading_1 > 100 && reading_2 > 100 && reading_3 > 100 && reading_4 > 100) {
                    reading = (reading_1 + reading_2 + reading_3 + reading_4)/4;
                    if (reading > 1100) {
                        state = STATE_OTSI;
                        dcmotor_drive_pwm(VASAK_MOOTOR, VASTUPAEVA, speed);
                        dcmotor_drive_pwm(PAREM_MOOTOR, VASTUPAEVA, speed);
                        hw_delay_ms(500);
                        break;
                    }
                }
            }
        }
    }
}

// Funktsioonide impl
unsigned short QTR_RC_measure(pin rc_in_pin) {
    if(_sys_freq == 2)
        Homelab_clock_init();
    pin_setup_output(rc_in_pin);
    TC_ClearOverflowFlag(&TCD1);
    TC_Restart(&TCD1);
    TC_SetPeriod(&TCD1,20000);
    TC1_ConfigClockSource( &TCD1, TC_CLKSEL_DIV1_gc);
    pin_set(rc_in_pin);
    while (TCD1.CNT < 50);
    TC_SetCount(&TCD1,0);
    pin_setup_input(rc_in_pin);
    while (pin_get_value(rc_in_pin))
    {
        if (TC_GetOverflowFlag(&TCD1))
        {
            return 0;
        }
    }
    return (unsigned long)TCD1.CNT;
}

