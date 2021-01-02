
BINS=blaze.so blaze.a
OBJS=blaze.o 
INCS=blaze.hpp

all: $(BINS)

$(OBJS): $(INCS)

blaze.a: $(OBJS)
	ar rs $@ $(OBJS)

blaze.so: $(OBJS)
	$(CXX) -shared -o $@ $(OBJS)

test: all

clean:
	rm -f $(OBJS) $(BINS)

