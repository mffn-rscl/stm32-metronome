.syntax unified
.cpu cortex-m3
.fpu softvfp
.thumb

.global vtable
.global default_interrupt_handler

.type vtable, %object
.section .vector_table,"a",%progbits
vtable:
    // 0-15: Cortex-M3 core exceptions
    .word _estack
    .word reset_handler
    .word NMI_handler
    .word hard_fault_handler
    .word mem_manage_handler
    .word bus_fault_handler
    .word usage_fault_handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word SVC_handler
    .word debug_mon_handler
    .word 0
    .word pending_SV_handler
    .word SysTick_handler
    // 16-31: STM32F103 peripheral interrupts (IRQ0-15)
    .word window_watchdog_IRQ_handler      // IRQ0  - WWDG
    .word PVD_IRQ_handler                  // IRQ1  - PVD
    .word tamper_IRQ_handler               // IRQ2  - TAMPER
    .word RTC_IRQ_handler                  // IRQ3  - RTC
    .word flash_IRQ_handler                // IRQ4  - FLASH
    .word RCC_IRQ_handler                  // IRQ5  - RCC
    .word EXTI0_IRQ_handler                // IRQ6  - EXTI0
    .word EXTI1_IRQ_handler                // IRQ7  - EXTI1
    .word EXTI2_IRQ_handler                // IRQ8  - EXTI2
    .word EXTI3_IRQ_handler                // IRQ9  - EXTI3
    .word EXTI4_IRQ_handler                // IRQ10 - EXTI4
    .word DMA1_chan1_IRQ_handler           // IRQ11 - DMA1_Channel1
    .word DMA1_chan2_IRQ_handler           // IRQ12 - DMA1_Channel2
    .word DMA1_chan3_IRQ_handler           // IRQ13 - DMA1_Channel3
    .word DMA1_chan4_IRQ_handler           // IRQ14 - DMA1_Channel4
    .word DMA1_chan5_IRQ_handler           // IRQ15 - DMA1_Channel5
    // 32-47: (IRQ16-31)
    .word DMA1_chan6_IRQ_handler           // IRQ16 - DMA1_Channel6
    .word DMA1_chan7_IRQ_handler           // IRQ17 - DMA1_Channel7
    .word ADC1_2_IRQ_handler               // IRQ18 - ADC1_2
    .word USB_HP_CAN1_TX_IRQ_handler       // IRQ19 - USB_HP_CAN1_TX
    .word USB_LP_CAN1_RX0_IRQ_handler      // IRQ20 - USB_LP_CAN1_RX0
    .word CAN1_RX1_IRQ_handler             // IRQ21 - CAN1_RX1
    .word CAN1_SCE_IRQ_handler             // IRQ22 - CAN1_SCE
    .word EXTI9_5_IRQ_handler              // IRQ23 - EXTI9_5
    .word TIM1_break_IRQ_handler           // IRQ24 - TIM1_BRK
    .word TIM1_up_IRQ_handler              // IRQ25 - TIM1_UP
    .word TIM1_trg_com_IRQ_handler         // IRQ26 - TIM1_TRG_COM
    .word TIM1_CC_IRQ_handler              // IRQ27 - TIM1_CC
    .word TIM2_IRQ_handler                 // IRQ28 - TIM2
    .word TIM3_IRQ_handler                 // IRQ29 - TIM3
    .word TIM4_IRQ_handler                 // IRQ30 - TIM4
    .word I2C1_EV_IRQ_handler              // IRQ31 - I2C1_EV
    // 48-63: (IRQ32-42, rest reserved for this chip)
    .word I2C1_ER_IRQ_handler              // IRQ32 - I2C1_ER
    .word I2C2_EV_IRQ_handler              // IRQ33 - I2C2_EV
    .word I2C2_ER_IRQ_handler              // IRQ34 - I2C2_ER
    .word SPI1_IRQ_handler                 // IRQ35 - SPI1
    .word SPI2_IRQ_handler                 // IRQ36 - SPI2
    .word USART1_IRQ_handler               // IRQ37 - USART1
    .word USART2_IRQ_handler               // IRQ38 - USART2
    .word USART3_IRQ_handler               // IRQ39 - USART3
    .word EXTI15_10_IRQ_handler            // IRQ40 - EXTI15_10
    .word RTC_alarm_IRQ_handler            // IRQ41 - RTCAlarm
    .word USB_wakeup_IRQ_handler           // IRQ42 - USBWakeup
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0

    /*
     * Setup weak aliases for each exception handler to the
     * default one. These can be updated later, or just
     * overridden since they're weak refs.
     * The reset_handler is set up separately.
     */
    .weak NMI_handler
    .thumb_set NMI_handler,default_interrupt_handler
    .weak hard_fault_handler
    .thumb_set hard_fault_handler,default_interrupt_handler
    .weak mem_manage_handler
    .thumb_set mem_manage_handler,default_interrupt_handler
    .weak bus_fault_handler
    .thumb_set bus_fault_handler,default_interrupt_handler
    .weak usage_fault_handler
    .thumb_set usage_fault_handler,default_interrupt_handler
    .weak SVC_handler
    .thumb_set SVC_handler,default_interrupt_handler
    .weak debug_mon_handler
    .thumb_set debug_mon_handler,default_interrupt_handler
    .weak pending_SV_handler
    .thumb_set pending_SV_handler,default_interrupt_handler
    .weak SysTick_handler
    .thumb_set SysTick_handler,default_interrupt_handler

    .weak window_watchdog_IRQ_handler
    .thumb_set window_watchdog_IRQ_handler,default_interrupt_handler
    .weak PVD_IRQ_handler
    .thumb_set PVD_IRQ_handler,default_interrupt_handler
    .weak tamper_IRQ_handler
    .thumb_set tamper_IRQ_handler,default_interrupt_handler
    .weak RTC_IRQ_handler
    .thumb_set RTC_IRQ_handler,default_interrupt_handler
    .weak flash_IRQ_handler
    .thumb_set flash_IRQ_handler,default_interrupt_handler
    .weak RCC_IRQ_handler
    .thumb_set RCC_IRQ_handler,default_interrupt_handler
    .weak EXTI0_IRQ_handler
    .thumb_set EXTI0_IRQ_handler,default_interrupt_handler
    .weak EXTI1_IRQ_handler
    .thumb_set EXTI1_IRQ_handler,default_interrupt_handler
    .weak EXTI2_IRQ_handler
    .thumb_set EXTI2_IRQ_handler,default_interrupt_handler
    .weak EXTI3_IRQ_handler
    .thumb_set EXTI3_IRQ_handler,default_interrupt_handler
    .weak EXTI4_IRQ_handler
    .thumb_set EXTI4_IRQ_handler,default_interrupt_handler
    .weak DMA1_chan1_IRQ_handler
    .thumb_set DMA1_chan1_IRQ_handler,default_interrupt_handler
    .weak DMA1_chan2_IRQ_handler
    .thumb_set DMA1_chan2_IRQ_handler,default_interrupt_handler
    .weak DMA1_chan3_IRQ_handler
    .thumb_set DMA1_chan3_IRQ_handler,default_interrupt_handler
    .weak DMA1_chan4_IRQ_handler
    .thumb_set DMA1_chan4_IRQ_handler,default_interrupt_handler
    .weak DMA1_chan5_IRQ_handler
    .thumb_set DMA1_chan5_IRQ_handler,default_interrupt_handler
    .weak DMA1_chan6_IRQ_handler
    .thumb_set DMA1_chan6_IRQ_handler,default_interrupt_handler
    .weak DMA1_chan7_IRQ_handler
    .thumb_set DMA1_chan7_IRQ_handler,default_interrupt_handler
    .weak ADC1_2_IRQ_handler
    .thumb_set ADC1_2_IRQ_handler,default_interrupt_handler
    .weak USB_HP_CAN1_TX_IRQ_handler
    .thumb_set USB_HP_CAN1_TX_IRQ_handler,default_interrupt_handler
    .weak USB_LP_CAN1_RX0_IRQ_handler
    .thumb_set USB_LP_CAN1_RX0_IRQ_handler,default_interrupt_handler
    .weak CAN1_RX1_IRQ_handler
    .thumb_set CAN1_RX1_IRQ_handler,default_interrupt_handler
    .weak CAN1_SCE_IRQ_handler
    .thumb_set CAN1_SCE_IRQ_handler,default_interrupt_handler
    .weak EXTI9_5_IRQ_handler
    .thumb_set EXTI9_5_IRQ_handler,default_interrupt_handler
    .weak TIM1_break_IRQ_handler
    .thumb_set TIM1_break_IRQ_handler,default_interrupt_handler
    .weak TIM1_up_IRQ_handler
    .thumb_set TIM1_up_IRQ_handler,default_interrupt_handler
    .weak TIM1_trg_com_IRQ_handler
    .thumb_set TIM1_trg_com_IRQ_handler,default_interrupt_handler
    .weak TIM1_CC_IRQ_handler
    .thumb_set TIM1_CC_IRQ_handler,default_interrupt_handler
    .weak TIM2_IRQ_handler
    .thumb_set TIM2_IRQ_handler,default_interrupt_handler
    .weak TIM3_IRQ_handler
    .thumb_set TIM3_IRQ_handler,default_interrupt_handler
    .weak TIM4_IRQ_handler
    .thumb_set TIM4_IRQ_handler,default_interrupt_handler
    .weak I2C1_EV_IRQ_handler
    .thumb_set I2C1_EV_IRQ_handler,default_interrupt_handler
    .weak I2C1_ER_IRQ_handler
    .thumb_set I2C1_ER_IRQ_handler,default_interrupt_handler
    .weak I2C2_EV_IRQ_handler
    .thumb_set I2C2_EV_IRQ_handler,default_interrupt_handler
    .weak I2C2_ER_IRQ_handler
    .thumb_set I2C2_ER_IRQ_handler,default_interrupt_handler
    .weak SPI1_IRQ_handler
    .thumb_set SPI1_IRQ_handler,default_interrupt_handler
    .weak SPI2_IRQ_handler
    .thumb_set SPI2_IRQ_handler,default_interrupt_handler
    .weak USART1_IRQ_handler
    .thumb_set USART1_IRQ_handler,default_interrupt_handler
    .weak USART2_IRQ_handler
    .thumb_set USART2_IRQ_handler,default_interrupt_handler
    .weak USART3_IRQ_handler
    .thumb_set USART3_IRQ_handler,default_interrupt_handler
    .weak EXTI15_10_IRQ_handler
    .thumb_set EXTI15_10_IRQ_handler,default_interrupt_handler
    .weak RTC_alarm_IRQ_handler
    .thumb_set RTC_alarm_IRQ_handler,default_interrupt_handler
    .weak USB_wakeup_IRQ_handler
    .thumb_set USB_wakeup_IRQ_handler,default_interrupt_handler
.size vtable, .-vtable

/*
 * A 'Default' interrupt handler. This is where interrupts
 * which are not otherwise configured will go.
 * It is an infinite loop, because...well, we weren't
 * expecting the interrupt, so what can we do?
 */
.section .text.default_interrupt_handler,"ax",%progbits
default_interrupt_handler:
    default_interrupt_loop:
      B default_interrupt_loop
.size default_interrupt_handler, .-default_interrupt_handler
