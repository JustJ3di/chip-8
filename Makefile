TARGET = emulatore_chip8_sdl


CXX = g++


CXXFLAGS = -Wall -Wextra -std=c++17 -pedantic

SRCS = main.cpp chip8.cpp
OBJS = $(SRCS:.cpp=.o)


SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LIBS = $(shell sdl2-config --libs)


CFLAGS_FINAL = $(CXXFLAGS) $(SDL_CFLAGS)


LDFLAGS_FINAL = $(SDL_LIBS)


$(TARGET): $(OBJS)
	@echo "Linking..."
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS_FINAL)
	@echo "Compilazione completata! Eseguibile: $(TARGET)"


%.o: justj3di/chip-8/chip-8-main/%.cpp
	@echo "Compiling $<..."
	$(CXX) -c $< -o $@ $(CFLAGS_FINAL)


.PHONY: clean
clean:
	@echo "Pulizia in corso..."
	rm -f $(OBJS) $(TARGET)
	@echo "Pulizia completata."


.PHONY: run
run: $(TARGET)
	@echo "Esecuzione di $(TARGET) (aggiungi la ROM dopo 'make run ROM=...')"
	@if [ -n "$(ROM)" ]; then \
		./$(TARGET) $(ROM); \
	else \
		echo "ERRORE: Devi specificare la ROM. Esempio: make run ROM=percorso/alla/tua.ch8"; \
	fi