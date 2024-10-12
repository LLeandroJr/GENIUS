/*
 =====================================================================================
 *
 *	Filename:  main.c
 *
 *	Description:  
 *
 *	Version:  a.0
 *	Created:  dd/mm/aa hh:mm:ss
 *	Revision:  none
 *	Compiler:  arm-none-eabi-gcc
 *
 *	Author:  Diogo Bandeira, user.email@gmail.com
 *	Author:  Leandro Rodrigues da Silva Júnior, leandro.jr369@gmail.com
 *
 * =====================================================================================
 */

#include "hw_types.h"
#include "soc_AM335x.h"
#include "CLOCK_MODULE_Registers.h"
#include "CONTROL_MODULE_Registers.h"
#include "GPIO_Registers.h"
#include "INTERRUPT_SERVICE_ROUTINE.h"
#include "TIMERS_Registers.h"
#include "WATCHDOG_TIMER_Registers.h"
#include "uart.h"

#include "types.h"

#define USR0							(21)	// INTERNAL LED gpio1_21
#define USR1							(22)	// INTERNAL LED gpio1_22
#define USR2							(23)	// INTERNAL LED gpio1_23
#define USR3							(24)	// INTERNAL LED gpio1_24

#define _typesel_PULLUP					(1<<4)		// 1: Pullup selected
#define _typesel_PULLDOWN				(~(1<<4))	// 0: Pulldown selected
#define _puden_enable					(~(1<<3))	// 0: Pullup/pulldown enabled
#define _puden_disable					(1<<3)		// 1: Pullup/pulldown disabled

#define clearTerminal					0, "\033[H\033[J\r", 8

_uint32 TIME_DELAY;

static void sysconfig(void);

// void ISR_Handler(void);

int _main(void){
	putString(clearTerminal);
    disableWDT();
  	sysconfig();
	timerSetup();

	TIME_DELAY=250;

  	while (1){
		HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) =  (1<<USR3);
		// HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) =  (1<<28);
		
		delayInterrupt_ms(TIME_DELAY);
		HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = (1<<USR3);
		// HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = (1<<28);
		delayInterrupt_ms(TIME_DELAY);
	}

	return(0);
}/* end of function main */

void sysconfig( ){
	interrupts(TINT7, IRL_priorityMax, IRQ);

	Init_module_gpio(GPIO1);
	Init_module_gpio(GPIO2);

   	Init_pin_gpio(GPIO1, OUTPUT, USR0);
	Init_pin_gpio(GPIO1, OUTPUT, USR1);
	Init_pin_gpio(GPIO1, OUTPUT, USR2);
	Init_pin_gpio(GPIO1, OUTPUT, USR3);

	Init_pin_gpio(GPIO1, OUTPUT, 28);
}/*	end of function ledInit */





