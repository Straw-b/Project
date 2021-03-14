BIN=server
INCLUDE=-I ../../include -I ../../thirdparty/msgpack-c/include/
CFALGS=-lpthread -g

$(BIN):main.cpp
	g++ $^ -o $@ $(INCLUDE) $(CFALGS)

clean:
	rm $(BIN)
