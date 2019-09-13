#include <sysparam.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include <custom_characteristics.h>


void get_sysparam_info() {
    uint32_t base_addr,num_sectors;
    sysparam_iter_t sysparam_iter;
    sysparam_status_t sysparam_status;
    
    sysparam_get_info(&base_addr, &num_sectors);
    
    printf ("get_sysparam_info - Sysparam base address %i, num_sectors %i\n", base_addr, num_sectors);
    sysparam_status = sysparam_iter_start (&sysparam_iter);
    while (sysparam_status==0){
        sysparam_status = sysparam_iter_next (&sysparam_iter);
        if (sysparam_status==0){
            printf("get_sysparam_info - sysparam name: %s\n", sysparam_iter.key);
        }
    }
    sysparam_iter_end (&sysparam_iter);
}


void save_characteristic_to_flash(homekit_characteristic_t *ch, homekit_value_t value){
    
    sysparam_status_t status = SYSPARAM_OK;
    bool bool_value;
    int8_t int8_value;
    int32_t int32_value;
    float float_value;
    char *string_value=NULL;    

    printf ("Save characteristic to flash\n");
    switch (ch->format) {
        case homekit_format_bool:
            printf ("writing bool value to flash\n");
            status = sysparam_get_bool(ch->description, &bool_value);
            if (status == SYSPARAM_OK && bool_value != ch->value.bool_value) {
                status = sysparam_set_bool(ch->description, ch->value.bool_value);
            } else if (status == SYSPARAM_NOTFOUND) {
                status = sysparam_set_bool(ch->description, ch->value.bool_value);
            }
            break;
        case homekit_format_uint8:
            printf ("writing int8 value to flash\n");
            status = sysparam_get_int8(ch->description, &int8_value);
            if (status == SYSPARAM_OK && int8_value != ch->value.int_value) {
                status = sysparam_set_int8(ch->description, ch->value.int_value);
            } else  if (status == SYSPARAM_NOTFOUND) {
                status = sysparam_set_int8(ch->description, ch->value.int_value);
            }
            break;
        case homekit_format_uint16:
        case homekit_format_uint32:
            printf ("writing int32 value to flash\n");
            status = sysparam_get_int32(ch->description, &int32_value);
            if (status == SYSPARAM_OK && int32_value != ch->value.int_value) {
                status = sysparam_set_int32(ch->description, ch->value.int_value);
            } else  if (status == SYSPARAM_NOTFOUND) {
                status = sysparam_set_int32(ch->description, ch->value.int_value);
            }
            break;
        case homekit_format_string:
            printf ("writing string value to flash\n");
            status = sysparam_get_string(ch->description, &string_value);
            if (status == SYSPARAM_OK && !strcmp (string_value, ch->value.string_value)) {
                status = sysparam_set_string(ch->description, ch->value.string_value);
            }  else  if (status == SYSPARAM_NOTFOUND) {
                status = sysparam_set_string(ch->description, ch->value.string_value);
            }
            free(string_value);
            break;
        case homekit_format_float:
            printf ("writing float value to flash\n");
            status = sysparam_get_int32(ch->description, &int32_value);
            float_value = int32_value * 1.00f / 100;
            if (status == SYSPARAM_OK && float_value != ch->value.float_value) {
		int32_value = (int)ch->value.float_value*100;
		status = sysparam_set_int32(ch->description, int32_value);
            } else if (status == SYSPARAM_NOTFOUND) {
		int32_value = (int)ch->value.float_value*100;
                status = sysparam_set_int32(ch->description, int32_value);
            }
	    break;
        case homekit_format_uint64:
        case homekit_format_int:
        case homekit_format_tlv:
        default:
            printf ("Unknown characteristic format in save_charactersitics_to_flash\n");
    }
    if (status != SYSPARAM_OK){
        printf ("Error in sysparams error:%i writing characteristic\n", status);
    }
    
}

void load_characteristic_from_flash (homekit_characteristic_t *ch){
              
                    
    sysparam_status_t status = SYSPARAM_OK;
    bool bool_value;
    int8_t int8_value;
    int32_t int32_value;
    char *string_value = NULL;
    printf ("Loading sysparam %s\n",ch->description);
    switch (ch->format){
        case homekit_format_bool:
            printf("Loading bool\n");
            status = sysparam_get_bool(ch->description, &bool_value);
            if (status == SYSPARAM_OK ) {
                ch->value.bool_value = bool_value;
            }
            break;
        case homekit_format_uint8:
            printf("Loading int8\n");
            status = sysparam_get_int8(ch->description, &int8_value);
            if (status == SYSPARAM_OK) {
                ch->value.int_value = int8_value;
            }
            break;
        case homekit_format_uint16:
        case homekit_format_uint32:
            printf("Loading in32\n");
            status = sysparam_get_int32(ch->description, &int32_value);
            if (status == SYSPARAM_OK ) {
                ch->value.int_value = int32_value;
            }
            break;
        case homekit_format_string:
            printf("Loading string\n");
            status = sysparam_get_string(ch->description, &string_value);
            if (status == SYSPARAM_OK) {
                ch->value = HOMEKIT_STRING(string_value);
            }
            break;
        case homekit_format_float:
            printf("Loading float\n");
            status = sysparam_get_int32(ch->description, &int32_value);
            if (status == SYSPARAM_OK ) {
                ch->value.float_value = int32_value * 1.0f /100;
            }
            break;
        case homekit_format_uint64:
        case homekit_format_int:
        case homekit_format_tlv:
        default:
            printf ("Unknown characteristic format in save_charactersitics_to_flash\n");
    }
    if (status != SYSPARAM_OK){
        printf ("Error in sysparams error:%i loading characteristic\n", status);
    }
    
    
    
}
