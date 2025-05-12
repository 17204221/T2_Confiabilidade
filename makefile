
COMPILADOR = g++

TARGET = vigenere

SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(COMPILADOR) -o $(TARGET) $(SRC)

execute: $(TARGET)
	./$(TARGET) 4221-0.txt frequencia.txt

clean:
	rm -f $(TARGET) resultado.txt

