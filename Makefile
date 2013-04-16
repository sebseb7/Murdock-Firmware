PROJECT=template


STM32F=4
CORTEXM=4
LSCRIPT=core/stm32f$(STM32F)xx_flash.ld

OPTIMIZATION = -O2

WITHUSB=1



SRC=$(wildcard  *.c libs/*.c) core/stm32fxxx_it.c core/system_stm32f$(STM32F)xx.c core/syscalls.c

ifeq ($(WITHUSB),1)
SRC +=$(wildcard usb/*.c) STM32_USB_Device_Library/Core/src/usbd_core.c \
	STM32_USB_Device_Library/Core/src/usbd_req.c \
	STM32_USB_Device_Library/Core/src/usbd_ioreq.c \
	STM32_USB_Device_Library/Class/cdc/src/usbd_cdc_core.c \
	STM32_USB_OTG_Driver/src/usb_core.c \
	STM32_USB_OTG_Driver/src/usb_dcd.c \
	STM32_USB_OTG_Driver/src/usb_dcd_int.c 
endif

ASRC=core/startup_stm32f$(STM32F)xx.s
LSTFILES=$(patsubst %,.bin/%,$(SRC:.c=.lst))

DEPS   =$(patsubst %,.bin/%,$(SRC:.c=.d))
OBJECTS=$(patsubst %,.bin/%,$(SRC:.c=.o)) $(patsubst %,.bin/%,$(ASRC:.s=.o))

#  Compiler Options
GCFLAGS = -DSTM32F=$(STM32F)  -ffreestanding -std=gnu99 -mcpu=cortex-m$(CORTEXM) -mthumb $(OPTIMIZATION) -I. -Icore  -DARM_MATH_CM$(CORTEXM) -DUSE_STDPERIPH_DRIVER 
ifeq ($(CORTEXM),4)
GCFLAGS+= -mfpu=fpv4-sp-d16 -mfloat-abi=hard -falign-functions=16 
endif
ifeq ($(WITHUSB),1)
GCFLAGS+=-DUSE_USB_OTG_FS=1 -Iusb
GCFLAGS+=-ISTM32_USB_Device_Library/Class/cdc/inc
GCFLAGS+=-ISTM32_USB_OTG_Driver/inc
GCFLAGS+=-ISTM32_USB_Device_Library/Core/inc
endif
# Warnings
GCFLAGS += -Wstrict-prototypes -Wundef -Wall -Wextra -Wunreachable-code  
# Optimizazions
GCFLAGS += -fsingle-precision-constant -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fno-builtin -ffunction-sections -fno-common -fdata-sections 
# Debug stuff
GCFLAGS += -Wa,-adhlns=.bin/$(<:.c=.lst),-gstabs -g 

GCFLAGS+= -ISTM32F$(STM32F)_drivers/inc 



LDFLAGS = -mcpu=cortex-m$(CORTEXM) -mthumb $(OPTIMIZATION) -nostartfiles -T$(LSCRIPT) 
ifeq ($(CORTEXM),4)
LDFLAGS+= -mfpu=fpv4-sp-d16 -mfloat-abi=hard -falign-functions=16
endif
LDFLAGS+= -LSTM32F$(STM32F)_drivers/build -lSTM32F$(STM32F)xx_drivers -lm -lnosys -lc


#  Compiler/Assembler Paths
GCC = arm-none-eabi-gcc
AS = arm-none-eabi-as
OBJCOPY = arm-none-eabi-objcopy
REMOVE = rm -f
SIZE = arm-none-eabi-size

#########################################################################

all: STM32F$(STM32F)_drivers/build/libSTM32F$(STM32F)_drivers.a .bin/$(PROJECT).bin Makefile 
	@$(SIZE) .bin/$(PROJECT).elf

STM32F$(STM32F)_drivers/build/libSTM32F$(STM32F)_drivers.a:
	@make -C STM32F$(STM32F)_drivers/build

.bin/$(PROJECT).bin: .bin/$(PROJECT).elf Makefile
	@echo "generating $(PROJECT).bin"
	@$(OBJCOPY) --strip-unneeded -S -g -R .stack -O binary .bin/$(PROJECT).elf .bin/$(PROJECT).bin

.bin/$(PROJECT).elf: $(OBJECTS) Makefile $(LSCRIPT)
	@echo "  LD $(PROJECT).elf"
	@$(GCC) $(OBJECTS) $(LDFLAGS)  -o .bin/$(PROJECT).elf

clean:
	$(REMOVE) $(DEPS)
	$(REMOVE) $(OBJECTS)
	$(REMOVE) $(LSTFILES)
	$(REMOVE) .bin/$(PROJECT).bin
	$(REMOVE) .bin/$(PROJECT).elf
	$(REMOVE) -r .bin
	make -C STM32F$(STM32F)_drivers/build clean

#########################################################################

-include $(DEPS)

.bin/%.o: %.c Makefile 
	@echo "  \033[1;34mCompile \033[0m $<\033[0m"
	@mkdir -p $(dir $@)
	@$(GCC) $(GCFLAGS) -o $@ -c $<
	@$(GCC) $(GCFLAGS) -MM $< > $*.d.tmp
	@sed -e 's|.*:|.bin/$*.o:|' < $*.d.tmp > .bin/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
		sed -e 's/^ *//' -e 's/$$/:/' >> .bin/$*.d
	@rm -f $*.d.tmp

.bin/%.o: %.s Makefile 
	@echo "  AS $<"
	@mkdir -p $(dir $@)
	@$(AS) $(ASFLAGS) -o $@  $< 

#########################################################################

flash: all
	dfu-util -a 0 -s 0x08000000 -D .bin/$(PROJECT).bin -R

.PHONY : clean all flash
