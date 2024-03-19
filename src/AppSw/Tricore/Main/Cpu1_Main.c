/**
 * \file Cpu1_Main.c
 * \brief CPU1 functions.
 *
 * \version iLLD_Demos_1_0_1_12_0
 * \copyright Copyright (c) 2014 Infineon Technologies AG. All rights reserved.
 *
 *
 *                                 IMPORTANT NOTICE
 *
 * Use of this file is subject to the terms of use agreed between (i) you or
 * the company in which ordinary course of business you are acting and (ii)
 * Infineon Technologies AG or its licensees. If and as long as no such terms
 * of use are agreed, use of this file is subject to following:
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer, must
 * be included in all copies of the Software, in whole or in part, and all
 * derivative works of the Software, unless such copies or derivative works are
 * solely in the form of machine-executable object code generated by a source
 * language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */


/*
	UART via ASCLIN module (manual, p.1647)		(could also use 'PSI5-S' for UART ?)

	ASCLIN_UART.c
	ASCLIN_UART.h
 */

#include "IfxCpu.h"			// IfxCpu_syncEvent
#include "IfxScuWdt.h" 		// IfxScuWdt_*
#include "IfxAsclin_asc.h"	// IfxAsclin_Asc_Config
#include "Ifx_Console.h"	// Ifx_Console_init
#include "IfxCpu_Irq.h"		// IfxCpu_Irq_installInterruptHandler
//#include "Bsp.h"			// waitTime

#include "ASCLIN_UART.h"	// LOG_DBG

#define WAIT_TIME_MS	1000


static inline Ifx_TickTime now(void)
{
    boolean interruptState = IfxCpu_disableInterrupts();
    Ifx_TickTime stmNow = (Ifx_TickTime) IfxStm_get(&MODULE_STM1) & TIME_INFINITE;
    IfxCpu_restoreInterrupts(interruptState);
    return stmNow;
}
static inline boolean isDeadLine(Ifx_TickTime deadLine)
{
	return (boolean) ((deadLine == TIME_INFINITE) ? FALSE : (now() >= deadLine));
}
static void wait(void)
{
	Ifx_TickTime timeout = IfxStm_getTicksFromMilliseconds(&MODULE_STM1, WAIT_TIME_MS);
	Ifx_TickTime deadLine = (timeout == TIME_INFINITE) ? TIME_INFINITE : (now() + timeout);
	while (isDeadLine(deadLine) == FALSE)
	{}
}


extern IfxCpu_syncEvent g_cpuSyncEvent;

//#include <stdio.h>

void core1_main(void)
{
    /**
     * !!WATCHDOG1 IS DISABLED HERE!!
     * Enable the watchdog and service it periodically if required
	 */
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());

    /* Enable the global interrupts of this CPU */
    IfxCpu_enableInterrupts();


	/* init UART */
	init_uart();

	/* Wait for CPU sync event */
	IfxCpu_emitEvent(&g_cpuSyncEvent);
	IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

	//LOG_DBG("hi from Core %d after 'init_uart()' \n", IfxCpu_getCoreId());

	uint64 i = 0;
	while (1) {
		// print sth. to console every second
		//IfxStdIf_DPipe_print(io, "=== [%3d] print from Core 1 ===" ENDL, i);
		//Ifx_Console_print("[%3d] print from Core 1 ===" ENDL, i);

		/*
		wait();
		++i;
		LOG_DBG("Core [%d] in loop", IfxCpu_getCoreId());
		*/

		//printf("printf from core [%d] in loop", IfxCpu_getCoreId());
    }
}
