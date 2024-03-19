/**
 * \file Cpu0_Main.c
 * \brief System initialisation and main program implementation.
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
#include "IfxCpu.h"			// IfxCpu_syncEvent
#include "IfxScuWdt.h" 		// IfxScuWdt_*
#include "Ifx_Types.h"		// uint32, ...
#include "IfxPort.h" 		// IfxPort_Pin_Config, IfxPort_setPinModeOutput
#include "Bsp.h"			// BSP_DEFAULT_TIMER
#include "IfxLldVersion.h"	// illd-version

#include "ASCLIN_UART.h"	// LOG_DBG

#include "IfxGtm_Cmu.h"		// GTM-related
#include "IfxGtm.h"			// GTM-related

#include <inttypes.h>	// PRIu64

//#include <stdio.h>	// sprintf

//#define LED				&MODULE_P10, 2 /* LED definition: Port, Pin */
#define WAIT_TIME_MS	500

const IfxPort_Pin_Config led = {
		.port = &MODULE_P10,
		.pinIndex = 2,
		.mode = IfxPort_OutputMode_pushPull,
		.padDriver = IfxPort_PadDriver_cmosAutomotiveSpeed1
};


IfxCpu_syncEvent g_cpuSyncEvent = 0;


int core0_main(void)
{
    /**
     * !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
     * Enable the watchdogs and service them periodically if required
	 */
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    /* Enable the global interrupts of this CPU */
    IfxCpu_enableInterrupts();

    /* Wait for CPU sync event */
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 5000);

    // init LED on ShieldBuddy
    IfxPort_setPinModeOutput(led.port, led.pinIndex, led.mode, IfxPort_OutputIdx_general);
    IfxPort_setPinPadDriver(led.port, led.pinIndex, led.padDriver); // do we need this ?
    IfxPort_setPinLow(led.port, led.pinIndex); // switch off LED (active high)



    uint64 timer_us;
	uint64 timer_us_end;
	uint64 timer_ms;
	sint64 stmNow;
	Ifx_STM* stm = &MODULE_STM0;
	const float32 stm_freq = IfxStm_getFrequency(stm);

	stmNow = IfxStm_now();
	timer_us = (IfxStm_get(stm) * 1000000) / stm_freq;
	timer_ms = (IfxStm_get(stm) * 1000) / stm_freq;

	uint32 microSeconds = 1000;
	uint32 milliSeconds = 1000;
	volatile sint32 us_from_ticks = IfxStm_getTicksFromMicroseconds(stm, microSeconds);
	volatile sint32 ms_from_ticks = IfxStm_getTicksFromMilliseconds(stm, milliSeconds);

	// to print float, use sprintf (needs stdio.h) -- sprintf not working
	char floatStr[64] = {0};
	//sprintf(floatStr, "%.2f", stm_freq);
	/*for (uint32 i = 1; i < ARRAY_NUM_ELEMS(floatStr); i++) {
		floatStr[i] = i;
	}
	floatStr[63] = '\0';
	*/
	floatStr[0] = 'A';
	floatStr[1] = 'B';
	floatStr[2] = 'a';
	floatStr[3] = 'b';
	char* testStr = "4.2";
	LOG_DBG("testStr: %s", testStr);
	LOG_DBG("floatStr: %s", floatStr); // doesn't work! not even with #include <stdio.h>
	LOG_DBG("test char: %c", floatStr[0]);

	LOG_DBG("illd-version: %d.%d.%d.%d.%d", IFX_LLD_VERSION_GENERATION, IFX_LLD_VERSION_MAJOR, IFX_LLD_VERSION_MAJOR_UPDATE, IFX_LLD_VERSION_MINOR, IFX_LLD_VERSION_REVISION);


	//LOG_DBG("stm_freq:  %f", stm_freq); // doesn't work! not even with #include <stdio.h>
	//LOG_DBG("stm_freq:  %lld.%lld", (sint64)stm_freq, (sint64)(stm_freq*1000));	// 100000000.0	<<<< %f not supported ?!?
	LOG_DBG("stm_freq:  %lld.%lld", (sint64)stm_freq, ((sint64)(stm_freq*1000)) % 1000); 					// 100000000.952
	LOG_DBG("stm_freq:  %lld.%lld", (sint64)stm_freq, ((sint64)((double)stm_freq*1000)) % 1000); 			// 100000000.952
	LOG_DBG("stm_freq:  %lld.%lld", (sint64)stm_freq, ((sint64)(stm_freq*1000000000)) % 1000000000);		// 100000000.430674944
	LOG_DBG("stm_freq:  %lld.%lld", (sint64)stm_freq, ((sint64)((double)stm_freq*000000000)) % 1000000000);	// 100000000.430674944
	LOG_DBG("stm_freq:  %lld (.xxx) Hz", (sint64)stm_freq); // 100000000 == 100 MHz == 10ns/tick
	// for stdio.h, checkout C:\HighTec\toolchains\tricore\v4.9.3.0-infineon-1.0\tricore\include
	LOG_DBG("timer_us:  %lld", timer_us);
	LOG_DBG("timer_ms:  %lld", timer_ms);
	LOG_DBG("%d us in stm-ticks :: us_from_ticks:  %d", microSeconds, us_from_ticks);	// 1000 us == 100000 ticks 		=> 1us == 100 ticks => 10ns == 1 tick
	LOG_DBG("%d ms in stm-ticks :: ms_from_ticks:  %d", milliSeconds, ms_from_ticks);	// 1000 ms == 100000000 ticks	=> 1ms == 100000 ticks


	volatile uint64 timeout = 0;
	volatile boolean flag = FALSE;
#define N_MEASUREMENTS	100
	uint64 diffs[N_MEASUREMENTS]= {0};

	LOG_DBG("Core [%d] starting %d measurements...", IfxCpu_getCoreId(), N_MEASUREMENTS);

	for (int i = 0; i < N_MEASUREMENTS; ++i) {
		timeout = 0;
		flag = FALSE;

		timer_us = (IfxStm_get(stm) * 1000000) / stm_freq;
		while (1) {
			if (timeout >= 2000000) {
				flag = TRUE;
				break;
			}
			timeout++;
		}
		timer_us_end = (IfxStm_get(stm) * 1000000) / stm_freq;
		uint64 diff = timer_us_end - timer_us;
		if (timeout >= 2000000) {

		}
		diffs[i] = diff;
	}
	for (int i = 0; i < N_MEASUREMENTS; ++i) {
		//LOG_DBG("diffs[%d] = %lld us (to count up to 200000)", i, diffs[i]);	// 15ms -- 15000 us
		LOG_DBG("diffs[%d] = %lld us (to count up to 2000000)", i, diffs[i]);	// 150ms -- 150000 us
		/*Ifx_Console_print("%d: %d %u %lu %llu %lld %lli %llx %lx" ENDL, i,
				diffs[i], diffs[i], diffs[i], diffs[i], diffs[i], diffs[i], diffs[i], diffs[i]);*/
		/*
			only working:
				%d, %lu
		 	llu, lld, lli output:
				for 150010 <-> 644288044072960 == 0x249fa00000000
				for 150011 <-> 644292339040256 == 0x249fb00000000
				for 150012 <-> 644296634007552 == 0x249fc00000000
			u, lx output:
				0

			we have that hex(150010) == 0x249fa
		 */

		// 0x249fb == 150011	<<< wrong endianness used !!! prints 0x249fb00000000
	}
	/*
	{
		uint8  u8  = 0x12; // == 18
		sint8  s8  = 0x12; // == 18
		uint16 u16 = 0x1234; // == 4660
		sint16 s16 = 0x1234; // == 4660
		uint32 u32 = 0x12345678; // == 305419896
		sint32 s32 = 0x12345678; // == 305419896
		uint64 u64 = 0x12345678abcdef09; // == 1311768467750121225
		sint64 s64 = 0x12345678abcdef09; // == 1311768467750121225
		Ifx_Console_print("u8:  %d %ld %lld %u %lu %llu %i %li %lli %x %lx %llx" ENDL, u8, u8, u8, u8, u8, u8, u8, u8, u8, u8, u8, u8);
		Ifx_Console_print("s8:  %d %ld %lld %u %lu %llu %i %li %lli %x %lx %llx" ENDL, s8, s8, s8, s8, s8, s8, s8, s8, s8, s8, s8, s8);
		Ifx_Console_print("u16: %d %ld %lld %u %lu %llu %i %li %lli %x %lx %llx" ENDL, u16, u16, u16, u16, u16, u16, u16, u16, u16, u16, u16, u16);
		Ifx_Console_print("s16: %d %ld %lld %u %lu %llu %i %li %lli %x %lx %llx" ENDL, s16, s16, s16, s16, s16, s16, s16, s16, s16, s16, s16, s16);
		Ifx_Console_print("u32: %d %ld %lld %u %lu %llu %i %li %lli %x %lx %llx" ENDL, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
		Ifx_Console_print("s32: %d %ld %lld %u %lu %llu %i %li %lli %x %lx %llx" ENDL, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
		Ifx_Console_print("u64: %d %ld %lld %u %lu %llu %i %li %lli %x %lx %llx" ENDL, u64, u64, u64, u64, u64, u64, u64, u64, u64, u64, u64, u64);
		Ifx_Console_print("s64: %d %ld %lld %u %lu %llu %i %li %lli %x %lx %llx" ENDL, s64, s64, s64, s64, s64, s64, s64, s64, s64, s64, s64, s64);
	}
	*/
	/*
	{
		uint8  u8  = -0x12; // == -18
		sint8  s8  = -0x12; // == -18
		uint16 u16 = -0x1234; // == -4660
		sint16 s16 = -0x1234; // == -4660
		uint32 u32 = -0x12345678; // == -305419896
		sint32 s32 = -0x12345678; // == -305419896
		uint64 u64 = -0x12345678abcdef09; // == -1311768467750121225
		sint64 s64 = -0x12345678abcdef09; // == -1311768467750121225
		Ifx_Console_print("u8:  %d %ld %lld %u %lu %llu %i %li %lli %x %lx %llx" ENDL, u8, u8, u8, u8, u8, u8, u8, u8, u8, u8, u8, u8);
		Ifx_Console_print("s8:  %d %ld %lld %u %lu %llu %i %li %lli %x %lx %llx" ENDL, s8, s8, s8, s8, s8, s8, s8, s8, s8, s8, s8, s8);
		Ifx_Console_print("u16: %d %ld %lld %u %lu %llu %i %li %lli %x %lx %llx" ENDL, u16, u16, u16, u16, u16, u16, u16, u16, u16, u16, u16, u16);
		Ifx_Console_print("s16: %d %ld %lld %u %lu %llu %i %li %lli %x %lx %llx" ENDL, s16, s16, s16, s16, s16, s16, s16, s16, s16, s16, s16, s16);
		Ifx_Console_print("u32: %d %ld %lld %u %lu %llu %i %li %lli %x %lx %llx" ENDL, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
		Ifx_Console_print("s32: %d %ld %lld %u %lu %llu %i %li %lli %x %lx %llx" ENDL, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
		Ifx_Console_print("u64: %d %ld %lld %u %lu %llu %i %li %lli %x %lx %llx" ENDL, u64, u64, u64, u64, u64, u64, u64, u64, u64, u64, u64, u64);
		Ifx_Console_print("s64: %d %ld %lld %u %lu %llu %i %li %lli %x %lx %llx" ENDL, s64, s64, s64, s64, s64, s64, s64, s64, s64, s64, s64, s64);

		Ifx_Console_print(ENDL);
		Ifx_Console_print("u8:  %d %d %d %d %d %d %d %d %d %d %d %d" ENDL, u8, u8, u8, u8, u8, u8, u8, u8, u8, u8, u8, u8);
		Ifx_Console_print("s8:  %d %d %d %d %d %d %d %d %d %d %d %d" ENDL, s8, s8, s8, s8, s8, s8, s8, s8, s8, s8, s8, s8);
		Ifx_Console_print("u16: %d %d %d %d %d %d %d %d %d %d %d %d" ENDL, u16, u16, u16, u16, u16, u16, u16, u16, u16, u16, u16, u16);
		Ifx_Console_print("s16: %d %d %d %d %d %d %d %d %d %d %d %d" ENDL, s16, s16, s16, s16, s16, s16, s16, s16, s16, s16, s16, s16);
		Ifx_Console_print("u32: %d %d %d %d %d %d %d %d %d %d %d %d" ENDL, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
		Ifx_Console_print("s32: %d %d %d %d %d %d %d %d %d %d %d %d" ENDL, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
		Ifx_Console_print("u64: %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld" ENDL, u64, u64, u64, u64, u64, u64, u64, u64, u64, u64, u64, u64);
		Ifx_Console_print("s64: %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld" ENDL, s64, s64, s64, s64, s64, s64, s64, s64, s64, s64, s64, s64);
	}
	*/
	//flush_uart();
	waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, 2000 /*ms*/));
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


	const sint32 wait_time_ticks = IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME_MS);
	timer_ms = (IfxStm_get(stm) * 1000) / stm_freq;

	while (1) {
    	// blink LED
    	// TODO: do this with STM-timer triggering interrupt every 10ms or so
    	IfxPort_togglePin(led.port, led.pinIndex);
    	waitTime(wait_time_ticks);
    	timer_ms = (IfxStm_get(stm) * 1000) / stm_freq;
    	LOG_DBG("core %d, %lld (%llds %lldms) in loop", IfxCpu_getCoreId(), timer_ms, timer_ms/1000, timer_ms%1000);
    	//LOG_DBG("[core %d, %llu.%llu s] in loop", IfxCpu_getCoreId(), timer_ms / 1000, timer_ms % 1000);
    }
}
