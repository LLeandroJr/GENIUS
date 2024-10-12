#include "hw_types.h"
#include "WATCHDOG_TIMER_Registers.h"

#define WAIT_WRITE_WSPR(W_PEND_WX_0_5) \
	while ((WDT1+WDT_WWPS) & (W_PEND_WX_0_5))

void disableWDT(){
	HWREG(WDT1+WDT_WSPR) = XXXX_AAAA;

	while (HWREG(WDT1+WDT_WWPS) & (1<<4));

	HWREG(WDT1+WDT_WSPR) = XXXX_5555;

	while (HWREG(WDT1+WDT_WWPS) & (1<<4));
}/* end of function disableWDT */

void enableWDT(){
	HWREG(WDT1+WDT_WSPR) = XXXX_BBBB;

	while (HWREG(WDT1+WDT_WWPS) & (1<<4));

	HWREG(WDT1+WDT_WSPR) = XXXX_4444;
	
    while (HWREG(WDT1+WDT_WWPS) & (1<<4));
}/* end of function	enableWDT */


#include "INTERRUPT_CONTROLLER_Registers.h"
#include "soc_AM335x.h"

void inicializationWDT(){

	disableWDT();
	// HWREG(WDT1+WDT_WCLR);
	// while(HWREG(SOC_WDT_1_REGS+WDT_WWPS) & (1<WDT_WCLR));

	// HWREG(WDT1+WDT_WCLR);
	// while(HWREG(SOC_WDT_1_REGS+WDT_WWPS) & (1<WDT_WCLR));

	unsigned timeCount=HWREG(SOC_WDT_1_REGS+WDT_WLDR);
	HWREG(WDT1+WDT_WDLY) = timeCount+ (0xFFFFFFFF-timeCount) - 10;
	while(HWREG(SOC_WDT_1_REGS+WDT_WWPS) & (1<WDT_WWPS_W_PEND_WDLY));

	// HWREG(WDT1+WDT_WCRR);
	// while(HWREG(SOC_WDT_1_REGS+WDT_WWPS) & (1<WDT_WWPS_W_PEND_WCRR);

	enableWDT();

	// HWREG(SOC_ADC_TSC_0_REGS+INTC_ILR91);
	HWREG(SOC_ADC_TSC_0_REGS+INTC_ILR_(91)) &= ~(1<<0) & ~(0x3F);

	HWREG(SOC_ADC_TSC_0_REGS+INTC_MIR_CLEAR2) |= (91 & 0x1F);

	// delay event
	HWREG(WDT1+WDT_WIRQENSET) |= (1<<1);	//enable 
	// HWREG(WDT1+WDT_WIRQENSET) |= ~(1<<1);	//disable

	// overflow event
	// HWREG(WDT1+WDT_WIRQENSET) |=  (1<<0);	//enable
	// HWREG(WDT1+WDT_WIRQENSET) |=  ~(1<<0);	//disable

}