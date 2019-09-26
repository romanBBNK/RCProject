subsystem:
	cd Server && $(MAKE)
	cd Client && $(MAKE)
	
clean:
	cd Server && $(MAKE) clean
	cd Client && $(MAKE) clean
