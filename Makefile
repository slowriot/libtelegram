ECHO=/bin/echo
CXX=g++
#DEBUGDEFINES=-DNDEBUG
DEBUGDEFINES+=-ggdb3 -Og
GCCOPTS=-std=gnu++1z -Wall -Wextra -Wno-unused-variable -Wno-vla -Wsizeof-pointer-memaccess -Wmissing-declarations -march=native -fexceptions -fexpensive-optimizations -flto -Ofast
#LINKOPTS=-static -static-libgcc -static-libstdc++ -flto=jobserver -pthread -Wl,--warn-once -Wl,--warn-common
LINKOPTS=-static-libgcc -static-libstdc++ -pthread -flto=jobserver -Wl,--warn-once -Wl,--warn-common
LIBS=-lboost_system \
     -lssl \
     -lcrypto
INCLUDES=-I. \
         -isystem include
CPPFLAGS=-DURDL_HEADER_ONLY=1 $(DEBUGDEFINES) $(GCCOPTS) $(INCLUDES)
LINKFLAGS=$(LINKOPTS)

# colour coding
NO_COLOUR=\033[0m
OK_COLOUR=\x1b[32;01m
ERROR_COLOUR=\x1b[31;01m
WARN_COLOUR=\x1b[33;01m
OK_STRING=$(OK_COLOUR)[OK]$(NO_COLOUR)
ERROR_STRING=$(ERROR_COLOUR)[ERROR]$(NO_COLOUR)
WARN_STRING=$(WARN_COLOUR)[WARNING]$(NO_COLOUR)

ODIR=obj
BINDIR=bin
PROJECT_PREFIX=

TARGET=placeholder

all: $(TARGET)

#alwaysrecompile:
#	@touch ./getcompileinfo.cpp

$(TARGET): alwaysrecompile $(shell ./listprojectobjects.sh "$(PROJECT_PREFIX)$(TARGET).cbp" obj)
	@printf %-76s "Linking $(TARGET)..."
	@mkdir -p $(BINDIR)
	+@$(CXX) $(LINKFLAGS) $(shell ./listprojectobjects.sh "$(PROJECT_PREFIX)$(TARGET).cbp" obj) $(LIBS) -o $(BINDIR)/$@ && $(ECHO) -e "$(OK_STRING)" || ($(ECHO) -e "$(ERROR_STRING)"; false)
	@$(ECHO) -en "Target $(TARGET): $(OK_COLOUR)Build completed successfully. $(NO_COLOUR)"

$(ODIR)/%.o: %.cpp
	@printf %-76s "Compiling $<..."
	@mkdir -p $(ODIR)/$(shell dirname $<)
	@$(CXX) $(CPPFLAGS) -c $< -o $@ && ($(ECHO) -e "$(OK_STRING)") || ($(ECHO) -e "$(ERROR_STRING)"; false)

placeholder:
	@$(ECHO) -e "$(ERROR_STRING) You must select a target with TARGET=whatever $(NO_COLOUR)"

.PHONY: all alwaysrecompile install clean rebuild placeholder

.DEFAULT: all

rebuild: clean all

clean:
	@printf %-76s "Cleaning $(ODIR)... "
	@rm -rf ./$(ODIR)/* && rm -rf $(DEPS) && $(ECHO) -e "$(OK_STRING)" || $(ECHO) -e "$(ERROR_STRING)"
