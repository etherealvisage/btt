.PHONY: all redep clean distclean

all:
	$(MAKE) -C build

redep:
	mkdir -p build ; cd build ; cmake ..

clean:
	$(MAKE) -C build clean

distclean:
	rm -rf build
