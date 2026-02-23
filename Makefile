CFLAGS	:=	$(shell pkg-config --cflags jack) -MMD -O0 -g -Wno-cpp
LDFLAGS	:=	$(shell pkg-config --libs jack) -lm
SOURCES	:=	$(wildcard *.c)
OBJECTS	:=	$(SOURCES:%.c=%.o)
DEPENDS	:=	$(SOURCES:%.c=%.d)

maskin: $(OBJECTS)
	@printf "CCLD\t%s\n" $(@)
	@$(CC) $(LDFLAGS) -o $(@) $(^)

-include $(DEPENDS)

.c.o:
	@printf "CC\t%s\n" $(@)
	@$(CC) $(CFLAGS) -o $(@) -c $(<)

clean:
	rm -rf *.o *.d maskin

.PHONY: clean
