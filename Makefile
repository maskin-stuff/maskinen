CFLAGS	:=	-MMD -O0 -g -Wno-cpp
LDFLAGS	:=	-lm
SOURCES	:=	$(wildcard *.c)
OBJECTS	:=	$(SOURCES:%.c=%.o)
DEPENDS	:=	$(SOURCES:%.c=%.d)

maskinen: $(OBJECTS)
	@printf "CCLD\t%s\n" $(@)
	@$(CC) $(LDFLAGS) -o $(@) $(^)

-include $(DEPENDS)

.c.o:
	@printf "CC\t%s\n" $(@)
	@$(CC) $(CFLAGS) -o $(@) -c $(<)

clean:
	rm -rf *.o *.d maskinen

.PHONY: clean
