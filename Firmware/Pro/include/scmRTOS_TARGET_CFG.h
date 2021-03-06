//******************************************************************************
//*
//*     FULLNAME:  Single-Chip Microcontroller Real-Time Operating System
//*
//*     NICKNAME:  scmRTOS
//*
//*     PROCESSOR: ARM Cortex-M0+
//*
//*     TOOLKIT:   ARM GCC
//*
//*     PURPOSE:   Project Level Configuration
//*
//*     Version: 4.00
//*
//*     $Revision: 561 $
//*     $Date:: 2012-12-06 #$
//*
//*     Copyright (c) 2003-2012, Harry E. Zhurov
//*
//*     Permission is hereby granted, free of charge, to any person
//*     obtaining  a copy of this software and associated documentation
//*     files (the "Software"), to deal in the Software without restriction,
//*     including without limitation the rights to use, copy, modify, merge,
//*     publish, distribute, sublicense, and/or sell copies of the Software,
//*     and to permit persons to whom the Software is furnished to do so,
//*     subject to the following conditions:
//*
//*     The above copyright notice and this permission notice shall be included
//*     in all copies or substantial portions of the Software.
//*
//*     THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//*     EXPRESS  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//*     MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//*     IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
//*     CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
//*     TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
//*     THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//*
//*     =================================================================
//*     See http://scmrtos.sourceforge.net for documentation, latest
//*     information, license and contact details.
//*     =================================================================
//*
//******************************************************************************
//*     GCC STM32L0XX Samples by Anton B. Gusev aka AHTOXA, Copyright (c) 2015

#ifndef  scmRTOS_TARGET_CFG_H
#define  scmRTOS_TARGET_CFG_H

// Define SysTick clock frequency and its interrupt rate in Hz.
#define SYSTICKFREQ     72000000
#define SYSTICKINTRATE  10000

#define delay_us(us) us
#define delay_ms(ms) ms * 10
#define delay_sec(sec) delay_ms(sec * 1000)


// Define number of priority bits implemented in hardware
#define CORE_PRIORITY_BITS  2

// Define some core registers in order to not include specific
// header file for various Cortex processor derivatives.
#define CPU_ICSR            ( ( volatile uint32_t *) 0xE000ED04 )   // Interrupt Control State Register
#define CPU_SYSTICKCSR      ( ( volatile uint32_t *) 0xE000E010 )   // SysTick Control and Status Register
#define CPU_SYSTICKCSR_EINT 0x02                                    // Bit for enable/disable SysTick interrupt

#ifndef __ASSEMBLER__
//------------------------------------------------------------------------------
//
//       System Timer stuff
//
//
namespace OS
{
OS_INTERRUPT void SystemTimer_ISR();
}

#define  LOCK_SYSTEM_TIMER()    ( *CPU_SYSTICKCSR &= ~CPU_SYSTICKCSR_EINT )
#define  UNLOCK_SYSTEM_TIMER()  ( *CPU_SYSTICKCSR |=  CPU_SYSTICKCSR_EINT )

//------------------------------------------------------------------------------
//
//       Context Switch ISR stuff
//
//
namespace OS
{
#if scmRTOS_CONTEXT_SWITCH_SCHEME == 1

    INLINE void raise_context_switch() { *CPU_ICSR = 0x10000000; }

    #define ENABLE_NESTED_INTERRUPTS()
    #define DISABLE_NESTED_INTERRUPTS() TCritSect cs

#else
    #error "Cortex-Mx port supports software interrupt switch method only!"

#endif // scmRTOS_CONTEXT_SWITCH_SCHEME

}
//-----------------------------------------------------------------------------
#endif // __ASSEMBLER__


#endif // scmRTOS_TARGET_CFG_H
//-----------------------------------------------------------------------------

