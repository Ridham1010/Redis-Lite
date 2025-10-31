CXX = g++
CXXFLAGS = -std=c++17 -pthread -Wall -MMD -MP -O2

# Server directories
SRC_DIR = src
BUILD_DIR = build

# Client directories
CLIENT_DIR = Redis-Client
CLIENT_BUILD_DIR = build-client

# Server files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Client files
CLIENT_SRCS := $(wildcard $(CLIENT_DIR)/*.cpp)
CLIENT_OBJS := $(patsubst $(CLIENT_DIR)/%.cpp,$(CLIENT_BUILD_DIR)/%.o,$(CLIENT_SRCS))

# Targets
SERVER_TARGET = redis-lite
CLIENT_TARGET = redis-cli

# Build both by default
all: $(SERVER_TARGET) $(CLIENT_TARGET)

# Server build rules
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(SERVER_TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(SERVER_TARGET)

# Client build rules
$(CLIENT_BUILD_DIR):
	mkdir -p $(CLIENT_BUILD_DIR)

$(CLIENT_BUILD_DIR)/%.o: $(CLIENT_DIR)/%.cpp | $(CLIENT_BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) $(CLIENT_OBJS) -o $(CLIENT_TARGET)

# Individual targets
server: $(SERVER_TARGET)
client: $(CLIENT_TARGET)

# Run targets
run-server: $(SERVER_TARGET)
	./$(SERVER_TARGET)

run-client: $(CLIENT_TARGET)
	./$(CLIENT_TARGET)

# Clean
clean:
	rm -rf $(BUILD_DIR) $(CLIENT_BUILD_DIR) $(SERVER_TARGET) $(CLIENT_TARGET)

rebuild: clean all

.PHONY: all server client run-server run-client clean rebuild


