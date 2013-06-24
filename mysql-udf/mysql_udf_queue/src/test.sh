src=$(wildcard *.c *.cpp ./pbc/*.c ./pbc/*.cc)
dir=$(notdir $(src))
obj=$(patsubst %.c,%.o,$(dir) )
 
all:
	@echo $(src)
	@echo $(dir)
	@echo $(obj)
	@echo "end"