#include <stddef.h>
#include <stdint.h>
#include "reg.h"
#include "threads.h"
#include "support.h"

#define MAX_INPUT 50

/* USART TXE Flag
 * This flag is cleared when data is written to USARTx_DR and
 * set when that data is transferred to the TDR
 */
#define USART_FLAG_TXE	((uint16_t) 0x0080)

/* USART RXNE Flag */
#define USART_FLAG_RXNE ((uint16_t) 0x0020)

extern int fibonacci(unsigned int x);
void command_detect(char *str);
int lock = 0;

void usart_init(void)
{
	*(RCC_APB2ENR) |= (uint32_t) (0x00000001 | 0x00000004);
	*(RCC_APB1ENR) |= (uint32_t) (0x00020000);

	/* USART2 Configuration, Rx->PA3, Tx->PA2 */
	*(GPIOA_CRL) = 0x00004B00;
	*(GPIOA_CRH) = 0x44444444;
	*(GPIOA_ODR) = 0x00000000;
	*(GPIOA_BSRR) = 0x00000000;
	*(GPIOA_BRR) = 0x00000000;

	*(USART2_CR1) = 0x0000000C;
	*(USART2_CR2) = 0x00000000;
	*(USART2_CR3) = 0x00000000;
	*(USART2_CR1) |= 0x2000;
}

void print_char(const char *str) 
{
	if (*str) {
		while (!(*(USART2_SR) & USART_FLAG_TXE));
		*(USART2_DR) = (*str & 0xFF);
	}
}

void print_str(const char *str)
{
	while (*str) {
		while (!(*(USART2_SR) & USART_FLAG_TXE));
		*(USART2_DR) = (*str & 0xFF);
		str++;
	}
}

char get_char(void)
{
	while (!(*(USART2_SR) & USART_FLAG_RXNE));
	return *(USART2_DR);
}

void fib()
{
	while (!lock);
	char num[MAX_INPUT];
	int fib_result;
	fib_result = fibonacci(thread_get_userdata());
	itoa(fib_result, num);
	print_str(num);
	print_char("\n");
	lock = 0;
}

void shell_input(char *buffer)
{
	int index = 0;
	while (1) {
		buffer[index] = get_char();

		if (buffer[index] == 13) {
			print_char("\0");
			buffer[index] = '\0';
			command_detect(buffer);
			break;
		} else if (buffer[index] == 8 || buffer[index] == 127) {
			if (index != 0) {
				print_char("\b");
				print_char(" ");
				print_char("\b");
				index--;
			}
		} else {
			print_char(&buffer[index++]);
		}
		if (index == MAX_INPUT)
			index--;
	}
}

void command_detect(char *str)
{
	char fib_num[MAX_INPUT];
	unsigned int number;
	if (strcmp("fibonacci", str)) {
		print_str("\nenter fib number...: ");
		shell_input(fib_num);
		number = atoi(fib_num);
		if (thread_create((void *) fib, (void *) (number)) == -1)
			print_str("Thread 1 creation failed\r\n");
		lock = 1;
	}
}

void shell(void *userdata)
{
	char buffer[MAX_INPUT];
	while (1) {
		while (lock);
		print_str("nobody@mini-arm-os:");

		shell_input(buffer);

		print_char("\n");
	}
}

/* 72MHz */
#define CPU_CLOCK_HZ 72000000

/* 100 ms per tick. */
#define TICK_RATE_HZ 10

int main(void)
{
	const char *str = "task_shell";

	usart_init();

	if (thread_create(shell, (void *) str) == -1)
		print_str("Thread 1 creation failed\r\n");

	/* SysTick configuration */
	*SYSTICK_LOAD = (CPU_CLOCK_HZ / TICK_RATE_HZ) - 1UL;
	*SYSTICK_VAL = 0;
	*SYSTICK_CTRL = 0x07;

	thread_start();

	return 0;
}
