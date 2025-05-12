
COMPILADOR = g++

TARGET = vigenere

SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(COMPILADOR) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
