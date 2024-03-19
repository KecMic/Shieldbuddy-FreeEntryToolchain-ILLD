#include "IfxAsclin_asc.h"	// IfxAsclin_Asc_Config
#include "Ifx_Console.h"	// Ifx_Console_init
#include "IfxCpu_Irq.h"		// IfxCpu_Irq_installInterruptHandler

#include "global.h"			// ARRAY_NUM_ELEMS
#include "ASCLIN_UART.h"	// LOG_DBG

#define ASC_BAUDRATE				115200	// TODO: try higher baudrate and lower 'IfxAsclin_OversamplingFactor_16'
											// baudrates (see device manager, COM4)
											// 11520, 460800, 921600
											// see 'IfxAsclin_Asc_initModuleConfig()' for initial parity settings
											// check also: IfxAsclin_TxFifoInletWidth_1, IfxAsclin_RxFifoOutletWidth_1
											// TODO: set TOS to DMA!
											// careful: printf-msg-strings need to be smaller than 'STDIF_DPIPE_MAX_PRINT_SIZE' !!!
#define ISR_PRIORITY_ASCLIN_TX       	 8
#define ISR_PRIORITY_ASCLIN_RX		 	 4
#define ISR_PRIORITY_ASCLIN_ER			12
#define ASC_TX_BUFFER_SIZE         	   256
#define ASC_RX_BUFFER_SIZE             256

/******************************************************************************
 * config of the 4 UARTs (see h-file)
 *****************************************************************************/
const Asclin_Uart_Config Asc_0 = {
		.rx = &IfxAsclin0_RXB_P15_3_IN,
		.tx = &IfxAsclin0_TX_P15_2_OUT
};
const Asclin_Uart_Config Asc_1 = { // use '&MODULE_ASCLIN1'
		.rx = &IfxAsclin1_RXA_P15_1_IN,
		.tx = &IfxAsclin1_TX_P15_0_OUT
};
const Asclin_Uart_Config Asc_2 = { // use '&MODULE_ASCLIN2'
		.rx = &IfxAsclin2_RXE_P33_8_IN,
		.tx = &IfxAsclin2_TX_P33_9_OUT
};
const Asclin_Uart_Config Asc_3_Usb = { // use '&MODULE_ASCLIN3'
		.rx = &IfxAsclin3_RXD_P32_2_IN,
		.tx = &IfxAsclin3_TX_P15_7_OUT
};
const Asclin_Uart_Config Asc_3 = { // use '&MODULE_ASCLIN3'
		.rx = &IfxAsclin3_RXC_P20_3_IN,
		.tx = &IfxAsclin3_TX_P20_0_OUT//&IfxAsclin3_TX_P20_3_OUT
};


const Asclin_Uart_Config* const uart_configs[] = {
		&Asc_3_Usb,
		&Asc_3
};
#define UART_NUM_CONFIGS	ARRAY_NUM_ELEMS(uart_configs)

//----------------------------------------------------------------------------------------
// set one of the 4 UARTS (Asc_3 is connected to FTDI-USB and 2 pins at the same time?)
#define ACTIVE_ASC		Asc_3_Usb//Asc_3_Usb
//----------------------------------------------------------------------------------------

// use these 3 macros in the code below
#define ASCLIN_TX_PIN	ACTIVE_ASC.tx
#define ASCLIN_RX_PIN	ACTIVE_ASC.rx
#define ASCLIN_MODULE	ACTIVE_ASC.tx->module

// spinlock
IfxCpu_spinLock uart_spinLock;


/******************************************************************************
 * handles
 *****************************************************************************/
static IfxStdIf_DPipe ascStandardInterface;		/** standard interface handle */
static IfxAsclin_Asc  asclin;					/** ASC module handle */

/******************************************************************************
 * buffers for TX and RX FIFOs
 *****************************************************************************/
// see 'Ifx_Fifo': needs to be aligned to 64 bit
// see also 'Ifx_AlignOn32', 'Ifx_Fifo_init'
static uint8 uartTxBuffer[ASC_TX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8] __attribute__ ((aligned(8)));// __attribute__ ((aligned(64)));
static uint8 uartRxBuffer[ASC_RX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8] __attribute__ ((aligned(8)));// __attribute__ ((aligned(64)));

/******************************************************************************
 * define interrupt service routines
 *****************************************************************************/
IFX_INTERRUPT(asc0TxISR,  0, ISR_PRIORITY_ASCLIN_TX) { IfxStdIf_DPipe_onTransmit(&ascStandardInterface); }
IFX_INTERRUPT(asc0RxISR,  0, ISR_PRIORITY_ASCLIN_RX) { IfxStdIf_DPipe_onReceive(&ascStandardInterface); }
IFX_INTERRUPT(asc0ErrISR, 0, ISR_PRIORITY_ASCLIN_ER) { IfxStdIf_DPipe_onError(&ascStandardInterface); }

/** installs ISRs */
static inline void install_ISRs()
{
	// Q: use 'interruptHandlerInstall' or 'IfxCpu_Irq_installInterruptHandler'
	#ifdef IFX_USE_SW_MANAGED_INT
	IfxCpu_Irq_installInterruptHandler(&asc0TxISR,  ISR_PRIORITY_ASCLIN_TX);
	IfxCpu_Irq_installInterruptHandler(&asc0RxISR,  ISR_PRIORITY_ASCLIN_RX);
	IfxCpu_Irq_installInterruptHandler(&asc0ErrISR, ISR_PRIORITY_ASCLIN_ER);
	#endif
}

/******************************************************************************
 * functions
 *****************************************************************************/

void init_uart()
{
	/** init module config */
	IfxAsclin_Asc_Config ascConf;
	IfxAsclin_Asc_initModuleConfig(&ascConf, ASCLIN_MODULE); //&MODULE_ASCLIN2);

	// TODO:
	/** */

	/** baudrate */
	ascConf.baudrate.baudrate     = ASC_BAUDRATE;
	ascConf.baudrate.oversampling = IfxAsclin_OversamplingFactor_16;

	/** sampling mode */
	ascConf.bitTiming.medianFilter        = IfxAsclin_SamplesPerBit_three;
	ascConf.bitTiming.samplePointPosition = IfxAsclin_SamplePointPosition_8;

	/** ISR prios and interrupt target */
	ascConf.interrupt.txPriority = ISR_PRIORITY_ASCLIN_TX;
	ascConf.interrupt.rxPriority = ISR_PRIORITY_ASCLIN_RX;
	ascConf.interrupt.erPriority = ISR_PRIORITY_ASCLIN_ER;
	ascConf.interrupt.typeOfService = IfxSrc_Tos_cpu1;

	/** pin config */
	const IfxAsclin_Asc_Pins pins = {
			.cts        = NULL_PTR,						// CTS pin not used
			.ctsMode    = IfxPort_InputMode_pullUp,
			.rx         = ASCLIN_RX_PIN,				// RX pin
			.rxMode     = IfxPort_InputMode_pullUp,
			.rts        = NULL_PTR,						// RTS pin not used
			.rtsMode    = IfxPort_OutputMode_pushPull,
			.tx         = ASCLIN_TX_PIN,				// TX pin
			.txMode     = IfxPort_OutputMode_pushPull,
			.pinDriver  = IfxPort_PadDriver_cmosAutomotiveSpeed1
	};
	ascConf.pins = &pins;

	/** FIFO buffers config */
	ascConf.txBuffer     = uartTxBuffer;
	ascConf.txBufferSize = ASC_TX_BUFFER_SIZE;
	ascConf.rxBuffer     = uartRxBuffer;
	ascConf.rxBufferSize = ASC_RX_BUFFER_SIZE;

	/** init ASCLIN module */
	IfxAsclin_Asc_initModule(&asclin, &ascConf);

	/** install isrs */
	install_ISRs();

	// TODO: put this in other function ?!
	// init standard interface
	IfxAsclin_Asc_stdIfDPipeInit(&ascStandardInterface, &asclin);

	// init console, so we can use 'Ifx_Console_print()' as printf
	// after that, you could also use 'Ifx_Console_print("[%3d] print from Core x ===" ENDL, i)'
	Ifx_Console_init(&ascStandardInterface);

	// FINISHED INIT, can print now
	LOG_DBG(ENDL "hi from Core %d (Ifx_Console_print) ===", IfxCpu_getCoreId());
	LOG_DBG(ENDL "hi from Core %d (Ifx_Console_print) ===", IfxCpu_getCoreId());
	LOG_DBG("used baudrate: %d", ASC_BAUDRATE);
	LOG_DBG("[test 1]");
	LOG_DBG("[test 2]");
	LOG_DBG("[test 3]");
	LOG_DBG("[test 4]");
	LOG_DBG("[testing longer multi-]" ENDL "line print");
}

void flush_uart()
{
	//IfxAsclin_Asc_flushTx(&MODULE_ASCLIN3, TIME_INFINITE);
	IfxStdIf_DPipe_flushTx(&ascStandardInterface, TIME_INFINITE);
}
