VPATH = /usr/include/c++/4.8
CXXFLAGS = -std=c++0x

objectname = ForOffer Function StackToQueQ7 testGlobal
objects := $(foreach name,$(objectname),$(name).o)

sources := $(foreach name,$(objectname),$(name).cpp)

ForOffer : $(objects)
	g++ -std=c++0x -o ForOffer $(objects)



#create reliant .h for .c

-include $(sources:.cpp=.d)

%.d: %.cpp
	@set -e;\
		rm -f $@;\
		g++ -MM $(CXXFlags) $< > $@.$$$$;\
		sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@;\
		rm -f $@.$$$$



#ForOffer.o : Function.h  StackToQueQ7.h iostream stdio.h cstddef exception
#Function.o :exception stdexcept  Function.h stack iostream
#StackToQueQ7.o : stack exception StackToQueQ7.h 


.PHONY : clean
clean :
	-rm ForOffer $(objects)
	




