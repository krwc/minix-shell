all: mshell
clean: 
	rm mshell ; \
        make -C src clean

mshell: $(OBJS) $(LIBS)
	make -C src ; cp src/mshell .

