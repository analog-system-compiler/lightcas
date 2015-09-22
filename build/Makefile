
EXE             = LightCAS
SRC_DIR         = ../src
CXX             = $(CROSS_COMPILE)g++
AR              = $(CROSS_COMPILE)ar
LD              = $(CROSS_COMPILE)ld
SRCS            = $(notdir $(wildcard $(SRC_DIR)/*.cpp))
OBJDIR          = $(shell mkdir -p ./objs ./objs/nostd ) ./objs
LIB             = lib$(EXE).a
EXE_SRC         = ../src/app/Console.cpp
LIB_OBJS        = $(SRCS:%.cpp=$(OBJDIR)/%.o)
INCDIR          = -I$(SRC_DIR)

#options
USE_STD         = 0
DEBUG           = 0
EMBED_RULES     = 1

ifeq ($(USE_STD),0)
	INCDIR += -I$(SRC_DIR)/nostd 
	SRCS   += nostd/LCString.cpp  
else
	INCDIR += -I$(SRC_DIR)/std 
endif

CPPFLAGS  = -MMD -Wall $(INCDIR)

ifeq ($(DEBUG),1)
	CPPFLAGS += -g -D_DEBUG -pg 
	LDFLAGS  += -Wl,-Map=$(EXE).map
else
	CPPFLAGS += -s -Os
endif

ifeq ($(EMBED_RULES),1)
	CPPFLAGS  += -DEMBED_RULES
	LIB_OBJS  += $(OBJDIR)/Rules.o
endif

all:$(EXE)

-include $(LIB_OBJS:.o=.d)

$(OBJDIR)/%.o: $(SRC_DIR)/%.cpp
	echo 'Compiling  $(notdir $< )'
	$(CXX) $(CPPFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRC_DIR)/%.txt
	echo 'Packaging  $(notdir $< )'
	cp $< $(OBJDIR)
	echo -e '\x00' >> $(OBJDIR)/$(notdir $< )
	$(LD) -r -b binary $(OBJDIR)/$(notdir $< ) -o $@ 

$(LIB): $(LIB_OBJS)
	echo 'Archiving  $(notdir $@)'
	$(AR) -r $@ $^

$(EXE):$(LIB)
	echo 'Linking    $(notdir $@)'
	$(CXX) -o $@  $(CPPFLAGS) $(LDFLAGS) $(EXE_SRC) $^
ifeq ($(EMBED_RULES),0)	
	cp ../src/Rules.txt .
endif

clean:
	@echo 'Cleaning  ...'
	rm -rf $(OBJDIR)

.PHONY: all
.SILENT:
