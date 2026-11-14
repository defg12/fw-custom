ifneq ($(PROJECT_CPU),simulator)
BOARDCPPSRC += \
    $(BOARD_DIR)/board_configuration.cpp \
BOARDINC += $(BOARD_DIR)
DDEFS += -DEFI_SDC_DEVICE=SDCD1
endif
