CC = gcc
CFLAGS = 
objectname = ReIPServer GetIPClient
#objects := $(foreach name,$(objectname),$(name).o)

#sources := $(foreach name,$(objectname),$(name).c)

.PHONY : TARGET
TARGET : $(objectname)
 
 $(objectname) : %:%.c   	#$(objects)
	$(CC) $< $(CFLAGS) -o $@
	
.PHONY : clean
clean :
	-rm  $(objectname) 		
	




