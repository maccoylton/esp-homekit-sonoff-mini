/*
 * HomeKit Custom Characteristics
 *
 * Copyright 2018 David B Brown (@maccoylton)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <homekit/homekit.h>
#include <homekit/characteristics.h>

#ifndef __HOMEKIT_DBB_CUSTOM_CHARACTERISTICS__
#define __HOMEKIT_DBB_CUSTOM_CHARACTERISTICS__

#define HOMEKIT_CUSTOM_UUID_DBB(value) (value"-4772-4466-80fd-a6ea3d5bcd55")


#define HOMEKIT_CHARACTERISTIC_CUSTOM_POLL_PERIOD HOMEKIT_CUSTOM_UUID_DBB("F0000001")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_POLL_PERIOD(_value, ...) .type = HOMEKIT_CHARACTERISTIC_CUSTOM_POLL_PERIOD, \
.description = "Poll Period", \
.format = homekit_format_uint8, \
.unit = homekit_unit_seconds, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.min_value = (float[]) {1000}, \
.max_value = (float[]) {10000}, \
.min_step = (float[]) {1}, \
.value = HOMEKIT_UINT8_(_value), \
##__VA_ARGS__


#define HOMEKIT_CHARACTERISTIC_CUSTOM_SQL_LOG HOMEKIT_CUSTOM_UUID_DBB("F0000002")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_SQL_LOG(_value, ...) \
.type = HOMEKIT_CHARACTERISTIC_CUSTOM_SQL_LOG, \
.description = "SQL LOG", \
.format = homekit_format_bool, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.value = HOMEKIT_BOOL_(_value), \
##__VA_ARGS__


#define HOMEKIT_CHARACTERISTIC_CUSTOM_LPG_LEVEL HOMEKIT_CUSTOM_UUID_DBB("F0000003")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_LPG_LEVEL(_value, ...) .type = HOMEKIT_CHARACTERISTIC_CUSTOM_LPG_LEVEL, \
.description = "LPG Level", \
.format = homekit_format_float, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_notify, \
.min_value = (float[]) {0}, \
.max_value = (float[]) {10000}, \
.min_step = (float[]) {1}, \
.value = HOMEKIT_FLOAT_(_value), \
##__VA_ARGS__

#define HOMEKIT_CHARACTERISTIC_CUSTOM_METHANE_LEVEL HOMEKIT_CUSTOM_UUID_DBB("F0000004")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_METHANE_LEVEL(_value, ...) .type = HOMEKIT_CHARACTERISTIC_CUSTOM_METHANE_LEVEL, \
.description = "Methane Level", \
.format = homekit_format_float, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_notify, \
.min_value = (float[]) {0}, \
.max_value = (float[]) {10000}, \
.min_step = (float[]) {1}, \
.value = HOMEKIT_FLOAT_(_value), \
##__VA_ARGS__

#define HOMEKIT_CHARACTERISTIC_CUSTOM_AMMONIUM_LEVEL HOMEKIT_CUSTOM_UUID_DBB("F0000005")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_AMMONIUM_LEVEL(_value, ...) .type = HOMEKIT_CHARACTERISTIC_CUSTOM_AMMONIUM_LEVEL, \
.description = "Amonium Level", \
.format = homekit_format_float, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_notify, \
.min_value = (float[]) {0}, \
.max_value = (float[]) {10000}, \
.min_step = (float[]) {1}, \
.value = HOMEKIT_FLOAT_(_value), \
##__VA_ARGS__


#endif

void save_characteristic_to_flash (homekit_characteristic_t *ch, homekit_value_t value);

void load_characteristic_from_flash (homekit_characteristic_t *ch);

void get_sysparam_info();

