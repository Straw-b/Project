BIN=client
INCLUDE=-I ../../include -I ../../thirdparty/msgpack-c/include/
CFALGS=-lpthread

$(BIN):main.cpp
	g++ $^ -o $@ $(INCLUDE) $(CFALGS)

clean:
	rm $(BIN)
