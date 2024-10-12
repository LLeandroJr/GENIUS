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

#define USR0    (21)    // INTERNAL LED gpio1_21
#define USR1    (22)    // INTERNAL LED gpio1_22
#define USR2    (23)    // INTERNAL LED gpio1_23
#define USR3    (24)    // INTERNAL LED gpio1_24
#define TIMER5							(5)    // EXTERNAL PIN gpio2_3 BUZZER
#define TIMER7							(3)    // EXTERNAL PIN gpio2_3
#define TIMER6							(4)    // EXTERNAL PIN gpio2_4
#define GPIO2_1							(1)    // EXTERNAL PIN gpio2_1
#define GPIO2_2							(2)    // EXTERNAL PIN gpio2_2
#define BUTTON1 (16)
#define BUTTON2 (28)
#define BUTTON3 (14)
#define BUTTON4 (15)


#define _typesel_PULLUP					(1<<4)		// 1: Pullup selected
#define _typesel_PULLDOWN				(~(1<<4))	// 0: Pulldown selected
#define _puden_enable					(~(1<<3))	// 0: Pullup/pulldown enabled
#define _puden_disable					(1<<3)		// 1: Pullup/pulldown disabled

#define clearTerminal					0, "\033[H\033[J\r", 8

_uint32 TIME_DELAY;

static void sysconfig(void);
void inToString(unsigned number);

// Definição de LEDs e Botões
const int leds[] = {TIMER7, TIMER6, GPIO2_1, GPIO2_2};
const int sequences[7][7] = {
    {3, -1, -1, -1, -1, -1, -1},   // Fase 1
    {3, 1, -1, -1, -1, -1, -1},   // Fase 2
    {3, 1, 2, -1, -1, -1, -1},   // Fase 3
    {3, 1, 2, 0, -1, -1, -1},   // Fase 4
    {3, 1, 2, 0, 1, -1, -1},   // Fase 5
    {3, 1, 2, 0, 1, 2, -1},   // Fase 6
    {3, 1, 2, 0, 1, 2, 0},   // Fase 7
};

const int buttons[] = {BUTTON1, BUTTON2, BUTTON3, BUTTON4};

int currentPhase = 0; 
int gameActive = 1; 
int buttonPressed = -1; // Último botão pressionado

void showSequence() {
    for (int i = 0; i <= currentPhase; i++) {
        HWREG(SOC_GPIO_2_REGS + GPIO_SETDATAOUT) = (1 << leds[sequences[currentPhase][i]]);
        delay_ms(TIME_DELAY);
        HWREG(SOC_GPIO_2_REGS + GPIO_CLEARDATAOUT) = (1 << leds[sequences[currentPhase][i]]);
        delay_ms(TIME_DELAY);
    }
}

void resetGame() {
    currentPhase = 0;
    gameActive = 1;
    putString(0, "Game Over! Reiniciando...\n\r", 28);
    delay_ms(1000);
}

void nextPhase() {
    currentPhase++;
    if (currentPhase == 20) {  // Se chegou à fase 20, o jogador venceu
        putString(0, "Você venceu todas as fases!\n", 29);
        resetGame();
    }else{
        delay_ms(TIME_DELAY);
    }
}

void gameOver() {
    gameActive = 0;
    resetGame();
}

int checkButton(int buttonPressed, int positionInSequence) {
    int expectedButton = sequences[currentPhase][positionInSequence];
    if (buttonPressed == expectedButton) {
        HWREG(SOC_GPIO_2_REGS + GPIO_SETDATAOUT) = (1 << TIMER5);
        delay_ms(TIME_DELAY);
        HWREG(SOC_GPIO_2_REGS + GPIO_CLEARDATAOUT) = (1 << TIMER5);
        delay_ms(TIME_DELAY);
        return 1;  // Botão correto
    }
    return 0;  // Botão incorreto
}

char opcao(UART_t uart) {
    return getCh(uart); // Assume que getCh retorna um caractere
}

void escolha(UART_t uart) {
    char option = opcao(uart); // Chama a função opcao para obter a entrada do usuário
    
    // Converte o caractere lido em um número inteiro
    int choice = option - '0'; // Assume que a entrada é um dígito ASCII ('1' a '5')
    
    switch(choice) {
        case 1:
            TIME_DELAY = 2000;
            break;
        case 2:
            TIME_DELAY = 1500;
            break;
        case 3:
            TIME_DELAY = 1000;
            break;
        case 4:
            TIME_DELAY = 250;
            break;
        case 5:
            TIME_DELAY = 0;
            break;
        default:
            gameOver(TIME_DELAY);
            break; // Corrigido: adicionado ponto e vírgula
    }
} 

// void ISR_Handler(void);

int _main(void){
	putString(clearTerminal);
    // disableWDT();
	inicializationWDT();
  	sysconfig();
	timerSetup();

	// Variável para armazenar a entrada do usuário
    char option;

    // Exibe o menu e aguarda a entrada do usuário

    uartClearBuffer(UART0);
    putString(UART0, "Escolha um nivel de dificuldade:\n\n\r", 36);
    uartClearBuffer(UART0);
    putString(UART0, "[1] Facil\n\r", 12);
    uartClearBuffer(UART0);
    putString(UART0, "[2] Normal\n\r", 13);
    uartClearBuffer(UART0);
    putString(UART0, "[3] Dificil\n\r", 14);
    uartClearBuffer(UART0);
    putString(UART0, "[4] Helder\n\r", 13);
    uartClearBuffer(UART0);
    putString(UART0, "[5] Elvis\n\r", 12);
    uartClearBuffer(UART0);

    // Aqui você espera pela entrada do usuário e define TIME_DELAY
    uartClearBuffer(UART0);
    UART_t uart = UART0;
    
    // Obtem a opção do usuário
    option = opcao(uart);
    
    // Converte o caractere lido em um número inteiro
    int choice = option - '0'; // Assume que a entrada é um dígito ASCII ('1' a '5')

    switch(choice) {
        case 1:
            TIME_DELAY = 500;
            break;
        case 2:
            TIME_DELAY = 350;
            break;
        case 3:
            TIME_DELAY = 200;
            break;
        case 4:
            TIME_DELAY = 150;
            break;
        case 5:
            TIME_DELAY = 50;
            break;
        default:
            gameOver();
            break;
    }

  	while (gameActive) {
    showSequence(); // Mostra a sequência até a fase atual
    for (int i = 0; i <= currentPhase; i++) {
        buttonPressed = -1;

        // Espera por uma entrada de botão válida
        while (buttonPressed == -1) {
            // Aguardar até que uma interrupção de botão ocorra
        }

        // Verifica se o botão pressionado está correto
        if (!checkButton(buttonPressed, i)) {
            gameOver(); // Reinicia o jogo se o botão estiver errado
            break;
        }
    }
    if (gameActive) {
        nextPhase(); // Avança para a próxima fase se todos os botões estiverem corretos
    }
}

	return(0);
}/* end of function main */

void sysconfig( ){
	interrupts(TINT7, IRL_priorityMax, IRQ);
	interrupts(WDT1INT, IRL_priorityMax, IRQ);
    interrupts(GPIOINT1B, IRL_priorityMax, IRQ);
    interrupts(GPIOINT1A, IRL_priorityMax, IRQ);

	Init_module_gpio(GPIO1);
	Init_module_gpio(GPIO2);

   	Init_pin_gpio(GPIO1, OUTPUT, USR0);
    Init_pin_gpio(GPIO1, OUTPUT, USR1);
    Init_pin_gpio(GPIO1, OUTPUT, USR2);
    Init_pin_gpio(GPIO1, OUTPUT, USR3);
    Init_pin_gpio(GPIO2, OUTPUT, TIMER7);
	Init_pin_gpio(GPIO2, OUTPUT, TIMER6);
	Init_pin_gpio(GPIO2, OUTPUT, GPIO2_1);
	Init_pin_gpio(GPIO2, OUTPUT, GPIO2_2);
    Init_pin_gpio(GPIO2, OUTPUT, TIMER5);

    Init_pin_gpio(GPIO1, INPUT, BUTTON1);
    Init_pin_gpio(GPIO1, INPUT, BUTTON2);
    Init_pin_gpio(GPIO1, INPUT, BUTTON3);
    Init_pin_gpio(GPIO1, INPUT, BUTTON4);

    // Configurar interrupções GPIO
    interruptConfigGPIO(SOC_GPIO_1_REGS, 1, BUTTON1);
    interruptConfigGPIO(SOC_GPIO_1_REGS, 1, BUTTON2);
    interruptConfigGPIO(SOC_GPIO_1_REGS, 1, BUTTON3);
    interruptConfigGPIO(SOC_GPIO_1_REGS, 1, BUTTON4);
}/*	end of function ledInit */


void inToString(unsigned number){
	char valor[15]={'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0'};
	int i=15;

	while(number>0){
		valor[--i]=(number%10)+0x30;

		number/=10;
	}

	putString(0,valor,15);
	putString(0,"\n\r",3);
}




