# This option enables -fsanitize=address for stage2 and stage3.

POSTSTAGE1_LDFLAGS += -B$$r/prev-$(TARGET_SUBDIR)/libsanitizer/asan/.libs
