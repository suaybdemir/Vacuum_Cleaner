# Define the compiler
CXX = g++

# Define the target executable
TARGET = output

# Define the source file
SRC = main.cpp

# Rule to build the executable
$(TARGET): $(SRC)
	$(CXX) $(SRC) -o $(TARGET)

# Clean rule to remove the generated executable
clean:
	rm -f $(TARGET)
