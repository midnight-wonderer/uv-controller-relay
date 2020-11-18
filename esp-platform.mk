PROJECT_NAME := uv_controller

ifndef IS_BOOTLOADER_BUILD
APPLICATION_LDFLAGS := $(SUGGESTED_APPLICATION_LDFLAGS)
endif

include $(IDF_PATH)/make/project.mk

var-APP_ELF:
	@echo $(VAR_SPLITTER) && \
	echo $(APP_ELF)

var-CC:
	@echo $(VAR_SPLITTER) && \
	echo $(CC)

var-CC_PARAMS:
	@echo $(VAR_SPLITTER) && \
	echo $(CFLAGS) $(CPPFLAGS) $(addprefix -I,$(COMPONENT_INCLUDES)) $(addprefix -I,$(COMPONENT_EXTRA_INCLUDES))

var-AR:
	@echo $(VAR_SPLITTER) && \
	echo $(AR)
