# Component makefile for button

INC_DIRS += $(button_ROOT)

button_INC_DIR = $(button_ROOT)
button_SRC_DIR = $(button_ROOT)

$(eval $(call component_compile_rules,button))
