# Component makefile for ota

INC_DIRS += $(ota_ROOT)

ota_INC_DIR = $(ota_ROOT)
ota_SRC_DIR = $(ota_ROOT)

$(eval $(call component_compile_rules,ota))
