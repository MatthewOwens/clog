TLIB += `pkg-config --libs check`
TCFLAGS += -I$(TESTS_DIR)
TCFLAGS += `pkg-config --cflags check`
TSRC += $(wildcard $(TESTS_DIR)/*.c)
