# Component makefile for custom_characteristics


INC_DIRS += $(custom_characteristics_ROOT)


custom_characteristics_INC_DIR = $(custom_characteristics_ROOT)
custom_characteristics_SRC_DIR = $(custom_characteristics_ROOT)

$(eval $(call component_compile_rules,custom_characteristics))
