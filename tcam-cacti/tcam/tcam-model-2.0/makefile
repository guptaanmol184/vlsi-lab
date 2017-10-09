LIBS = -lm

tcam_cell_width = 4.33    # in um 
tcam_cell_height = 4.05   # in um

#FLAGS = -DTCAMBitWidth=$(tcam_cell_width)  -DTCAMBitHeight=$(tcam_cell_height) 

SRCS = main.c tcam-power.c leakage.c itrs_interconnect.c

OBJS = main.o tcam-power.o leakage.o itrs_interconnect.o

all: tcam-power

tcam-power : $(OBJS) 
	  gcc $(FLAGS) $(OBJS) -o tcam-power $(LIBS)

main.o : main.c  tcam-power.h
	  gcc $(FLAGS) -c main.c -o main.o

tcam-power.o :  tcam-power.c tcam-power.h itrs_interconnect.h
	  gcc $(FLAGS) -c tcam-power.c -o tcam-power.o

leakage.o :  leakage.c leakage.h 
	  gcc $(FLAGS) -c leakage.c -o leakage.o

itrs_interconnect.o :  itrs_interconnect.c itrs_interconnect.h tcam-power.h
	  gcc $(FLAGS) -c itrs_interconnect.c -o itrs_interconnect.o

clean:
	  rm *.o tcam-power

