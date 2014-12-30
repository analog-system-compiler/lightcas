
SRC_DIR         = ../src
CXX             = $(CROSS_COMPILE)g++
AR              = $(CROSS_COMPILE)ar
SRCS            = $(notdir $(wildcard $(SRC_DIR)/*.cpp))
OBJDIR          = $(shell mkdir -p ./objs) ./objs
INCDIR          = -I$(SRC_DIR) -I$(SRC_DIR)/std 
EXE             = LightCAS
LIB             = liblightcas.a
EXE_SRC         = ../src/app/Console.cpp

IB_OBJS       = $(SRCS:%.cpp=$(OBJDIR)/%.o)

-include $(LIB_OBJS:.o=.d)

DEBUG_FLAGS = -g -D_DEBUG
GPROF_FLAGS = -pg

BUILD_CFLAGS  = $(CFLAGS) -MD -Wall $(INCDIR)
BUILD_CFLAGS += $(GPROF_FLAGS)
BUILD_CFLAGS += $(DEBUG_FLAGS)

all:$(EXE)

$(EXE):$(LIB)
	@echo  [Linking ... $(notdir $@)]
	$(CXX) -o $@  $(BUILD_CPPFLAGS) $(BUILD_CFLAGS) $(EXE_SRC) $(LIB_OBJS)

$(LIB): $(LIB_OBJS) 
	@echo  [Generating ... $(notdir $@)]
	$(AR) -r $@ $(LIB_OBJS)

$(OBJDIR)/%.o: $(SRC_DIR)/%.cpp
	@echo '[Compiling ...  $(notdir $< )]'
	$(CXX) $(BUILD_CPPFLAGS) $(BUILD_CFLAGS) -c $^ -o $@

clean:
	@echo [cleaning ..]
	rm -rf $(OBJDIR)

.PHONY: all
.SILENT:
