SRCDIR:=./
BUILDDIR:=build

SRCFILES:=$(wildcard $(SRCDIR)/*.c)
BUILTINFILES:=$(wildcard $(BUILTINDIR)/*.c)

OBJDIR:=build/objs
OBJFILES=$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCFILES))

CC:=clang
CCX:=clang++
LIBRARIES:=
DEBUG:=-g
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

all: prep peer


peer: $(OBJFILES)
	$(CC) $(CCFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CCFLAGS) $(BUILTINARG) $(COMPILE) -o $@ $<


prep:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(OBJDIR)

clean:
	rm -rf $(BUILDDIR)
