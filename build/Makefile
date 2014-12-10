
SRC_DIR         = ../src
CXX             = $(CROSS_COMPILE)g++
SRCS            = $(notdir $(wildcard $(SRC_DIR)/*.cpp))
OBJDIR          = $(shell mkdir -p ./objs) ./objs
INCDIR          = $(SRC_DIR)/std 
EXE             = LightCAS

vpath %.cpp    $(SRC_DIR)
vpath %.o      $(OBJDIR)
LIB_OBJS       = $(SRCS:%.cpp=${OBJDIR}/%.o)

-include $(LIB_OBJS:.o=.d)

DEBUG_FLAGS = -g -D_DEBUG
GPROF_FLAGS = -pg

BUILD_CFLAGS  = $(CFLAGS) -MD -Wall -I$(INCDIR)
BUILD_CFLAGS += $(GPROF_FLAGS)
BUILD_CFLAGS += $(DEBUG_FLAGS)

all: $(EXE)

$(EXE):$(LIB_OBJS)
	@echo  [Linking ... $(notdir $@)]
	$(CXX) -o $@  $(BUILD_CPPFLAGS) $(BUILD_CFLAGS) ${LIB_OBJS}

$(OBJDIR)/%.o: %.cpp
	@echo '[Compiling ...  $(notdir $< )]'
	$(CXX) $(BUILD_CPPFLAGS) $(BUILD_CFLAGS) -c $^ -o $@

clean:
	@echo [cleaning ..]
	rm -rf $(OBJDIR)/*

.PHONY: all
.SILENT:
