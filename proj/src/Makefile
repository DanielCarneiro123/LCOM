# Nome do binário. Tem de ser "proj".
PROG=proj

# Indicar as pastas de todos os ficheiros a compilar.
.PATH: ${.CURDIR}/driver/
.PATH: ${.CURDIR}/driver/keyboard
.PATH: ${.CURDIR}/driver/mouse
.PATH: ${.CURDIR}/driver/timer
.PATH: ${.CURDIR}/driver/video
.PATH: ${.CURDIR}/driver/rtc
.PATH: ${.CURDIR}/driver/serialport
.PATH: ${.CURDIR}/logic
.PATH: ${.CURDIR}/logic/queue
.PATH: ${.CURDIR}/model/
.PATH: ${.CURDIR}/view/

# Ficheiros ".c" a compilar. Têm de estar contidos nas pastas indicadas no passo anterior.
SRCS = 	main.c timer.c utils.c graphics.c KBC.c keyboard.c mouse.c sprite.c model.c view.c rtc.c sp.c queue.c board_logic.c

# Flags de compilação
CFLAGS += -pedantic -D_LCOM_OPTIMIZED_ -O2

# Dependências
DPADD += ${LIBLCF}
LDADD += -Wall -llcf -lxpm -llm
.include <minix.lcom.mk>
