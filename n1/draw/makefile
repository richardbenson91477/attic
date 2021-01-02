
BINS=draw.a draw.so
OBJS=draw.o
INCS=draw.hpp

all: $(BINS)

$(OBJS): $(INCS)

draw.a: $(OBJS)
	ar rs $@ $^

draw.so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^

test: all

clean:
	rm -f $(OBJS) $(BINS)

