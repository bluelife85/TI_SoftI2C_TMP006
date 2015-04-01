#include <iocc2541.h>
#include <softi2c.h>
#include "ti_tmp006.h"

#define BV(n)      (1 << (n))

/* SLEEPCMD bit definitions */
#define OSC_PD     BV(2)            /* Idle Osc: powered down=1 */
#define PMODE     (BV(1) | BV(0))   /* Power mode bits */

/* SLEEPSTA bit definitions */
#define XOSC_STB   BV(6)  /* XOSC: powered, stable=1 */
#define HFRC_STB   BV(5)  /* HFRCOSC: powered, stable=1 */

/* SLEEPCMD and SLEEPSTA bit definitions */
#define OSC_PD     BV(2)  /* 0: Both oscillators powered up and stable
                           * 1: oscillators not stable */

/* CLKCONCMD bit definitions */
#define OSC              BV(6)
#define TICKSPD(x)       (x << 3)
#define CLKSPD(x)        (x << 0)
#define CLKCONCMD_32MHZ  (0)
#define CLKCONCMD_16MHZ  (CLKSPD(1) | TICKSPD(1) | OSC)

#define OSC_32KHZ                     0x80

void xtal_init(void);
double sqrt(double d);

uint16 tamb;
uint16 vobj;

void delay_full(void){
	
	uint16 i,j;
	
	for(i=0;i<20;i++)
		for(j=0;j<65535;j++);
}

int main( void )
{
	xtal_init();
	TMP006_start();
	TMP006_config();
	
	while(1)
	{
		tamb = TMP006_readTAMBValue();
		vobj = TMP006_readVOBJValue();
		
		delay_full();
	}
}

void start_hsosc_xosc(void){
	
	SLEEPCMD &= ~OSC_PD;
	
	while(!(SLEEPSTA & XOSC_STB));
}

void set_osc_to_hsosc(void){
	
	CLKCONCMD = (CLKCONCMD & 0x80) | CLKCONCMD_16MHZ;
	while((CLKCONSTA & ~0x80) != CLKCONCMD_16MHZ);
}

void set_32khz_osc(void){
	
	CLKCONCMD = (CLKCONCMD & ~0x80) | OSC_32KHZ;
	while((CLKCONSTA & 0x80) != OSC_32KHZ);
}

void set_osc_to_xosc(void){
	
	CLKCONCMD = (CLKCONCMD & 0x80) | CLKCONCMD_32MHZ;
	while((CLKCONSTA & ~0x80) != CLKCONCMD_32MHZ);
}

void prefetch_en(void){
	
	FCTL = 0x08;
}

void xtal_init(void){
	
	start_hsosc_xosc();
	set_osc_to_hsosc();
	set_32khz_osc();
	set_osc_to_xosc();
	prefetch_en();
}