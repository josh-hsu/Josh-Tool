###############################################################################
# JoshTool mandatory packages

PRODUCT_PACKAGES += \
	JoshToolApp \
	joshtool

PRODUCT_COPY_FILES += \
    vendor/mumu/rootdir/init.mumu.rc:root/init.mumu.rc

PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.language=zh \
    persist.sys.region=TW \
    persist.sys.country=TW



