# Common for all Toradex modules
ifeq ($(CONFIG_SPL_BUILD),y)
# necessary to create built-in.o
obj- := __dummy__.o
else
obj-$(CONFIG_TRDX_CFG_BLOCK) += ../common/configblock.o
obj-y += ../common/common.o
endif
