#include <stdio.h>
#include <homelab/pin.h>
#include <homelab/delay.h>
#include <homelab/module/sensors.h>
#include <homelab/module/segment_display.h>
// Kodulabor III ultraheli SRF05 anduri signaali viik
pin pin_trigger = PIN(B, 1);
pin pin_echo    = PIN(B, 2);
// Põhiprogramm
    int main(void) {
     // Distants in cm
     unsigned short distance;
     // 7-segmendilise indikaatori seadistamine
     segment_display_init();
     // Lõputu tsükkel
     while (1) {
         // Väike paus
         sw_delay_ms(500);
         // Mõõtmine
         distance = ultrasonic_measure_srf04(pin_trigger, pin_echo);
         // Mõõtmine õnnestus -> salvesta tulemus in dm
         if (distance > 0) {
            distance = distance;
         }
         // Mõõtmisel tekkis viga?
         else {
            distance = 0;
         }
         segment_display_write(distance / 10);
     }
}
