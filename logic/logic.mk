LOGIC = logic

INCDIR += $(LOGIC) 
CSRC += $(LOGIC)/logic_use.c $(LOGIC)/logic_use_test.c $(LOGIC)/logic_fill.c $(LOGIC)/logic_flash.c
CSRC += $(LOGIC)/stm32f4xx_flash.c