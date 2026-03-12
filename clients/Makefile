CXX = clang++
CXXFLAGS = -std=c++17 -Wall -I ./include

# Updated Source paths
BOT_SRC = ./bot/bot.cpp
PLAYER_SRC = ./player/player.cpp

# Output directory and binaries
OUTDIR = ./bin
BOT_OUT = $(OUTDIR)/bot
PLAYER_OUT = $(OUTDIR)/player

# Default target builds both
all: $(OUTDIR) $(BOT_OUT) $(PLAYER_OUT)

# Build the AI Bot
$(BOT_OUT): $(BOT_SRC)
	$(CXX) $(CXXFLAGS) -o $(BOT_OUT) $(BOT_SRC)

# Build the Manual player
$(PLAYER_OUT): $(PLAYER_SRC)
	$(CXX) $(CXXFLAGS) -o $(PLAYER_OUT) $(PLAYER_SRC)

# Create bin directory if it doesn't exist
$(OUTDIR):
	mkdir -p $(OUTDIR)

# Cleanup
clean:
	rm -rf $(OUTDIR)

.PHONY: all clean