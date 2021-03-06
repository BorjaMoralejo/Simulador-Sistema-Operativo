
SRCDIR = src
OBJDIR = obj
INCDIR = include
CFLAGS = -I$(INCDIR) -pthread

objs = $(addprefix $(OBJDIR)/, simulador.o clock.o timer.o scheduler.o estructuras.o randomp.o queue.o rt_struct.o list.o dispatcher.o physical.o loader.o tlb.o machine.o)

vpath %.h $(INCDIR)
vpath %.o $(OBJDIR)
vpath %.c $(SRCDIR)

#Target principal, monta el simulador
simulador: $(objs)
	gcc -o simulador $(CFLAGS) $(objs)


#Target para cada clase
$(OBJDIR)/clock.o: clock.c globals.h estructuras.h
	gcc -c $(CFLAGS) -o $@ $<
$(OBJDIR)/estructuras.o: estructuras.c globals.h estructuras.h
	gcc -c $(CFLAGS) -o $@ $<
$(OBJDIR)/queue.o: queue.c queue.h estructuras.h
	gcc -c $(CFLAGS) -o $@ $<
$(OBJDIR)/randomp.o: randomp.c
	gcc -c $(CFLAGS) -o $@ $<
$(OBJDIR)/scheduler.o: scheduler.c globals.h estructuras.h queue.h
	gcc -c $(CFLAGS) -o $@ $<
$(OBJDIR)/simulador.o: simulador.c defines.h estructuras.h queue.h estructuras.h clock.h timer.h scheduler.h pgenerator.h
	gcc -c $(CFLAGS) -o $@ $<
$(OBJDIR)/timer.o: timer.c globals.h estructuras.h defines.h
	gcc -c $(CFLAGS) -o $@ $<
$(OBJDIR)/rt_struct.o: rt_struct.c queue.h estructuras.h
	gcc -c $(CFLAGS) -o $@ $<
$(OBJDIR)/list.o: list.c list.h estructuras.h
	gcc -c $(CFLAGS) -o $@ $<
$(OBJDIR)/dispatcher.o: dispatcher.c dispatcher.h estructuras.h rt_struct.h queue.h list.h scheduler.h globals.h
	gcc -c $(CFLAGS) -o $@ $<
$(OBJDIR)/physical.o: physical.c physical.h
	gcc -c $(CFLAGS) -o $@ $<
$(OBJDIR)/loader.o: loader.c loader.h
	gcc -c $(CFLAGS) -o $@ $<
$(OBJDIR)/tlb.o: tlb.c loader.h
	gcc -c $(CFLAGS) -o $@ $<
$(OBJDIR)/machine.o: machine.c machine.h
	gcc -c $(CFLAGS) -o $@ $<
