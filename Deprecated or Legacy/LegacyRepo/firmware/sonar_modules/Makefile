# Top-level Makefile for building and installing all sonar modules

# Find all sonar directories
SONAR_DIRS := $(wildcard sonar*)

# Define the kernel build directory
KDIR := /lib/modules/$(shell uname -r)/build

# Define object files
obj-m := $(foreach dir,$(SONAR_DIRS),$(dir)/$(dir).o)

# Default target
all: $(SONAR_DIRS)

# Build each sonar module
$(SONAR_DIRS):
	$(MAKE) -C $(KDIR) M=$(PWD)/$@ modules

# Install each sonar module
install: all
	@for dir in $(SONAR_DIRS); do \
		sudo insmod $$dir/$$dir.ko; \
	done

# Clean all sonar modules
clean:
	@for dir in $(SONAR_DIRS); do \
		$(MAKE) -C $(KDIR) M=$(PWD)/$$dir clean; \
	done

# Remove each sonar module
remove:
	@for dir in $(SONAR_DIRS); do \
		sudo rmmod $$dir; \
	done

.PHONY: all clean install remove $(SONAR_DIRS)
