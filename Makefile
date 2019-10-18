subsystem:
	cd Server && $(MAKE) && cp FS ../
	cd User && $(MAKE) && cp user ../
	
clean:
	cd Server && $(MAKE) clean
	cd User && $(MAKE) clean
	rm -f FS user