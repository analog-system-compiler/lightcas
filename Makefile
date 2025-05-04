
MAKEFLAGS = --jobs 4

EXE         = asysc
BIN_DIR     = $(shell mkdir -p bin) bin
LIB_DIR     = src
APP_DIR     = app
RULES_DIR   = rules
OBJ_DIR     = $(shell mkdir -p objs objs/nostd objs/asysc ) objs
LIB         = lib$(EXE).a
EXE_OBJ     = $(APP_SRC:%.cpp=$(OBJ_DIR)/%.o) $(OBJ_DIR)/asysc/help.o
LIB_OBJS    = $(LIB_SRC:%.cpp=$(OBJ_DIR)/%.o)
INCDIR      = -I$(LIB_DIR)
RULE_FILES  = $(addprefix $(RULES_DIR)/, $(shell awk -F '<|>' '$$1~/`include/ {print $$2}' $(RULES_DIR)/includes.rule))
CPP_FILES   = $(addprefix $(LIB_DIR)/, $(LIB_SRC) )  $(addprefix $(APP_DIR)/, $(APP_SRC) ) 
SRC_FILES   = $(shell find . -name "*.cpp"; find . -name "*.h"; find . -name "*.txt")
LDFLAGS     = 

#options
USE_CLANG   ?= 0
USE_STD     ?= 1
DEBUG       ?= 0
TEST        ?= 0
EMBED_RULES ?= 0
GPROF       ?= 0

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

ifeq ($(USE_STD),0)
	INCDIR += -I$(LIB_DIR)/nostd 
else
	INCDIR += -I$(LIB_DIR)/std
endif


CPPFLAGS = -MMD -Wall -fno-rtti -fno-exceptions $(INCDIR)

APP_SRC = \
	asysc/asysc.cpp \
	asysc/mathexpressionex.cpp

LIB_SRC = \
    display.cpp \
	parser.cpp \
    element.cpp \
    elementdatabase.cpp \
    function.cpp \
    mathexpression_core.cpp \
    mathexpression_display.cpp \
    mathexpression_parser.cpp \
    mathexpression_simplify.cpp \
	value.cpp \
	evaluator.cpp \
    test.cpp
	
ifeq ($(USE_STD),0)
	LIB_SRC  += nostd/LCString.cpp  
endif

ifeq ($(DEBUG),1)
	CPPFLAGS += -g -D_DEBUG
	#LDFLAGS  += -Wl,-Map=$(EXE).map
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
	LIB_OBJS += $(OBJ_DIR)/Rules_concat.o
endif

all: $(BIN_DIR)/$(EXE) rules

-include $(LIB_OBJS:.o=.d)
-include $(EXE_OBJ:.o=.d)

$(OBJ_DIR)/%.o: $(APP_DIR)/%.cpp
	echo 'Compiling  $(notdir $< )'
	$(CXX) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(LIB_DIR)/%.cpp
	echo 'Compiling  $(notdir $< )'
	$(CXX) $(CPPFLAGS) -c $< -o $@

$(APP_DIR)/Rules_concat.txt: $(RULE_FILES)
	cat $^ > $@

$(OBJ_DIR)/%.ztxt: $(APP_DIR)/%.txt
	cp $< $@
	echo -e '\0' >> $@

$(OBJ_DIR)/%.o: $(OBJ_DIR)/%.ztxt
	echo 'Packaging  $(notdir $< )'	
	$(LD) -r -b binary $< -o $@
	#objdump -t $@

$(LIB): $(LIB_OBJS)
	echo 'Archiving  $(notdir $@)'
	$(AR) rcs $@ $^

$(BIN_DIR)/$(EXE): $(EXE_OBJ) $(LIB)
	echo 'Linking    $(notdir $@)'
	$(CXX) -o $@ $(LDFLAGS) $(EXE_OBJ) $(LIB)

rules: $(RULE_FILES)
ifeq ($(EMBED_RULES),0)
	echo 'Copying    $(notdir $^)'
	cp -r $(RULES_DIR) $(BIN_DIR)
endif

clean:
	@echo 'Cleaning  ...'
	rm -rf $(LIB) $(BIN_DIR) $(OBJ_DIR) $(EXE_OBJ)

archive:
	cd ..; mkdir -p archive; git ls-files | xargs zip archive/lightcas_`date +%y%m%d`.zip

cppcheck:
	cppcheck --enable=all --suppress=missingIncludeSystem --suppress=noExplicitConstructor $(INCDIR) $(CPP_FILES)

clang-tidy:
	clang-tidy $(CPP_FILES) -- $(INCDIR)

header:	
	insert-license --license-filepath=LICENSE --use-current-year --comment-style "/*| *| */" $(SRC_FILES); sed -i -e 's|C:/msys64||' $(SRC_FILES)	
	
.PHONY: all clean rules archive cppcheck clang-tidy header
.SILENT:
#.SECONDARY:
