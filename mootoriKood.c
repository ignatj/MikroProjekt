#include <homelab/module/motors.h>
#include <homelab/adc.h>
// Põhiprogramm
int main(void)
{
    // Nupude seadistus
    pin_setup_input(S1);
    pin_setup_input(S2);
    pin_setup_input(S3);
    // Kiiruse hoidmise muutuja
    int speed = 100;
    // Analoog-digitaalmuunduri käivitamine
    adc_init(ADC_REF_AVCC, ADC_PRESCALE_8);
    // Mootorite eelseadistamine
    dcmotor_drive_pwm_init(1, TIMER_NO_PRESCALE);
    // Lõputu tsükkel
    while (1) {
         // Ootame selleks, et kiiruse vahetamine toimuks umbes 10ms+ kestvusega taktis
        hw_delay_ms(10);
         // Kui kiirus on mitte maksimaalne ja S1 vajutatud, siis suurendame kiirust
        if(button_read(S1) && speed < 255) {
            speed += 1;
        }
        // Kui kiirus on mitte minimaalne ja S2 vajutatud, siis vähendame kiirust
        else if(button_read(S2) && speed > 0) {
            speed -= 1;
        }
        // Kui S3 vajutatud, siis paneme kiirus nulliks
        else if(button_read(S3)) {
            speed = 0;
        }
        // Edastame mootorile uue kiiruse
        dcmotor_drive_pwm(1, 1, speed);
    }
}