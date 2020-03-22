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
export ST_TARGET := lib$(TARGET_NAME)$(STO_END)
export SO_TARGET := lib$(TARGET_NAME)$(SO_END)
export TEST_TARGET := check

MODULES := tests

default: all
all: $(TARGET_NAME) $(TEST_TARGET)

## look for include files in each of the modules
CFLAGS += $(patsubst %,-Isrc/%, $(MODULES)) -Isrc -D_REENTRANT -g -Wall
SO_CFLAGS := -c -fPIC
LIBS :=
SO_LIBS := -ldl
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
ST_OBJ := $(patsubst %.c, %.sto, $(filter %.c, $(SRC)))
TOBJ := $(patsubst %.c, %.o, $(filter %.c, $(TSRC)))
SO_OBJ := $(patsubst %.c, %.o, $(filter %.c, $(SRC)))

# setting additional flags for recipe targets
$(TEST_TARGET): LIBS += $(TLIB)
$(TEST_TARGET): OBJ := $(filter-out src/main.%o, $(OBJ))

$(SO_OBJ): CFLAGS += $(SO_CFLAGS)
$(TOBJ): CFLAGS += $(TCFLAGS)

clean: OBJ += $(TOBJ)
clean: TARGET += $(TEST_TARGET)

####################
#     RECIPIES     #
####################
# compile recipes
$(ST_OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $^ -o $@

$(SO_OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $^ -o $@

# link recipes
$(TARGET_NAME): $(ST_TARGET) $(SO_TARGET)
	@echo "========== built libs =========="

$(ST_TARGET): $(ST_OBJ)
	@echo "========== static lib =========="
ifeq ($(TARGET_OS),windows)
	@echo "Windows builds are currently unsupported"
else
	ar rcs $(ST_TARGET) $(ST_OBJ)
endif

$(SO_TARGET): $(SO_OBJ)
	@echo "========== shared lib =========="
ifeq ($(TARGET_OS),linux)
	$(CC) -shared -Wl,-soname,$(SO_TARGET),-rpath=. -o $(SO_TARGET) $(SO_OBJ) $(SO_LIBS)
else ifeq ($(TARGET_OS),darwin)
	$(CC) -dynamiclib -install_name $(SO_TARGET) -o $(SO_TARGET) $(SO_OBJ) $(SO_LIBS)
else ifeq ($(TARGET_OS),windows)
	@echo "Windows builds are currently unsupported"
endif

$(TEST_TARGET): $(OBJ) $(TOBJ) $(TARGET)
	@echo "========== building $(TEST_TARGET) =========="
	$(CC) -o $@ $(OBJ) $(TOBJ) $(LIBS)
	./$(TEST_TARGET)

clean:
	-rm -f $(SO_TARGET)
	-rm -f $(ST_TARGET)
	-find . -name '*.o' -exec rm {} \;
	-find . -name '*.sto' -exec rm {} \;
	-find . -name '*.d' -exec rm {} \;

output:
	@echo "========== $(TARGET) =========="
	@echo "CFLAGS:"
	@echo "    $(CFLAGS)"
	@echo "LIBS:"
	@echo "    $(LIBS)"
	@echo "ST_OBJ:"
	@echo "    $(ST_OBJ)"
	@echo "SO_CFLAGS:"
	@echo "    $(CFLAGS) $(SO_CFLAGS)"
	@echo "SO_LIBS:"
	@echo "    $(SO_LIBS)"
	@echo "SO_OBJ:"
	@echo "    $(SO_OBJ)"
	@echo "========== $(TEST_TARGET) =========="
	@echo "CFLAGS:"
	@echo "    $(CFLAGS) $(TCFLAGS)"
	@echo "LIBS:"
	@echo "    $(LIBS) $(TLIB)"
	@echo "OBJ:"
	@echo "    $(OBJ) $(TOBJ)"

.PHONY: default all clean FORCE
