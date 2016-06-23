.KEEP_STAT:

all: main

Compiler	= g++
FLAGS		= -D NDEBUG -O3 -m64
LIB		= -lm -lpthread
SOURCE		= main.cpp GetData.cpp Mapping.cpp AlignmentCandidates.cpp AlignmentRescue.cpp KmerAnalysis.cpp PairwiseAlignment.cpp tools.cpp bwt_index.cpp bwt_search.cpp
HEADER		= structure.h
OBJECT		= $(SOURCE:%.cpp=%.o)

main:		$(OBJECT)
			$(Compiler) $(FLAGS) $(OBJECT) -o kart $(LIB)
%.o:		%.cpp $(HEADER)
			$(Compiler) $(FLAGS) -c $<
clean:
		rm -f *.o *~
eva:
		$(Compiler) $(FLAGS) SamEvaulation.cpp -o eva
