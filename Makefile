
all:
	$(MAKE) -C module
	$(MAKE) -C test

clean:
	$(MAKE) -C  module clean
	$(MAKE) -C  test clean
