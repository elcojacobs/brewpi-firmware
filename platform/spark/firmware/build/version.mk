
VERSION_STRING = 0.7.0-rc.6

# PRODUCT_FIRMWARE_VERSION reported by default
# FIXME: Unclear if this is used, PRODUCT_FIRMWARE_VERSION defaults to 65535 every release
VERSION = 205

CFLAGS += -DSYSTEM_VERSION_STRING=$(VERSION_STRING)
