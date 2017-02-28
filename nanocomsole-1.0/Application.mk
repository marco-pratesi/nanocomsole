APP_ABI := armeabi-v7a #all

# Workaround for MIPS toolchain linker being unable to find liblog dependency
# of shared object in NDK versions at least up to r9.
#
APP_LDFLAGS := -llog
