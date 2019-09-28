subsystem:
	cd Server && $(MAKE)
	cd User && $(MAKE)
	
clean:
	cd Server && $(MAKE) clean
	cd User && $(MAKE) clean
