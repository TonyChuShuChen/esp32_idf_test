#
# Component Makefile
#
# This Makefile should, at the very least, just include $(SDK_PATH)/Makefile. By default, 
# this will take the sources in the src/ directory, compile them and link them into 
# lib(subdirectory_name).a in the build directory. This behaviour is entirely configurable,
# please read the SDK documents if you need to do this.
#

COMPONENT_EXTRA_CLEAN := test_tjpgd_logo.h

include $(SDK_PATH)/Makefile

test_tjpgd.o: test_tjpgd_logo.h

test_tjpgd_logo.h: $(COMPONENT_PATH)/logo.jpg
	$(vecho) XXD logo.jpg
	$(Q) cd $(COMPONENT_PATH); xxd -i logo.jpg $(COMPONENT_BUILD_DIR)/test_tjpgd_logo.h

