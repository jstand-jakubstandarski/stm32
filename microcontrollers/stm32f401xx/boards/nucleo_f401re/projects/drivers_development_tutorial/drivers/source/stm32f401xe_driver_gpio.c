/*
 * Project: Drivers development tutorial
 * Target MCU: STM32F401XE
 * Author: Jakub Standarski 
 *
 */

#include "stm32f401xe_driver_gpio.h"
#include "stm32f401xe_driver_syscfg.h"

#include "stm32f401xe.h"

#include "nvic_irq.h"



/*****************************************************************************/
/* GPIO CLOCK ENABLE AND DISABLE MACROS */
/*****************************************************************************/

#define GPIOA_CLOCK_ENABLE()  ( RCC->AHB1ENR |= (1 << 0) )
#define GPIOB_CLOCK_ENABLE()  ( RCC->AHB1ENR |= (1 << 1) )
#define GPIOC_CLOCK_ENABLE()  ( RCC->AHB1ENR |= (1 << 2) )
#define GPIOD_CLOCK_ENABLE()  ( RCC->AHB1ENR |= (1 << 3) )
#define GPIOE_CLOCK_ENABLE()  ( RCC->AHB1ENR |= (1 << 4) )
#define GPIOH_CLOCK_ENABLE()  ( RCC->AHB1ENR |= (1 << 7) )



#define GPIOA_CLOCK_DISABLE() ( RCC->AHB1ENR &= ~(1 << 0) )
#define GPIOB_CLOCK_DISABLE() ( RCC->AHB1ENR &= ~(1 << 1) )
#define GPIOC_CLOCK_DISABLE() ( RCC->AHB1ENR &= ~(1 << 2) )
#define GPIOD_CLOCK_DISABLE() ( RCC->AHB1ENR &= ~(1 << 3) )
#define GPIOE_CLOCK_DISABLE() ( RCC->AHB1ENR &= ~(1 << 4) )
#define GPIOH_CLOCK_DISABLE() ( RCC->AHB1ENR &= ~(1 << 7) )



/*****************************************************************************/
/* GPIO PORT RESET MACROS */
/*****************************************************************************/

#define GPIOA_PORT_RESET()  do{ (RCC->AHB1RSTR |= (1 << 0));\
    (RCC->AHB1RSTR &= ~(1 << 0)); }while(0)
#define GPIOB_PORT_RESET()  do{ (RCC->AHB1RSTR |= (1 << 1));\
    (RCC->AHB1RSTR &= ~(1 << 1)); }while(0)
#define GPIOC_PORT_RESET()  do{ (RCC->AHB1RSTR |= (1 << 2));\
    (RCC->AHB1RSTR &= ~(1 << 2)); }while(0)
#define GPIOD_PORT_RESET()  do{ (RCC->AHB1RSTR |= (1 << 3));\
    (RCC->AHB1RSTR &= ~(1 << 3)); }while(0)
#define GPIOE_PORT_RESET()  do{ (RCC->AHB1RSTR |= (1 << 4));\
    (RCC->AHB1RSTR &= ~(1 << 4)); }while(0)
#define GPIOH_PORT_RESET()  do{ (RCC->AHB1RSTR |= (1 << 7));\
    (RCC->AHB1RSTR &= ~(1 << 7)); }while(0)



/*****************************************************************************/
/* GPIO HELPER FUNCTIONS PROTOTYPES */
/*****************************************************************************/

static syscfg_exti_port_code_t gpio_into_port_code_conversion(gpio_registers_t
    *gpio_port);



/*****************************************************************************/
/* GPIO API DEFINITIONS */
/*****************************************************************************/

void gpio_clock_enable(gpio_registers_t *gpio_port)
{
    if(gpio_port == GPIOA) {
        GPIOA_CLOCK_ENABLE();
    } else if(gpio_port == GPIOB) {
        GPIOB_CLOCK_ENABLE();
    } else if(gpio_port == GPIOC) {
        GPIOC_CLOCK_ENABLE();
    } else if(gpio_port == GPIOD) {
        GPIOD_CLOCK_ENABLE();
    } else if(gpio_port == GPIOE) {
        GPIOE_CLOCK_ENABLE();
    } else if(gpio_port == GPIOH) {
        GPIOH_CLOCK_ENABLE();
    } else {
        /* TODO */
    }
}



void gpio_clock_disable(gpio_registers_t *gpio_port)
{
    if(gpio_port == GPIOA) {
        GPIOA_CLOCK_DISABLE();
    } else if(gpio_port == GPIOB) {
        GPIOB_CLOCK_DISABLE();
    } else if(gpio_port == GPIOC) {
        GPIOC_CLOCK_DISABLE();
    } else if(gpio_port == GPIOD) {
        GPIOD_CLOCK_DISABLE();
    } else if(gpio_port == GPIOE) {
        GPIOE_CLOCK_DISABLE();
    } else if(gpio_port == GPIOH) {
        GPIOH_CLOCK_DISABLE();
    } else {
        /* TODO */
    }
}



void gpio_pin_init_config(gpio_handle_t *gpio_handle)
{
    uint32_t settings = 0;

    settings = gpio_handle->gpio_pin_config.pin_mode <<
        (2 * gpio_handle->gpio_pin_config.pin_number);
    gpio_handle->gpio_port->MODER |= settings;

    settings = 0;
    settings = gpio_handle->gpio_pin_config.pin_output_type <<
        gpio_handle->gpio_pin_config.pin_number;
    gpio_handle->gpio_port->OTYPER |= settings;

    settings = 0;
    settings |= gpio_handle->gpio_pin_config.pin_speed <<
        (2* gpio_handle->gpio_pin_config.pin_number);
    gpio_handle->gpio_port->OSPEEDR |= settings;

    settings = 0;
    settings |= gpio_handle->gpio_pin_config.pin_pullup_pulldown_control <<
        (2 * gpio_handle->gpio_pin_config.pin_number);
    gpio_handle->gpio_port->PUPDR |= settings;

    settings = 0;
    if(gpio_handle->gpio_pin_config.pin_mode == gpio_mode_alternate_function) {
        if(gpio_handle->gpio_pin_config.pin_number < gpio_pin_number_8) {
            settings |=
                gpio_handle->gpio_pin_config.pin_alternate_function_mode <<
                    (4 * gpio_handle->gpio_pin_config.pin_number);
            gpio_handle->gpio_port->AFRL |= settings;
        } else {
            uint8_t pin_number = gpio_handle->gpio_pin_config.pin_number % 8;
            settings |= 
                gpio_handle->gpio_pin_config.pin_alternate_function_mode <<
                    (4 * pin_number);
            gpio_handle->gpio_port->AFRH |= settings;
        }
    }
}



void gpio_pin_clear_config(gpio_handle_t *gpio_handle)
{
    gpio_handle->gpio_port->MODER &= ~(0x3 <<
        (2 * gpio_handle->gpio_pin_config.pin_number));

    gpio_handle->gpio_port->OTYPER &= ~(0x1 <<
        gpio_handle->gpio_pin_config.pin_number);

    gpio_handle->gpio_port->OSPEEDR &= ~(0x3 <<
        (2 * gpio_handle->gpio_pin_config.pin_number));

    gpio_handle->gpio_port->PUPDR &= ~(0x3 <<
        (2 * gpio_handle->gpio_pin_config.pin_number));

    if(gpio_handle->gpio_pin_config.pin_number < gpio_pin_number_8) {
        gpio_handle->gpio_port->AFRL &= ~(0xF << 
            (4 * gpio_handle->gpio_pin_config.pin_number));
    } else {
        uint8_t pin_number = gpio_handle->gpio_pin_config.pin_number % 8;
        gpio_handle->gpio_port->AFRH &= ~(0xF << (4 * pin_number));
    }
}



void gpio_port_reset(gpio_registers_t *gpio_port)
{
    if(gpio_port == GPIOA) {
        GPIOA_PORT_RESET();
    } else if(gpio_port == GPIOB) {
        GPIOB_PORT_RESET();
    } else if(gpio_port == GPIOC) {
        GPIOC_PORT_RESET();
    } else if(gpio_port == GPIOD) {
        GPIOD_PORT_RESET();
    } else if(gpio_port == GPIOE) {
        GPIOE_PORT_RESET();
    } else if(gpio_port == GPIOH) {
        GPIOH_PORT_RESET();
    } else {
        /* TODO */
    }
}    



uint8_t gpio_pin_read(gpio_registers_t *gpio_port,
    gpio_pin_number_t pin_number)
{
   return (uint8_t)((gpio_port->IDR >> pin_number) & 0x1);
}



uint16_t gpio_port_read(gpio_registers_t *gpio_port)
{
    return (uint16_t)(gpio_port->IDR);
}



void gpio_pin_set(gpio_registers_t *gpio_port, gpio_pin_number_t pin_number)
{
    gpio_port->ODR |= (1 << pin_number); 
}



void gpio_pin_reset(gpio_registers_t *gpio_port, gpio_pin_number_t pin_number)
{
    gpio_port->ODR &= ~(1 << pin_number);
}



void gpio_pin_toggle(gpio_registers_t *gpio_port,
    gpio_pin_number_t pin_number)
{
    gpio_port->ODR ^= (1 << pin_number);
}



void gpio_pin_irq_config(gpio_handle_t *gpio_handle)
{
    if(gpio_handle->gpio_irq_config.trigger_selection == gpio_trigger_rising) {
        EXTI->RTSR |= (1 << gpio_handle->gpio_pin_config.pin_number);
    } else if(gpio_handle->gpio_irq_config.trigger_selection == 
        gpio_trigger_falling) {
        EXTI->FTSR |= (1 << gpio_handle->gpio_pin_config.pin_number);
    } else if(gpio_handle->gpio_irq_config.trigger_selection ==
        gpio_trigger_rising_falling) {
        EXTI->RTSR |= (1 << gpio_handle->gpio_pin_config.pin_number);
        EXTI->FTSR |= (1 << gpio_handle->gpio_pin_config.pin_number);
    }

    uint8_t exti_cr_number = gpio_handle->gpio_pin_config.pin_number / 4;
    uint8_t exti_number = gpio_handle->gpio_pin_config.pin_number % 4;
    syscfg_exti_port_code_t gpio_port_code = gpio_into_port_code_conversion(
        gpio_handle->gpio_port);
    if(exti_cr_number == 0) {
        SYSCFG->EXTICR1 |= (gpio_port_code << (exti_number * 4));
    } else if(exti_cr_number == 1) {
        SYSCFG->EXTICR2 |= (gpio_port_code << (exti_number * 4));
    } else if(exti_cr_number == 2) {
        SYSCFG->EXTICR3 |= (gpio_port_code << (exti_number * 4));
    } else {
        SYSCFG->EXTICR4 |= (gpio_port_code << (exti_number * 4));
    }

    EXTI->IMR |= (1 << gpio_handle->gpio_pin_config.pin_number);
}



void gpio_pin_irq_handler(gpio_pin_number_t pin_number)
{
    if(EXTI->PR & (1 << pin_number)) {
        EXTI->PR |= (1 << pin_number);
    }
}



/*****************************************************************************/
/* GPIO HELPER FUNCTIONS DEFINITIONS */
/*****************************************************************************/

static syscfg_exti_port_code_t gpio_into_port_code_conversion(gpio_registers_t
    *gpio_port)
{
    if(gpio_port == GPIOA) {
        return syscfg_exti_port_code_gpio_a; 
    } else if(gpio_port == GPIOB) {
        return syscfg_exti_port_code_gpio_b;
    } else if(gpio_port == GPIOC) {
        return syscfg_exti_port_code_gpio_c;
    } else if(gpio_port == GPIOD) {
        return syscfg_exti_port_code_gpio_d;
    } else if(gpio_port == GPIOE) {
        return syscfg_exti_port_code_gpio_e;
    } else {
        return syscfg_exti_port_code_gpio_h;
    }
}

