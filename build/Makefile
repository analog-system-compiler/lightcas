
EXE         = LightCAS
SRC_DIR     = ../src
RULES_DIR   = ../rules
OBJDIR      = $(shell mkdir -p ./objs ./objs/nostd ) ./objs
LIB         = lib$(EXE).a
EXE_SRC     = ../src/app/Console.cpp
LIB_OBJS    = $(SRCS:%.cpp=$(OBJDIR)/%.o)
INCDIR      = -I$(SRC_DIR)
RULE_FILES  = $(addprefix $(RULES_DIR)/, Symbols.txt Rules.txt RulesElectricity.txt )

#Compiler settings
CXX         = $(CROSS_COMPILE)g++
#CXX         = $(CROSS_COMPILE)clang++
AR          = $(CROSS_COMPILE)ar
LD          = $(CROSS_COMPILE)ld

#options
USE_STD     = 0
DEBUG       = 0
TEST        = 1
EMBED_RULES = 1

CPPFLAGS  = -MMD -Wall -fno-rtti -fno-exceptions $(INCDIR)
LDFLAGS   = -s -Wl,-Map=$(EXE).map

SRCS = \
    Debug.cpp \
    Element.cpp \
    ElementDataBase.cpp \
    Evaluator.cpp \
    Function.cpp \
    MathExpression_core.cpp \
    MathExpression_display.cpp \
    MathExpression_parser.cpp \
    MathExpression_simplify.cpp \
    Parser.cpp \
    Test.cpp \
    Value.cpp

ifeq ($(USE_STD),0)
	CPPFLAGS += -I$(SRC_DIR)/nostd 
	SRCS   += nostd/LCString.cpp  
else
	CPPFLAGS += -I$(SRC_DIR)/std
endif

ifeq ($(DEBUG),1)
	CPPFLAGS += -g -D_DEBUG
else
	CPPFLAGS += -Os
endif

ifeq ($(TEST),1)
	CPPFLAGS += -D_TEST
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

$(OBJDIR)/rules_tmp.txt : $(RULE_FILES)
	rm -f $@
	cat $^ >> $@
	echo -e '\x00' >> $@

$(OBJDIR)/Rules.o: $(OBJDIR)/rules_tmp.txt
	echo 'Packaging  $(notdir $< )'	
	$(LD) -r -b binary $< -o $@

$(LIB): $(LIB_OBJS)
	echo 'Archiving  $(notdir $@)'
	$(AR) rcs $@ $^
    
$(EXE): $(EXE_SRC) $(LIB)
	echo 'Linking    $(notdir $@)'
	$(CXX) -o $@  $(CPPFLAGS) $(LDFLAGS) $^
ifeq ($(EMBED_RULES),0)
	cp ../src/*.txt .
endif

clean:
	@echo 'Cleaning  ...'
	rm -rf $(LIB) $(EXE) $(OBJDIR)

.PHONY: all
.SILENT:
