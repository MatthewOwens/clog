####################
#   OS DETECTION   #
####################
# ensuring we have defaults for TARGET_OS and TARGET_NAME
ifndef $(TARGET_OS)
TARGET_OS := linux
endif
ifndef $(TARGET_NAME)
TARGET_NAME := clogs
endif

# checking for env. variables
ifeq ($(TARGET_OS), linux)
EXE_END :=
SO_END := .so
STO_END := .a
CC := gcc
endif
ifeq ($(TARGET_OS), darwin)
EXE_END :=
SO_END := .dylib
STO_END := .a
CC := clang
endif
ifeq ($(TARGET_OS), windows)
$(error windows builds are currently unsupported)
EXE_END := .exe
SO_END := .dll
STO_END := .lib
CC := gcc
endif

####################
#    TARGET VARS   #
####################
# exporting sane defaults for our various targets
#export TARGET := lib$(TARGET_NAME)$(STO_END)
export TARGET := $(TARGET_NAME)
export TEST_TARGET := check

MODULES := tests

default: all
all: $(TARGET) $(TEST_TARGET)

# base cflags, libs and srcs, used by ecs module
## look for include files in each of the modules
CFLAGS += $(patsubst %,-Isrc/%, $(MODULES)) -Isrc -D_REENTRANT -g -Wall
LIBS :=
SRC :=

# extra cflags, libs and srcs for each of our modules/submodules
## TEST_TARGET
TCFLAGS :=
TLIBS :=
TSRC :=

# module code directories, required by submodules
export TESTS_DIR=tests
export CLOG_DIR=src

# include the description for each module
include module.mk
include $(patsubst %, %/module.mk, $(MODULES))

# determining our object files
OBJ := $(patsubst %.c, %.o, $(filter %.c, $(SRC)))
TOBJ := $(patsubst %.c, %.o, $(filter %.c, $(TSRC)))

# include the C include deps
include $(OBJ:.o=.d)

# setting additional flags for recipe targets
$(TEST_TARGET): LIBS += $(TLIB)
$(TEST_TARGET): OBJ := $(filter-out src/main.o, $(OBJ))

$(OBJ): CFLAGS += $(ECS_MDEFS)
$(TOBJ): CFLAGS += $(TCFLAGS)

clean: OBJ += $(TOBJ)
clean: TARGET += $(TEST_TARGET)

####################
#     RECIPIES     #
####################
# link recipes
$(TARGET): $(OBJ) $(EXAMPLE_SYSTEMS) $(COMP_TARGET)
	@echo "========== building $(TARGET) =========="
	#ar rcs $(TARGET) $(OBJ)
	$(CC) -o $@ $(OBJ) $(LIBS)
	./$(TARGET)

$(TEST_TARGET): $(OBJ) $(TOBJ) $(TARGET)
	@echo "========== building $(TEST_TARGET) =========="
	$(CC) -o $@ $(OBJ) $(TOBJ) $(LIBS)
	./$(TEST_TARGET)


# calculate the C include deps
%.d: %.c
	./depend.sh `dirname $*.c` $(CFLAGS) $*.c > $@

clean:
	-rm -f $(TARGET)
	-find . -name '*.o' -exec rm {} \;
	-find . -name '*.d' -exec rm {} \;

output:
	@echo "========== $(TARGET) =========="
	@echo "CFLAGS:"
	@echo "    $(CFLAGS) $(ECS_MDEFS)"
	@echo "LIBS:"
	@echo "    $(LIBS)"
	@echo "OBJ:"
	@echo "    $(OBJ)"
	@echo "========== $(TEST_TARGET) =========="
	@echo "CFLAGS:"
	@echo "    $(CFLAGS) $(TCFLAGS)"
	@echo "LIBS:"
	@echo "    $(LIBS) $(TLIB)"
	@echo "OBJ:"
	@echo "    $(OBJ) $(TOBJ)"

.PHONY: default all clean FORCE
