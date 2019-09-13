# Component makefile for check_wifi

INC_DIRS += $(check_wifi_ROOT)

check_wifi_INC_DIR = $(check_wifi_ROOT)
check_wifi_SRC_DIR = $(check_wifi_ROOT)

$(eval $(call component_compile_rules,check_wifi))
