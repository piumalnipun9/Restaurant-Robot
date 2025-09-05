#include <asf.h>

// Onboard LED pin definitions (PB27)
#define LED_PIO        PIOB
#define LED_PIN_MASK   PIO_PB27
#define LED_PIN_ID     ID_PIOB

// LED for 'B' ? Pin 46 = PC17
#define LEDB_PIO        PIOC
#define LEDB_PIN_MASK   (1u << 17)
#define LEDB_PIN_ID     ID_PIOC

// LED for 'C' ? Pin 48 = PC15
#define LEDC_PIO        PIOC
#define LEDC_PIN_MASK   (1u << 15)
#define LEDC_PIN_ID     ID_PIOC

// USART0 pin definitions (TX1 = PA11, RX1 = PA10)
#define UART_PIO       PIOA
#define UART_ID        ID_USART0
#define UART_RX_PIN    PIO_PA10A_RXD0
#define UART_TX_PIN    PIO_PA11A_TXD0
#define UART_PERIPHERAL USART0

void blink_led(Pio *pio, uint32_t pin_mask) {
	pio_set(pio, pin_mask);
	delay_ms(100);
	pio_clear(pio, pin_mask);
}

int main(void)
{
	// Initialize system clock and delay
	sysclk_init();
	delay_init(sysclk_get_cpu_hz());

	// Enable peripheral clocks
	pmc_enable_periph_clk(UART_ID);      // USART0
	pmc_enable_periph_clk(ID_PIOA);      // PIOA for UART
	pmc_enable_periph_clk(LED_PIN_ID);   // PIOB for LED
	pmc_enable_periph_clk(LEDB_PIN_ID);   // PC13
	pmc_enable_periph_clk(LEDC_PIN_ID);   // PC15

	// Configure USART0 RX/TX pins (Peripheral A)
	pio_configure(UART_PIO, PIO_PERIPH_A, UART_RX_PIN | UART_TX_PIN, PIO_DEFAULT);

	// Configure USART0 options
	sam_usart_opt_t usart_settings = {
		.baudrate     = 9600,
		.char_length  = US_MR_CHRL_8_BIT,
		.parity_type  = US_MR_PAR_NO,
		.stop_bits    = US_MR_NBSTOP_1_BIT,
		.channel_mode = US_MR_CHMODE_NORMAL
	};

	// Initialize USART0 with peripheral clock
	usart_init_rs232(UART_PERIPHERAL, &usart_settings, sysclk_get_peripheral_hz());

	// Enable RX and TX
	usart_enable_tx(UART_PERIPHERAL);
	usart_enable_rx(UART_PERIPHERAL);

	// Configure on board LED as output
	pio_set_output(LED_PIO, LED_PIN_MASK, LOW, DISABLE, ENABLE);
	pio_set_output(LEDB_PIO, LEDB_PIN_MASK, LOW, DISABLE, ENABLE); // D46
	pio_set_output(LEDC_PIO, LEDC_PIN_MASK, LOW, DISABLE, ENABLE);

	// Main loop
	while (1) {
		uint32_t received_char;
		if (usart_read(UART_PERIPHERAL, &received_char) == 0) {
			switch ((char)received_char) {
				case '1':
				blink_led(LED_PIO, LED_PIN_MASK);
				break;
				case '2':
				blink_led(LEDB_PIO, LEDB_PIN_MASK);
				break;
				case '3':
				blink_led(LEDC_PIO, LEDC_PIN_MASK);
				break;
			}
		}
	}
}