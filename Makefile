TARGET = app.out



# --- Directories ---
SDL_DIR     = $(CURDIR)/external/SDL3
SDL_DIST    = $(SDL_DIR)/dist
TTF_DIR     = $(CURDIR)/external/SDL_ttf
TTF_DIST    = $(TTF_DIR)/dist
BIN2C_DIR   = $(CURDIR)/external/bin2c
BIN2C_DIST  = $(BIN2C_DIR)/dist
UTFCPP_DIR = $(CURDIR)/external/utfcpp

JSON_DIR = $(CURDIR)/external/json

# My Packages
PACKAGED_DATA_DIR = $(CURDIR)/packagedData
PACKAGED_DATA_DIST = $(PACKAGED_DATA_DIR)/dist


# --- Configuration (Default to Debug) ---
# Usage: 'make' for debug, 'make CONF=release' for release
CONF ?= debug

ifeq ($(CONF),release)
    # Release: Optimization ON, Debug info OFF, Strip unused code
    CXXFLAGS = -Os -ffunction-sections -fdata-sections -s -flto -MMD \
		-static-libstdc++ -static-libgcc
    LDFLAGS  = -Wl,--gc-sections -static-libstdc++ -static-libgcc -flto
else
    # Debug: Optimization OFF, Debug info ON
    CXXFLAGS = -g -O0 -MMD
    LDFLAGS  = 
endif

# --- Compiler & Linker Flags ---
CXXFLAGS += -I$(SDL_DIST)/include -I$(TTF_DIST)/include -I$(CURDIR)/src -I$(UTFCPP_DIR)/source \
			-I$(JSON_DIR)/include
# Add both lib paths with -L and link both libs
#LDFLAGS  += -L$(SDL_DIST)/lib -L$(TTF_DIST)/lib -Wl, -Bstatic -rpath,$(SDL_DIST)/lib:$(TTF_DIST)/lib -lSDL3 -lSDL3_ttf -lm -lpthread -ldl
#LDFLAGS += -L$(SDL_DIST)/lib -L$(TTF_DIST)/lib \
#            -Wl,-Bstatic  -rpath,$(SDL_DIST)/lib:$(TTF_DIST)/lib  \
#            -lSDL3  -lSDL3_ttf  \
#           -Wl,-Bdynamic -lm -lpthread -ldl
#LDFLAGS += -L$(SDL_DIST)/lib -L$(TTF_DIST)/lib \
#           -Wl,-Bstatic -lSDL3 -lSDL3_ttf\
#           -Wl,-Bdynamic \
#           -Wl,-rpath,$(SDL_DIST)/lib:$(TTF_DIST)/lib 
##           -lm -lpthread -ldl

#Can do this on windows but i think for linux i want dynaimc libgcc and stuff
#LDFLAGS += -static -L$(SDL_DIST)/lib -L$(TTF_DIST)/lib \
#           -lSDL3 -lSDL3_ttf -lm -lpthread -ldl
LDFLAGS += -L$(SDL_DIST)/lib -L$(TTF_DIST)/lib \
           -Wl,-Bstatic -lSDL3 -lSDL3_ttf\
           -Wl,-Bdynamic \
           -lm -lpthread -ldl \
           -lX11 -lXext -lXcursor -lXrender -lXfixes -lXi -lXrandr



# --- Targets ---
all: \
	$(BIN2C_DIST) \
	$(SDL_DIST) \
	$(TTF_DIST) \
	$(PACKAGED_DATA_DIST) \
	$(TARGET)

# Build bin2c
$(BIN2C_DIST):
	cd $(BIN2C_DIR) && make


# Build SDL3
$(SDL_DIST):
	cmake -S $(SDL_DIR) -B $(SDL_DIR)/build -DCMAKE_INSTALL_PREFIX=$(SDL_DIST) -DSDL_X11_XTEST=OFF \
		-DBUILD_SHARED_LIBS=OFF \
		-DCMAKE_BUILD_TYPE=Release
	cmake --build $(SDL_DIR)/build -j$(shell nproc) --target install

# Build SDL_TTF
$(TTF_DIST): $(SDL_DIST)
	cmake -S $(TTF_DIR) -B $(TTF_DIR)/build \
		-DSDLTTF_VENDORED=ON \
		-DSDLTTF_HARFBUZZ=ON \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_INSTALL_PREFIX=$(TTF_DIST) \
		-DCMAKE_PREFIX_PATH=$(SDL_DIST) \
		-DSDL3_DIR=$(SDL_DIST) \
		-DBUILD_SHARED_LIBS=OFF
	cmake --build $(TTF_DIR)/build -j$(shell nproc) --target install

#TODO just put bin2c as part of the packaged data make file
$(PACKAGED_DATA_DIST): $(BIN2C_DIST)
	@echo Packaged Data is compiling!!!
	cd $(PACKAGED_DATA_DIR) && make
	@echo Compiled packaged data

.PHONY: $(TARGET)

$(TARGET):
	@mkdir -p build
	$(CXX) $(PACKAGED_DATA_DIST)/packagedData.o src/main.cpp -o $(TARGET) $(CXXFLAGS) $(LDFLAGS)
#strip $(TARGET)

# --- Files ---
# Recursive search for all source files
#SRCS = $(shell find $(SRC_DIR) -name "*.cpp")

# Map them to the object directory
#OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

#app.out: main.cpp
#	mkdir build
#	$(CXX) main.cpp -o app.out $(CXXFLAGS) $(LDFLAGS)


# Link step
#$(TARGET): $(OBJS) $(PACKAGED_DATA_DIST)
#	mkdir -p build
#	@echo "Linking $(TARGET)..."
#	$(CXX) \
#		$(OBJS) $(PACKAGED_DATA_DIST)/packagedData.o \
#		-o $(TARGET) $(LDFLAGS)

# Compilation step (compile each .cpp to .o)
#$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
#	@mkdir -p $(@D)
#	@echo "Compiling $<"
#	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create obj directory if it doesn't exist
#$(OBJ_DIR):
#	mkdir -p $(OBJ_DIR)

clean:
	rm -f app.out
	rm -rf $(SDL_DIR)/dist $(TTF_DIR)/dist
	rm -rf $(SDL_DIR)/build $(TTF_DIR)/build
	cd $(BIN2C_DIR) && make clean
	cd $(PACKAGED_DATA_DIR) && make clean
	rm -rf $(CURDIR)/build


#Handles header files changing i think

# Find all the .d files that have been created so far
#DEPS = $(OBJS:.o=.d)

# Include them! The "-" at the start prevents errors if the files don't exist yet
#-include $(DEPS)