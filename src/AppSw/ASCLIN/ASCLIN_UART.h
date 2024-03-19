#ifndef ASCLIN_UART_H_
#define ASCLIN_UART_H_


/** from ShielBuddy User Manual, table 4 - "ASCLIN to ShieldBuddy connector mapping":


	TC275 Port Pin    ASCLIN    Board Marking    ShieldBuddy Connector Name    Comment
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	P15.0             ASC1      TX0 14           COMMUNICATION.1 (J403.1)      Serial0     // ERROR in manual: COMMUNICATION.8
	P15.1             ASC1      RX0 15           COMMUNICATION.2 (J403.2)                  // ERROR in manual: COMMUNICATION.7
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	P33.9             ASC2      TX1 16           COMMUNICATION.3 (J403.3)      Serial1     // ERROR in manual: COMMUNICATION.6
	P33.8             ASC2      RX1 17           COMMUNICATION.4 (J403.4)                  // ERROR in manual: COMMUNICATION.5
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	P20.0             ASC3      TX2 18           COMMUNICATION.5 (J403.5)      Serial2     // ERROR in manual: COMMUNICATION.4
	P20.3             ASC3      RX2 19           COMMUNICATION.6 (J403.6)                  // ERROR in manual: COMMUNICATION.3
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	P15.2             ASC0      TX   1           PWML.2 (J402.2)               Serial
	P15.3             ASC0      RX   0           PWML.1 (J402.1)
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	P15.7             ASC3      TX                                             SerialASC - Available via USB (FTDI-USB)
	P32.2             ASC3      RX
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	=> ShieldBuddy USB is connected to 'MODULE_ASCLIN3'
	Q: can I 'sniff' USB communication of ASC on 18/19 ?


	pin-mapping for UART function:

		ASC0
			IfxAsclin0_RXB_P15_3_IN		// Serial
			IfxAsclin0_TX_P15_2_OUT
		ASC1
			IfxAsclin1_RXA_P15_1_IN		// Serial0
			IfxAsclin1_TX_P15_0_OUT
		ASC2
			IfxAsclin2_RXE_P33_8_IN		// Serial1
			IfxAsclin2_TX_P33_9_OUT
		ASC3
			IfxAsclin3_RXD_P32_2_IN		// SerialASC
			IfxAsclin3_TX_P15_7_OUT

			IfxAsclin3_RXC_P20_3_IN		// Serial2
			IfxAsclin3_TX_P20_3_OUT


	Q: is using printf() == simulated IO ? It shows that I need 'vio_virtio.c' ?!?

 */

#include <IfxAsclin_PinMap.h>	// IfxAsclin_Rx_In, IfxAsclin_Tx_Out
#include <Ifx_Console.h>		// Ifx_Console_print()


typedef struct {
	const IfxAsclin_Rx_In* const rx;
	const IfxAsclin_Tx_Out* const tx;
} Asclin_Uart_Config;


/** init UART */
void init_uart();

void flush_uart();

/** spinlock as timed mutex to enable multi-core use of LOG_DBG */
extern IfxCpu_spinLock uart_spinLock;

/**
 * use LOG_DBG() as printf()
 * fmt is the printf-style format-string
 * #define LOG_DBG(...) Ifx_Console_print(__VA_ARGS__);
 */

/**
 * debugging logs on/off
 *
 * if LOG_DBG() is used on multiple cores 	=> #define MK_DEBUG_MULTI_CORE
 * if LOG_DBG() is used on a single core 	=> #define MK_DEBUG_SINGLE_CORE
 * if you want strip LOG_DBG() from file	=> define neither MK_DEBUG_MULTI_CORE nor MK_DEBUG_SINGLE_CORE
 */
#define MK_DEBUG_SINGLE_CORE

/**
 * fmt is the printf-style format-string
 */
//#define CONSOLE_PRINT(fmt, ...)		Ifx_Console_print("[%s:%d] " fmt ENDL, __FILE__, __LINE__, ##__VA_ARGS__)
#define CONSOLE_PRINT(fmt, ...)		Ifx_Console_print("| %s:%d,%s() | " fmt ENDL, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
									//flush_uart()


#if defined(MK_DEBUG_SINGLE_CORE)
	#define LOG_DBG(fmt, ...)	CONSOLE_PRINT(fmt, ##__VA_ARGS__);
#elif defined(MK_DEBUG_MULTI_CORE)
	#define LOG_DBG(fmt, ...) 															\
		do { 																			\
			const uint32 timeoutCnt = 0xffff;											\
			boolean gotSpinLock = IfxCpu_setSpinLock(&uart_spinLock, timeoutCnt);		\
			if (gotSpinLock) {															\
				/* critical section: the printing */									\
				CONSOLE_PRINT(fmt, ##__VA_ARGS__);										\
				/* release spinlock */													\
				IfxCpu_resetSpinLock(&uart_spinLock);									\
			} else {																	\
				/* TODO: didn't get spinlock after timeout */							\
			}																			\
		} while (0)
#else
	#define LOG_DBG(fmt, ...)  /* empty */
#endif


#endif /* ASCLIN_UART_H_ */
