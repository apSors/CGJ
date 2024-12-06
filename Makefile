DIR := src

debug : TARGET := debug
debug : all

release : TARGET := release
release : all

all:
	cd mgl && $(MAKE) $(TARGET)
	cd $(DIR) && $(MAKE) $(TARGET)

clean:
	cd mgl && $(MAKE) clean
	cd $(DIR) && $(MAKE) clean

run:
	cd $(DIR) && $(MAKE) run
