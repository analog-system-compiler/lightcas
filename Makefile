
MAKEFLAGS = --jobs 4

USE_CLANG   = 1
EXE         = asysc
LIB_DIR     = src
APP_DIR     = app
RULES_DIR   = rules
OBJDIR      = $(shell mkdir -p ./objs ./objs/nostd ./objs/asysc ) ./objs
LIB         = lib$(EXE).a
EXE_OBJ     = $(APP_SRC:%.cpp=$(OBJDIR)/%.o)
LIB_OBJS    = $(LIB_SRC:%.cpp=$(OBJDIR)/%.o)
INCDIR      = -I$(LIB_DIR)
RULE_FILES  = $(wildcard $(RULES_DIR)/*.txt)
CPP_FILES   = $(addprefix $(LIB_DIR)/, $(LIB_SRC) )  $(addprefix $(APP_DIR)/, $(APP_SRC) ) 

#Compiler settings
ifeq ($(USE_CLANG),1)
	CXX = $(CROSS_COMPILE)clang++
	LD  = $(CROSS_COMPILE)ld
	AR  = $(CROSS_COMPILE)ar
else
	CXX = $(CROSS_COMPILE)g++
	LD  = $(CROSS_COMPILE)ld
	AR  = $(CROSS_COMPILE)ar
endif

#options
USE_STD     = 1
DEBUG       = 0
TEST        = 0
EMBED_RULES = 0
GPROF       = 0

ifeq ($(USE_STD),0)
	INCDIR += -I$(LIB_DIR)/nostd 
else
	INCDIR += -I$(LIB_DIR)/std
endif


CPPFLAGS  = -MMD -Wall -fno-rtti -fno-exceptions $(INCDIR)
LDFLAGS   = -Wl,-Map=$(EXE).map

APP_SRC = \
	asysc/ASysC.cpp \
	asysc/MathExpressionEx.cpp

LIB_SRC = \
    Debug.cpp \
	Parser.cpp \
    Element.cpp \
    ElementDataBase.cpp \
    Function.cpp \
    MathExpression_core.cpp \
    MathExpression_display.cpp \
    MathExpression_parser.cpp \
    MathExpression_simplify.cpp \
	Value.cpp \
	Evaluator.cpp \
    Test.cpp
	
ifeq ($(USE_STD),0)
	LIB_SRC  += nostd/LCString.cpp  
endif

ifeq ($(DEBUG),1)
	CPPFLAGS += -g -D_DEBUG
else ifeq ($(GPROF),1)
	CPPFLAGS += -pg
	LDFLAGS  += -pg
else	
	CPPFLAGS += -O3
	LDFLAGS  += -s
endif

ifeq ($(TEST),1)
	CPPFLAGS += -D_TEST
endif

ifeq ($(EMBED_RULES),1)
	CPPFLAGS += -DEMBED_RULES
	LIB_OBJS += $(OBJDIR)/Rules.o
endif

all: $(EXE) rules

-include $(LIB_OBJS:.o=.d)
-include $(EXE_OBJ:.o=.d)

$(OBJDIR)/%.o: $(APP_DIR)/%.cpp
	echo 'Compiling  $(notdir $< )'
	$(CXX) $(CPPFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(LIB_DIR)/%.cpp
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

rules: $(RULE_FILES)
ifeq ($(EMBED_RULES),0)
	echo 'Copying    $(notdir $^)'
	cp $(RULE_FILES) .
endif

clean:
	@echo 'Cleaning  ...'
	rm -rf $(LIB) $(EXE) $(OBJDIR)

archive:
	cd ..; mkdir -p archive; git ls-files | xargs zip archive/lightcas_`date +%y%m%d`.zip

cppcheck:
	cppcheck --enable=all --suppress=missingIncludeSystem --suppress=noExplicitConstructor $(INCDIR) $(CPP_FILES)

clang-tidy:
	clang-tidy $(CPP_FILES) -- $(INCDIR)

.PHONY: all clean rules archive
.SILENT:
#.SECONDARY:
