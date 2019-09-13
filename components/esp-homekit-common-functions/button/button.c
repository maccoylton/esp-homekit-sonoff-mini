#include <string.h>
#include <etstimer.h>
#include <esplibs/libmain.h>
#include "button.h"


typedef struct _button {
    uint8_t gpio_num;
    button_callback_fn callback;
    
    uint16_t debounce_time;
    uint16_t long_press_time;
    uint16_t double_press_time;
    
    uint8_t press_count;
    ETSTimer press_timer;
    uint32_t last_press_time;
    uint32_t last_event_time;
    
    struct _button *next;
} button_t;


button_t *buttons = NULL;


static button_t *button_find_by_gpio(const uint8_t gpio_num) {
    button_t *button = buttons;
    while (button && button->gpio_num != gpio_num)
        button = button->next;
    
    return button;
}


void button_intr_callback(uint8_t gpio) {
    int last_event_elapsed =0;
    int last_press_elapsed=0;
    button_t *button = button_find_by_gpio(gpio);
    if (!button)
        return;
    
    uint32_t now = xTaskGetTickCountFromISR();
    last_event_elapsed =(now - button->last_event_time)*portTICK_PERIOD_MS;
/*    printf ("button press gpio = %i last event elapsed = %i, portTICK_PERIOD_MS = %i, mow = %i, last event time %i \n", gpio, last_event_elapsed, portTICK_PERIOD_MS, now, button->last_event_time); */
    
    if (last_event_elapsed < button->debounce_time) {
        // debounce time, ignore events
        return;
    }
    
    button->last_event_time = now;
    int gpio_value =gpio_read(button->gpio_num);
/*    printf ("GPIO value is %i\n", gpio_value); */
    if (gpio_value == button_pressed_value ){
        button->last_press_time = now;
/*        printf("Button depressed set button last press time to %i\n", now); */
    } else {
/*        printf("Button released set button last press time to %i\n", now);*/
        last_press_elapsed = (now - button->last_press_time)*portTICK_PERIOD_MS;
        if (last_press_elapsed > button->long_press_time) {
            button->press_count = 0;
/*            printf ("Long press button press gpio = %i last press elapsed = %i, portTICK_PERIOD_MS = %i, mow = %i, last press timme = %i\n", gpio, last_press_elapsed, portTICK_PERIOD_MS, now, button->last_press_time); */
            button->callback(button->gpio_num, button_event_long_press);
        } else {
/*            printf ("Not long press\n"); */
            button->press_count++;
            if (button->press_count > 1) {
/*               printf ("Press count >1, double press\n");*/
                button->press_count = 0;
                sdk_os_timer_disarm(&button->press_timer);
                
                button->callback(button->gpio_num, button_event_double_press);
            } else {
/*                printf ("Press count not > 1, check for double press\n");*/
                sdk_os_timer_arm(&button->press_timer, button->double_press_time, 1);
            }
        }
    }
}


void button_timer_callback(void *arg) {
    button_t *button = arg;
    
/*    printf ("button timer callback\n");*/
    button->press_count = 0;
    sdk_os_timer_disarm(&button->press_timer);
    button->callback(button->gpio_num, button_event_single_press);
}


int button_create(const uint8_t gpio_num, button_callback_fn callback) {
    button_t *button = button_find_by_gpio(gpio_num);
    if (button)
        return -1;
    
    button = malloc(sizeof(button_t));
    memset(button, 0, sizeof(*button));
    button->gpio_num = gpio_num;
    button->callback = callback;
    
    // times in milliseconds
    button->debounce_time = 50;
    button->long_press_time = 1000;
    button->double_press_time = 500;
    
    button->next = buttons;
    buttons = button;
    
    gpio_set_pullup(button->gpio_num, true, true);
    gpio_set_interrupt(button->gpio_num, GPIO_INTTYPE_EDGE_ANY, button_intr_callback);
    
    sdk_os_timer_disarm(&button->press_timer);
    sdk_os_timer_setfn(&button->press_timer, button_timer_callback, button);
    
    return 0;
}


void button_delete(const uint8_t gpio_num) {
    if (!buttons)
        return;
    
    button_t *button = NULL;
    if (buttons->gpio_num == gpio_num) {
        button = buttons;
        buttons = buttons->next;
    } else {
        button_t *b = buttons;
        while (b->next) {
            if (b->next->gpio_num == gpio_num) {
                button = b->next;
                b->next = b->next->next;
                break;
            }
        }
    }
    
    if (button) {
        sdk_os_timer_disarm(&button->press_timer);
        gpio_set_interrupt(button->gpio_num, GPIO_INTTYPE_EDGE_ANY, NULL);
    }
}


