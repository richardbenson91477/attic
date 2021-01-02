
NOBJ=n1_var.o n1_conf.o n1_disp.o n1_app.o n1_obj.o n1_ptcl.o n1_phys.o n1_cell.o n1_rgn.o n1_texshader.o
NINC=n1.hpp n1_var.hpp n1_conf.hpp n1_disp.hpp n1_app.hpp n1_obj.hpp n1_ptcl.hpp n1_phys.hpp n1_cell.hpp n1_rgn.hpp n1_texshader.hpp

all: n1.a
	+$(MAKE) -C mm
	+$(MAKE) -C pushem

n1.a: $(NOBJ)
	ar rs n1.a $(NOBJ)

$(NOBJ): $(NINC)

test: all
	+$(MAKE) -C pushem/ test

clean:	
	rm -f $(NOBJ) n1.a
	+$(MAKE) -C mm clean
	+$(MAKE) -C pushem clean

