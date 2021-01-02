BINS=win.a win.so 
OBJS=win.o
INCS=win.hpp 
LIBS=../dlog/dlog.so ../blaze/blaze.so -lSDL2  -lGL

all: $(BINS)

$(OBJS): $(INCS)

win.a: $(OBJS)
	ar rs $@ $^

win.so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^ $(LIBS)

test: all

clean:
	rm -f $(OBJS) $(BINS)

