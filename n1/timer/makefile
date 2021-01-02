
OBJS=timer.o
INCS=timer.hpp
BINS=timer.a timer.so

all: $(BINS)

$(OBJS): $(INCS)

timer.a: $(OBJS)
	ar rs $@ $^

timer.so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^

test: all

clean:
	rm -f $(BINS) $(OBJS)

