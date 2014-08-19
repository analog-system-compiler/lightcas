
TOP_DIR         = ../src
CXX             = $(CROSS_COMPILE)g++
SRCDIRS         = $(shell (find  $(TOP_DIR) -type d \( -name CVS -o -name build \) -prune -o -type d ))
SRCS_W_DIRS     = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.cpp))
SRCS            = $(notdir $(SRCS_W_DIRS))
OBJDIR          = $(shell mkdir -p ./objs) ./objs
EXE             = LightCAS

vpath %.cpp    ${SRCDIRS}
vpath %.o      $(OBJDIR)
LIB_OBJS       = $(SRCS:%.cpp=${OBJDIR}/%.o)

-include $(LIB_OBJS:.o=.d)

DEBUG_FLAGS = -g -D_DEBUG
GPROF_FLAGS = -pg

BUILD_CFLAGS  = $(CFLAGS) -MD -Wall
BUILD_CFLAGS += $(GPROF_FLAGS)
BUILD_CFLAGS += $(DEBUG_FLAGS)

all: $(EXE)

$(EXE):$(LIB_OBJS)
	@echo  [Linking   ... $(notdir $@)]
	$(CXX) -o $@  $(BUILD_CPPFLAGS) $(BUILD_CFLAGS) ${LIB_OBJS}

$(OBJDIR)/%.o: %.cpp
	@echo '[Compiling ...  $(notdir $< )]'
	$(CXX) $(BUILD_CPPFLAGS) $(BUILD_CFLAGS) -c $^ -o $@

clean:
	@echo [cleaning ..]
	rm -rf $(OBJDIR)/*

.PHONY: all
.SILENT:
