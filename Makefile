CXX=g++
CXX_FLAGS=-Wall -std=c++0x -g3
BIN=predict

$(BIN): $(BIN).o params.o xvokra00.o network.o
	$(CXX) $(CXX_FLAGS) -lm  $^ -o $@

run: $(BIN)
	./$(BIN)

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

clean:
	rm -rf $(BIN) *.o *.net 2>/dev/null
