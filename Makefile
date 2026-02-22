CFLAGS	:=	$(shell pkg-config --cflags jack) -MMD
LDFLAGS	:=	$(shell pkg-config --libs jack)
SOURCES	:=	$(wildcard *.c)
OBJECTS	:=	$(SOURCES:%.c=%.o)
DEPENDS	:=	$(SOURCES:%.c=%.d)

maskin: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(@) $(^)

-include $(DEPENDS)

.c.o:
	$(CC) $(CFLAGS) -o $(@) -c $(<)

clean:
	rm -rf *.o *.d maskin

.PHONY: clean
