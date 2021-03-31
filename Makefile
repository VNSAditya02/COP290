#
# TODO: Include all TODOs here
#

CC := g++ # This is the main compiler

SRCDIR := src
BUILDDIR := build
TARGET := bin/runner

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS:= -g # -Wall
CFLAGSOPT := -g -O3 -flto -march=native -mtune=knl # -Wall
LIB := -pthread -std=c++11 `pkg-config --cflags --libs opencv` -lboost_program_options -lstdc++fs
INC := -I include

all: part1 part2 part3 part4 angle density

task1: angle

task2: density

task3: part1 part2 part3 part4

part1: $(SRCDIR)/Partc_Method_1.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $< -o $(BUILDDIR)/Partc_Method_1 $(CFLAGS) $(LIB)"; $(CC) $< -o $(BUILDDIR)/Partc_Method_1 $(CFLAGS) $(LIB)

part2: $(SRCDIR)/Partc_Method_2.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $< -o $(BUILDDIR)/Partc_Method_2 $(CFLAGS) $(LIB)"; $(CC) $< -o $(BUILDDIR)/Partc_Method_2 $(CFLAGS) $(LIB)

part3: $(SRCDIR)/Partc_Method_3.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $< -o $(BUILDDIR)/Partc_Method_3 $(CFLAGS) $(LIB)"; $(CC) $< -o $(BUILDDIR)/Partc_Method_3 $(CFLAGS) $(LIB)

part4: $(SRCDIR)/Partc_Method_4.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $< -o $(BUILDDIR)/Partc_Method_4 $(CFLAGS) $(LIB)"; $(CC) $< -o $(BUILDDIR)/Partc_Method_4 $(CFLAGS) $(LIB)

angle: $(SRCDIR)/angle_correction.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $< -o $(BUILDDIR)/angle_correction $(CFLAGS) $(LIB)"; $(CC) $< -o $(BUILDDIR)/angle_correction $(CFLAGS) $(LIB)

density: $(SRCDIR)/traffic_density.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $< -o $(BUILDDIR)/traffic_density $(CFLAGS) $(LIB)"; $(CC) $< -o $(BUILDDIR)/traffic_density $(CFLAGS) $(LIB)

.PHONY: clean

clean:
	@echo " Cleaning...";
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)