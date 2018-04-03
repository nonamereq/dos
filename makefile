SRCDIR:=./src/peer
DDOSSRCDIR:=./src/ddos
BUILDDIR:=build

SRCFILES:=$(wildcard $(SRCDIR)/*.c)
DDOSFILE:=$(wildcard $(DDOSSRCDIR)/*.c)

OBJDIR:=build/objs
OBJFILES=$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCFILES))

DDOSOBJS=$(patsubst $(DDOSSRCDIR)/*.c, $(OBJDIR)/%.o, $(DDOSFILE))

CC:=clang
CCX:=clang++
LIBRARIES:=
DEBUG:=-g
INCLUDE=./
RELEASE:=-o2
COMPILE:=-c
LDFLAGS:=
CCFLAGS=-fno-omit-frame-pointer
SANITIZE:=-fsanitize=address,undefined

debug: CCFLAGS += $(DEBUG) 
debug: all

release: CCFLAGS += $(RELEASE)
release: all

sanitize: CCFLAGS += $(SANITIZE)
sanitize: all

all: prep peer ddos


ddos: $(DDOSOBJS)
	$(CC) $(CCFLAGS) -o $@ $^

peer: $(OBJFILES)
	$(CC) $(CCFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -I $(INCLUDE) $(CCFLAGS) $(COMPILE) -o $@ $<


prep:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(OBJDIR)

clean:
	rm -rf $(BUILDDIR)
	rm -rf peer
	rm -rf ddos
