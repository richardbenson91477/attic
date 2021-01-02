
OBJS=dlog.o
INCS=dlog.hpp
BINS=dlog.a dlog.so

all: $(BINS)

$(OBJS): $(INCS)

dlog.a: $(OBJS)
	ar rs $@ $^

dlog.so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^

test: all

clean:
	rm -f $(BINS) $(OBJS)

