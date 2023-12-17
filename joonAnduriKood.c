// Kodulabori QTR_RC jooneanduri näidisprogramm
// Peegeldumise mõõtmise funktsioon on blokeeruv
#include <stdio.h>
#include <homelab/pin.h>
#include <homelab/delay.h>
#include <homelab/timer.h>
#include <homelab/module/lcd_gfx.h>

// Kodulabor III
pin pin_rc_in_1 = PIN(B, 2);
pin pin_rc_in_2 = PIN(B, 1);
pin pin_rc_in_3 = PIN(B, 0);
pin pin_rc_in_4 = PIN(A, 7);

unsigned short QTR_RC_measure(pin rc_in_pin)
{
    if(_sys_freq == 2)
        Homelab_clock_init();

    // Pin setup
    pin_setup_output(rc_in_pin);

    // Reset timer
    TC_ClearOverflowFlag(&TCD1);
    TC_Restart(&TCD1);

    // Set timer D0 Normal mode
    // Top value 9000 (~270cm)
    // with clock freq of F_CPU / 64
    TC_SetPeriod(&TCD1,20000);
    TC1_ConfigClockSource( &TCD1, TC_CLKSEL_DIV1_gc);

    // Create trigger pulse
    pin_set(rc_in_pin);

    // Wait ~10 us
    while (TCD1.CNT < 50);

    // Reset timer
    TC_SetCount(&TCD1,0);

    // Wait for echo start
    pin_setup_input(rc_in_pin);

    while (pin_get_value(rc_in_pin))
    {
        // Timeout ?
        if (TC_GetOverflowFlag(&TCD1))
        {
            return 0;
        }
    }

    // Convert time to distance:
    //   distance = timer * (1 / (F_CPU / 8)) * speed / 2
    return (unsigned long)TCD1.CNT;
}


// Põhiprogramm
int main(void)
{
	unsigned short reading_1;
	unsigned short reading_2;
	unsigned short reading_3;
	unsigned short reading_4;
	char text[16];

	// LCD ekraani seadistamine
	lcd_gfx_init();
	lcd_gfx_clear();
	lcd_gfx_goto_char_xy(1, 1);
	lcd_gfx_write_string("Jooneandurid");

	// Väike paus
	sw_delay_ms(100);

 	// Lõputu tsükkel
	while (1)
	{
		// Mõõtmine
		reading_1 = QTR_RC_measure(pin_rc_in_1);
		//sw_delay_ms(10);
		reading_2 = QTR_RC_measure(pin_rc_in_2);
		//sw_delay_ms(10);
		reading_3 = QTR_RC_measure(pin_rc_in_3);
		//sw_delay_ms(10);
		reading_4 = QTR_RC_measure(pin_rc_in_4);

		// Mõõtmine õnnestus?
		if (reading_1 > 0)
		{
			// Kauguse tekstiks teisendamine
			sprintf(text, "%3d     ", reading_1);
		}
		// Mõõtmisel tekkis viga?
		else
		{
			// Vea tekst
			sprintf(text, "Viga    ");
		}

		// Teksti kuvamine LCD teise rea alguses
		lcd_gfx_goto_char_xy(3, 3);
		lcd_gfx_write_string(text);

		// Mõõtmine õnnestus?
		if (reading_2 > 0)
		{
			// Kauguse tekstiks teisendamine
			sprintf(text, "%3d     ", reading_2);
		}
		// Mõõtmisel tekkis viga?
		else
		{
			// Vea tekst
			sprintf(text, "Viga    ");
		}

		// Teksti kuvamine LCD teise rea alguses
		lcd_gfx_goto_char_xy(3, 4);
		lcd_gfx_write_string(text);

		// Mõõtmine õnnestus?
		if (reading_3 > 0)
		{
			// Kauguse tekstiks teisendamine
			sprintf(text, "%3d     ", reading_3);
		}
		// Mõõtmisel tekkis viga?
		else
		{
			// Vea tekst
			sprintf(text, "Viga    ");
		}

		// Teksti kuvamine LCD teise rea alguses
		lcd_gfx_goto_char_xy(3, 5);
		lcd_gfx_write_string(text);

		// Mõõtmine õnnestus?
		if (reading_4 > 0)
		{
			// Kauguse tekstiks teisendamine
			sprintf(text, "%3d     ", reading_4);
		}
		// Mõõtmisel tekkis viga?
		else
		{
			// Vea tekst
			sprintf(text, "Viga    ");
		}

		// Teksti kuvamine LCD teise rea alguses
		lcd_gfx_goto_char_xy(3, 6);
		lcd_gfx_write_string(text);

		// Väike paus
		sw_delay_ms(100);
	}
}