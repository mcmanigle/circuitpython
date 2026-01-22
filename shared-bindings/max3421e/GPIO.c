// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 John McManigle
//
// SPDX-License-Identifier: MIT

#include "shared-module/max3421e/Max3421E.h"

#include "shared-bindings/max3421e/Max3421E.h"
#include "shared-bindings/max3421e/GPIO.h"
#include "shared-bindings/digitalio/Direction.h"

#include "py/runtime.h"
#include "py/objproperty.h"

//| class GPIO:
//|     """Proxy object representing a MAX3421E GPIO pin.
//|
//|     This object provides a pin-like interface compatible with many
//|     CircuitPython libraries. The pin's role (input or output) is fixed
//|     by the MAX3421E hardware and cannot be changed.
//|
//|     This object is not constructed directly; it is returned by
//|     :meth:`Max3421E.get_gpin` and :meth:`Max3421E.get_gpout`.
//|     """
//|
//|     value: bool
//|     """The digital logic level of the GPIO pin.
//|
//|     For input pins, reading ``value`` returns the current pin state.
//|
//|     For output pins, reading returns the last value written, and
//|     writing sets the output level.
//|
//|     Writing to an input pin raises ``ValueError``.
//|     """
static mp_obj_t max3421e_gpio_get_value(mp_obj_t self_in) {
    max3421e_gpio_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->role == MAX3421E_GPIO_ROLE_INPUT) {
        uint8_t v =
            common_hal_max3421e_max3421e_get_gpins(self->host);
        return mp_obj_new_bool((v >> self->pin) & 1);
    }

    // OUTPUT: read back cached outputs
    uint8_t v =
        common_hal_max3421e_max3421e_get_gpouts(self->host);
    return mp_obj_new_bool((v >> self->pin) & 1);
}
MP_DEFINE_CONST_FUN_OBJ_1(
    max3421e_gpio_get_value_obj,
    max3421e_gpio_get_value
);

static mp_obj_t max3421e_gpio_set_value(
    mp_obj_t self_in,
    mp_obj_t value_in) {

    max3421e_gpio_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->role != MAX3421E_GPIO_ROLE_OUTPUT) {
        mp_raise_ValueError(
            MP_ERROR_TEXT("Pin is input-only"));
    }

    bool v = mp_obj_is_true(value_in);
    uint8_t mask = (uint8_t)(1u << self->pin);
    uint8_t out  = v ? mask : 0;

    common_hal_max3421e_max3421e_set_gpouts(
        self->host, out, mask);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(
    max3421e_gpio_set_value_obj,
    max3421e_gpio_set_value
);

static const mp_obj_property_t max3421e_gpio_value_obj = {
    .base.type = &mp_type_property,
    .proxy = {
        (mp_obj_t)&max3421e_gpio_get_value_obj,
        (mp_obj_t)&max3421e_gpio_set_value_obj,
        mp_const_none,
    },
};

//|     direction: digitalio.Direction
//|     """The fixed direction of the GPIO pin.
//|
//|     The direction is determined by the hardware configuration and
//|     cannot be changed. Assigning the same direction is allowed and
//|     has no effect. Assigning a different direction raises ``ValueError``.
//|     """
static mp_obj_t max3421e_gpio_get_direction(mp_obj_t self_in) {
    max3421e_gpio_obj_t *self = MP_OBJ_TO_PTR(self_in);

    digitalio_direction_t dir =
        (self->role == MAX3421E_GPIO_ROLE_INPUT)
        ? DIRECTION_INPUT
        : DIRECTION_OUTPUT;

    return MP_OBJ_NEW_SMALL_INT(dir);
}
MP_DEFINE_CONST_FUN_OBJ_1(
    max3421e_gpio_get_direction_obj,
    max3421e_gpio_get_direction
);

static mp_obj_t max3421e_gpio_set_direction(
    mp_obj_t self_in,
    mp_obj_t dir_in) {

    max3421e_gpio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    digitalio_direction_t requested =
        (digitalio_direction_t)mp_obj_get_int(dir_in);

    digitalio_direction_t actual =
        (self->role == MAX3421E_GPIO_ROLE_INPUT)
        ? DIRECTION_INPUT
        : DIRECTION_OUTPUT;

    if (requested != actual) {
        mp_raise_ValueError(
            MP_ERROR_TEXT("Pin direction is fixed by hardware"));
    }

    // Compatible direction → no-op
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(
    max3421e_gpio_set_direction_obj,
    max3421e_gpio_set_direction
);

static const mp_obj_property_t max3421e_gpio_direction_obj = {
    .base.type = &mp_type_property,
    .proxy = {
        (mp_obj_t)&max3421e_gpio_get_direction_obj,
        (mp_obj_t)&max3421e_gpio_set_direction_obj,
        mp_const_none,
    },
};


static const mp_rom_map_elem_t max3421e_gpio_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_value),
      MP_ROM_PTR(&max3421e_gpio_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_direction),
      MP_ROM_PTR(&max3421e_gpio_direction_obj) },
};
static MP_DEFINE_CONST_DICT(
    max3421e_gpio_locals_dict,
    max3421e_gpio_locals_dict_table
);


MP_DEFINE_CONST_OBJ_TYPE(
    max3421e_gpio_type,
    MP_QSTR_GPIO,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    locals_dict, &max3421e_gpio_locals_dict
);

