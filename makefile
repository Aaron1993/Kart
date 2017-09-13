.KEEP_STAT:

all: main

Compiler	= g++
FLAGS		= -D NDEBUG -O2 -m64 -msse4.1
LIB		= -lz -lm -lpthread
SOURCE		= main.cpp GetData.cpp Mapping.cpp AlignmentCandidates.cpp AlignmentRescue.cpp KmerAnalysis.cpp nw_alignment.cpp edlib_alignment.cpp ksw2_alignment.cpp tools.cpp bwt_index.cpp bwt_search.cpp
HEADER		= structure.h edlib.h
OBJECT		= $(SOURCE:%.cpp=%.o)

all:		main index

main:		$(OBJECT)
			$(Compiler) $(FLAGS) $(OBJECT) -o kart $(LIB)
			
static:		$(OBJECT)
			$(Compiler) $(FLAGS) -static $(OBJECT) -o kart_static $(LIB)

%.o:		%.cpp $(HEADER)
			$(Compiler) $(FLAGS) -c $<

index:
		make -C BWT_Index && mv BWT_Index/bwa_index .

clean:
		rm -f *.o *~
		
eva:		SamEvaulation.cpp
		$(Compiler) $(FLAGS) SamEvaulation.cpp -o eva
