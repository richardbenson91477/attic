
OBJS=mm.o cam.o conf.o item.o map.o menu.o player.o tool.o s.o
INCS=cam.hpp conf.hpp item.hpp map.hpp menu.hpp mm.hpp player.hpp s.hpp tool.hpp
LIBS=../n1.a ../blaze/blaze.a ../img/img.a ../arc/arc.a ../file/file.a ../timer/timer.a ../dlog/dlog.a ../snd/snd.a ../math/math.a ../win/win.a
LDFLAGS=-lSDL2 -lSDL2_mixer -lGL -lm -lpng

all: lib_check mm

lib_check:
	+$(MAKE) -C .. n1.a

$(OBJS): $(INCS)

mm: $(OBJS) $(LIBS)
	$(CXX) -o $@ $^ $(LDFLAGS)

test: all
	cd data; ../mm 

clean:
	rm -f mm $(OBJS)

