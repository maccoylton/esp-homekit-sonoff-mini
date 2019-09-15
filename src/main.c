/*
 * Copyright 2018 David B Brown (@maccoylton)
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
 *
 * Homekit firmware for Sonoff Mini
 */

#define DEVICE_MANUFACTURER "itead"
#define DEVICE_NAME "Sonoff"
#define DEVICE_MODEL "Mini"
#define DEVICE_SERIAL "12345678"
#define FW_VERSION "1.0"

#include <stdio.h>
#include <espressif/esp_wifi.h>
#include <espressif/esp_sta.h>
#include <espressif/esp_common.h>
#include <esp/uart.h>
#include <esp8266.h>
#include <FreeRTOS.h>
#include <task.h>

#include <homekit/homekit.h>
#include <homekit/characteristics.h>
//#include <wifi_config.h>

#include <adv_button.h>
#include <led_codes.h>
#include <udplogger.h>


// add this section to make your device OTA capable
// create the extra characteristic &ota_trigger, at the end of the primary service (before the NULL)
// it can be used in Eve, which will show it, where Home does not
// and apply the four other parameters in the accessories_information section

#include <ota-api.h>

void switch_on_callback(homekit_characteristic_t *_ch, homekit_value_t on, void *context);

homekit_characteristic_t ota_trigger  = API_OTA_TRIGGER;
homekit_characteristic_t name         = HOMEKIT_CHARACTERISTIC_(NAME, DEVICE_NAME);
homekit_characteristic_t manufacturer = HOMEKIT_CHARACTERISTIC_(MANUFACTURER,  DEVICE_MANUFACTURER);
homekit_characteristic_t serial       = HOMEKIT_CHARACTERISTIC_(SERIAL_NUMBER, DEVICE_SERIAL);
homekit_characteristic_t model        = HOMEKIT_CHARACTERISTIC_(MODEL,         DEVICE_MODEL);
homekit_characteristic_t revision     = HOMEKIT_CHARACTERISTIC_(FIRMWARE_REVISION,  FW_VERSION);
homekit_characteristic_t switch_on = HOMEKIT_CHARACTERISTIC_(
                                                             ON, false, .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(switch_on_callback)
                                                             );


// The GPIO pin that is connected to the relay on the Sonoff Mini.
const int relay_gpio = 12;
// The GPIO pin that is connected to the LED on the Sonoff Mini.
const int LED_GPIO = 13;
// The GPIO pin that is oconnected to the button on the Sonoff Mini.
const int reset_button_gpio = 0;
const int s2_gpio = 4;
int led_off_value=1; /* global varibale to support LEDs set to 0 where the LED is connected to GND, 1 where +3.3v */


void relay_write(bool on) {
    gpio_write(relay_gpio, on ? 1 : 0);
}

void led_write(bool on) {
    gpio_write(LED_GPIO, on ? 0 : 1);
}

void reset_configuration_task() {
    //Flash the LED first before we start the reset
    led_code (LED_GPIO, WIFI_CONFIG_RESET);    
    printf("Resetting Wifi Config\n");
    
//    wifi_config_reset();
    
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
    printf("Resetting HomeKit Config\n");
    
    homekit_server_reset();
    
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
    printf("Restarting\n");
    
    sdk_system_restart();
    
    vTaskDelete(NULL);
}

void reset_configuration() {
    printf("Resetting Sonoff configuration\n");
    xTaskCreate(reset_configuration_task, "Reset configuration", 256, NULL, 2, NULL);
}

void reset_button_callback(uint8_t gpio, void* args) {
    
    printf("Reset Button event long press on GPIO : %d\n", gpio);
    reset_configuration();
    
}

void s2_button_callback(uint8_t gpio, void* args) {
    
    printf("S2 Button event single press on GPIO : %d\n", gpio);
    printf("Toggling relay\n");
    switch_on.value.bool_value = !switch_on.value.bool_value;
    relay_write(switch_on.value.bool_value);
    led_write(switch_on.value.bool_value);
    homekit_characteristic_notify(&switch_on, switch_on.value);
}

void gpio_init() {

    const uint8_t toggle_press = 0, single_press = 1, double_press = 2,  long_press = 3, very_long_press = 4, hold_press = 5;

    adv_button_set_evaluate_delay(10);

    /* GPIO for button, pull-up resistor, inverted */
    printf("Initialising buttons\n");
    adv_button_create(s2_gpio, true, false);
    adv_button_register_callback_fn(s2_gpio, s2_button_callback, toggle_press, NULL);
    adv_button_register_callback_fn(s2_gpio, s2_button_callback, single_press, NULL);
    
    adv_button_create(reset_button_gpio, true, false);
    adv_button_register_callback_fn(reset_button_gpio, reset_button_callback, very_long_press, NULL);
    
    
    gpio_enable(LED_GPIO, GPIO_OUTPUT);
    led_write(false);
    gpio_enable(relay_gpio, GPIO_OUTPUT);
    relay_write(switch_on.value.bool_value);
    
}

void switch_on_callback(homekit_characteristic_t *_ch, homekit_value_t on, void *context) {
    printf("Switch on callback\n");
    relay_write(switch_on.value.bool_value);
    led_write(switch_on.value.bool_value);
}


void switch_identify_task(void *_args) {
    // We identify the Sonoff by Flashing it's LED.
    led_code( LED_GPIO, IDENTIFY_ACCESSORY);
    vTaskDelete(NULL);
}

void switch_identify(homekit_value_t _value) {
    printf("Switch identify\n");
    xTaskCreate(switch_identify_task, "Switch identify", 128, NULL, 2, NULL);
}


homekit_accessory_t *accessories[] = {
    HOMEKIT_ACCESSORY(.id=1, .category=homekit_accessory_category_switch, .services=(homekit_service_t*[]){
        HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]){
            &name,
            &manufacturer,
            &serial,
            &model,
            &revision,
            HOMEKIT_CHARACTERISTIC(IDENTIFY, switch_identify),
            NULL
        }),
        HOMEKIT_SERVICE(SWITCH, .primary=true, .characteristics=(homekit_characteristic_t*[]){
            HOMEKIT_CHARACTERISTIC(NAME, "Switch"),
            &switch_on,
            &ota_trigger,
            NULL
        }),
        NULL
    }),
    NULL
};

homekit_server_config_t config = {
    .accessories = accessories,
    .password = "111-11-111"
};

void create_accessory_name() {

    int serialLength = snprintf(NULL, 0, "%d", sdk_system_get_chip_id());

    char *serialNumberValue = malloc(serialLength + 1);

    snprintf(serialNumberValue, serialLength + 1, "%d", sdk_system_get_chip_id());
    
    int name_len = snprintf(NULL, 0, "%s-%s-%s",
				DEVICE_NAME,
				DEVICE_MODEL,
				serialNumberValue);

    if (name_len > 63) {
        name_len = 63;
    }

    char *name_value = malloc(name_len + 1);

    snprintf(name_value, name_len + 1, "%s-%s-%s",
		 DEVICE_NAME, DEVICE_MODEL, serialNumberValue);

   
    name.value = HOMEKIT_STRING(name_value);
    serial.value = name.value;
}

void user_init(void) {
    uart_set_baud(0, 115200);
    
    udplog_init(3);
    gpio_init();
    
    create_accessory_name();
    
    
    int c_hash=ota_read_sysparam(&manufacturer.value.string_value,&serial.value.string_value,
                                 &model.value.string_value,&revision.value.string_value);
    if (c_hash==0) c_hash=1;
    config.accessories[0]->config_number=c_hash;
    
    homekit_server_init(&config);
}
