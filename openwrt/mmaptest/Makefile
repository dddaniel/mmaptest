include $(TOPDIR)/rules.mk

PKG_NAME:=mmaptest
PKG_VERSION:=1
PKG_RELEASE:=1

PKG_MAINTAINER:=daniel danzberger <daniel@dd-wrt.com>

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/mmaptest
  CATEGORY:=Extra
  TITLE:=mmaptest
  FILES:=\
     $(PKG_BUILD_DIR)/mmaptest.ko
  AUTOLOAD:=$(call AutoProbe,mmaptest)
endef

define Build/Compile
	$(MAKE) $(PKG_JOBS) -C "$(LINUX_DIR)" \
		$(KERNEL_MAKE_FLAGS) \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		NOSTDINC_FLAGS="$(NOSTDINC_FLAGS)" \
		modules
endef

define Package/mmaptest/install
	true
endef

$(eval $(call KernelPackage,mmaptest))

