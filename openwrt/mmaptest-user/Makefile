include $(TOPDIR)/rules.mk

PKG_NAME:=mmaptest-user
PKG_VERSION:=1
PKG_RELEASE:=1

PKG_MAINTAINER:=daniel danzberger <daniel@dd-wrt.com>

include $(INCLUDE_DIR)/package.mk

define Package/mmaptest-user
  CATEGORY:=Extra
  TITLE:=mmaptest-user
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)"
endef

define Package/mmaptest-user/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mmaptest-user $(1)/usr/bin/
endef

$(eval $(call BuildPackage,mmaptest-user))

