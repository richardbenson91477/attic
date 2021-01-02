
OBJS=ray.o
INCS=ray.hpp
BINS=ray.a ray.so

all: $(BINS)
	+$(MAKE) -C tests/

$(OBJS): $(INCS)

ray.a: $(OBJS)
	ar rs $@ $^

ray.so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^

test: all
	+$(MAKE) -C tests/ test

clean:
	rm -f $(BINS) $(OBJS)
	+$(MAKE) -C tests/ clean

