
OBJS=nnet.o
INCS=nnet.hpp
BINS=nnet.a nnet.so

all: $(BINS)
	+$(MAKE) -C tests/

$(OBJS): $(INCS)

nnet.a: $(OBJS)
	ar rs $@ $^

nnet.so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^

test: all
	+$(MAKE) -C tests/ test

clean:
	rm -f $(BINS) $(OBJS)
	+$(MAKE) -C tests/ clean

