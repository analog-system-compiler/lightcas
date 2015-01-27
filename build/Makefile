
SRC_DIR         = ../src
CXX             = $(CROSS_COMPILE)g++
AR              = $(CROSS_COMPILE)ar
SRCS            = $(notdir $(wildcard $(SRC_DIR)/*.cpp))
OBJDIR          = $(shell mkdir -p ./objs ./objs/nostd ) ./objs
INCDIR          = -I$(SRC_DIR) 
EXE             = LightCAS
LIB             = liblightcas.a
EXE_SRC         = ../src/app/Console.cpp

#options
USE_STD         = 0
DEBUG           = 0

ifeq ($(USE_STD),0)
  INCDIR += -I$(SRC_DIR)/nostd 
  SRCS += nostd/LCString.cpp  
else
  INCDIR += -I$(SRC_DIR)/std 
endif

LIB_OBJS       = $(SRCS:%.cpp=$(OBJDIR)/%.o)
-include $(LIB_OBJS:.o=.d)

GPROF_FLAGS   = -pg
BUILD_CFLAGS  = $(CFLAGS) -MD -Wall $(INCDIR)

ifeq ($(DEBUG),1)
  BUILD_CFLAGS += -g -D_DEBUG $(GPROF_FLAGS)
else
  BUILD_CFLAGS += -s -Os
endif

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
