
ROM=dev7

CC=..\..\glcc
CFLAGS32=-map=32k -rom=${ROM}
CFLAGS64=-map=64k -rom=${ROM}

all: TSTfill.gt1 TSTblit.gt1

TSTfill.gt1: TSTfill.c
	${CC} ${CFLAGS32} -o $@ $<

TSTblit.gt1: TSTblit.c
	${CC} ${CFLAGS32} -o $@ $<

TSTrotator.gt1: TSTrotator.s
	${CC} ${CFLAGS64} -o $@ --gt1-exec-address=main --entry=main --frags $<

TSTcontrol.gt1: TSTcontrol.s
	${CC} ${CFLAGS64} -o $@ --gt1-exec-address=main --entry=main --frags $<


clean: FORCE
	-rm 2>&1 *.gt1

FORCE:

.PHONY: FORCE
