// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2026 John McManigle
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "common-hal/busio/SPI.h"
#include "common-hal/digitalio/DigitalInOut.h"

#define CIRCUITPY_USB_MAX3421_INSTANCE 2

#define MAX3421E_REG_IOPINS1  ((uint8_t)20) // = 0x14, SPI register for low-side inputs (high byte) and outputs (low byte)
#define MAX3421E_REG_IOPINS2  ((uint8_t)21) // = 0x15, SPI register for high-side inputs (high byte) and outputs (low byte)

#define MAX3421E_CMD_READ  (0u << 1)
#define MAX3421E_CMD_WRITE (1u << 1)

typedef struct {
    mp_obj_base_t base;
    busio_spi_obj_t *bus;
    digitalio_digitalinout_obj_t chip_select;
    digitalio_digitalinout_obj_t irq;
    uint32_t baudrate;
    bool bus_locked;
    uint8_t gpout_cache;
} max3421e_max3421e_obj_t;

typedef enum {
    MAX3421E_GPIO_ROLE_INPUT,
    MAX3421E_GPIO_ROLE_OUTPUT,
} max3421e_gpio_role_t;

// GPIO proxy object
typedef struct {
    mp_obj_base_t base;
    mp_obj_t host;             // Max3421E instance
    uint8_t pin;               // 0-7
    max3421e_gpio_role_t role;
} max3421e_gpio_obj_t;

// Setup irq on self->irq to call tuh_int_handler(rhport, true) on falling edge
void common_hal_max3421e_max3421e_init_irq(max3421e_max3421e_obj_t *self);

// Deinit the irq
void common_hal_max3421e_max3421e_deinit_irq(max3421e_max3421e_obj_t *self);

// Enable or disable the irq interrupt.
void common_hal_max3421e_max3421e_irq_enabled(max3421e_max3421e_obj_t *self, bool enabled);

// Queue up the actual interrupt handler for when the SPI bus is free.
void max3421e_interrupt_handler(max3421e_max3421e_obj_t *self);

// GPIO access helpers
uint8_t common_hal_max3421e_max3421e_get_gpins(mp_obj_t self_in);
uint8_t common_hal_max3421e_max3421e_get_gpouts(mp_obj_t self_in);
void common_hal_max3421e_max3421e_set_gpouts(mp_obj_t self_in, uint8_t value, uint8_t mask);

