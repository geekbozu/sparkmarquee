export APP=SPARKMARQUEE

%::
	@echo $(MAKECMDGOALS)
	$(MAKE) -C ../../build -f makefile $(MAKECMDGOALS)
