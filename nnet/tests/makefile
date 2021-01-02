
BINS=nn_train nn_map
LIBS=../nnet.a

all: $(BINS)

$(BINS): $(LIBS)

test: all
	./train_and_plot sin

clean:
	rm -f $(BINS) *.o

