# Component makefile for http_post

INC_DIRS += $(http_post_ROOT)

http_post_INC_DIR = $(http_post_ROOT)
http_post_SRC_DIR = $(http_post_ROOT)

$(eval $(call component_compile_rules,http_post))
