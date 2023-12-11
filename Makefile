
MAKEFLAGS = --jobs 4

EXE         = asysc
SRC_DIR     = src
APP_DIR     = app
RULES_DIR   = rules
OBJDIR      = $(shell mkdir -p ./objs ./objs/nostd ./objs/asysc ) ./objs
LIB         = lib$(EXE).a
EXE_OBJ     = $(EXE_SRC:%.cpp=$(OBJDIR)/%.o)
LIB_OBJS    = $(OBJ_SRC:%.cpp=$(OBJDIR)/%.o)
INCDIR      = -I$(SRC_DIR)
RULE_FILES  = $(addprefix $(RULES_DIR)/, $(wildcard *.txt) )

#Compiler settings
CXX         = $(CROSS_COMPILE)g++
#CXX         = $(CROSS_COMPILE)clang++
AR          = $(CROSS_COMPILE)ar
LD          = $(CROSS_COMPILE)ld

#options
USE_STD     = 0
DEBUG       = 0
TEST        = 0
EMBED_RULES = 0

CPPFLAGS  = -MMD -Wall -fno-rtti -fno-exceptions $(INCDIR)
LDFLAGS   = -Wl,-Map=$(EXE).map

EXE_SRC = \
	asysc/ASysC.cpp \
	asysc/MathExpressionEx.cpp

OBJ_SRC = \
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

ifeq ($(GPROF),1)
	CPPFLAGS += -pg
endif

ifeq ($(USE_STD),0)
	CPPFLAGS += -I$(SRC_DIR)/nostd 
	OBJ_SRC  += nostd/LCString.cpp  
else
	CPPFLAGS += -I$(SRC_DIR)/std
endif

ifeq ($(DEBUG),1)
	CPPFLAGS += -g -D_DEBUG
else
	CPPFLAGS += -Os
	LDFLAGS  += -s
endif

ifeq ($(TEST),1)
	CPPFLAGS += -D_TEST
endif

ifeq ($(EMBED_RULES),1)
	CPPFLAGS += -DEMBED_RULES
	LIB_OBJS += $(OBJDIR)/Rules.o
endif

all:$(EXE) rules

-include $(LIB_OBJS:.o=.d)
-include $(EXE_OBJ:.o=.d)

$(OBJDIR)/%.o: $(APP_DIR)/%.cpp
	echo 'Compiling  $(notdir $< )'
	$(CXX) $(CPPFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRC_DIR)/%.cpp
	echo 'Compiling  $(notdir $< )'
	$(CXX) $(CPPFLAGS) -c $< -o $@

$(OBJDIR)/rules_tmp.txt : $(RULE_FILES)
	printf "%s\0" "$$(cat $^)" > $@

$(OBJDIR)/Rules.o: $(OBJDIR)/rules_tmp.txt
	echo 'Packaging  $(notdir $< )'	
	$(LD) -r -b binary $< -o $@

$(LIB): $(LIB_OBJS)
	echo 'Archiving  $(notdir $@)'
	$(AR) rcs $@ $^

$(EXE): $(EXE_OBJ) $(LIB)
	echo 'Linking    $(notdir $@)'
	$(CXX) -o $@ $(LDFLAGS) $(EXE_OBJ) $(LIB)

rules:
ifeq ($(EMBED_RULES),0)
	cp $(RULES_DIR)/*.txt .
endif

clean:
	@echo 'Cleaning  ...'
	rm -rf $(LIB) $(EXE) $(OBJDIR)

archive:
	cd ..; mkdir -p archive; git ls-files | xargs zip archive/lightcas_`date +%y%m%d`.zip

.PHONY: all clean rules archive
.SILENT:
#.SECONDARY:
