
/**
 * This example will flood the UART0_TX line with hello world one byte at a time and then
 * read out the same number of bytes on the UART0_RX line then print out the results.
 *
 * Make sure to set the proper address range in xparameters.h for the
 * STDIN_BASEADDRESSES and STDOUT_BASEADDRESSES
 * They should reflect the XPAR_PS7_UART_1_BASEADDR value
 *
 * Expected output (prints via usb/uart with UART1 controller):
 * ```
 * UART0: SentCount = 12 B, RecvCount = 12 B, Received = "hello world"
 *```
 */

#include "xparameters.h"
#include "xuartps.h"
#include <stdio.h>

//#define STDIN_BASEADDRESS XPAR_PS7_UART_1_BASEADDR
//#define STDOUT_BASEADDRESS XPAR_PS7_UART_1_BASEADDR

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define UART_DEVICE_ID			XPAR_XUARTPS_0_DEVICE_ID

#define TEST_BUFFER_SIZE 		16

/* problem in vivado 2015.4 does not map user ip in xparameters.h */
/* must verify base address for PMOD_RS485 in vivado address editor tab */

int pmod_rs485_ps_uart_example(u16 DeviceId);

XUartPs Uart_Ps;		/* The instance of the UART Driver */

int main(void)
{
	int Status;
	/* run example UART read/write */
	Status = pmod_rs485_ps_uart_example(UART_DEVICE_ID);
	return Status;
}

/**
 * sends and receives data from pmod device
 */
int pmod_rs485_ps_uart_example(u16 DeviceId)
{
	int SentCount = 0;
	int RecvCount = 0;
	int Status;
	XUartPs_Config *Config;

	/*
	 * Initialize the UART driver so that it's ready to use
	 * Look up the configuration in the config table and then initialize it.
	 */

	xil_printf("\n\rSystem start.\n\r");

	Config = XUartPs_LookupConfig(DeviceId);
	if (NULL == Config) {
		return XST_FAILURE;
	}

	Status = XUartPs_CfgInitialize(&Uart_Ps, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XUartPs_SetBaudRate(&Uart_Ps, 115200);

	u8 recv_buf[TEST_BUFFER_SIZE];

	/* Clear receive buffer */
	for(int Index = 0; Index < TEST_BUFFER_SIZE-1; Index++) {
		recv_buf[Index] = 0;
	}

	u8 send_buf[]=  {0x56, 0x00, 0x36, 0x01, 0x00};  // GET IMAGE
	//u8 send_buf[]=  {0x56, 0x00, 0x26, 0x00};    // RESET

	SentCount = 0;
	while (SentCount < sizeof(send_buf)) {
		/* Transmit the data */
		SentCount += XUartPs_Send(&Uart_Ps, &send_buf[SentCount], 1);
		xil_printf("Sent count = %d, Data = 00x%02X\r", SentCount, send_buf[SentCount-1]);
	}

	RecvCount = 0;
	while(RecvCount < TEST_BUFFER_SIZE){
		/* Receive the data */
		RecvCount += XUartPs_Recv(&Uart_Ps, &recv_buf[RecvCount], 1);
		xil_printf("Received count = %d, Data = 00x%02X\r", RecvCount, recv_buf[RecvCount-1]);
	}

	return XST_SUCCESS;
}
