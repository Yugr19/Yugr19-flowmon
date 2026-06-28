include $(TOPDIR)/rules.mk

PKG_NAME:=Tfmon-Lite
PKG_RELEASE:=2.0

include $(INCLUDE_DIR)/package.mk

define Package/Tfmon-Lite
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=TfMon - Conntrack Flow Monitor
  DEPENDS:=+libstdcpp
endef

define Package/Tfmon-Lite/description
  About
Simple C++ network speed flow monitor
endef

define Build/Compile
	$(TARGET_CXX) $(TARGET_CXXFLAGS) \
		-std=c++17 \
		-o $(PKG_BUILD_DIR)/Tfmon-Lite \
		./src/main.cpp
endef

define Package/Tfmon-Lite/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/Tfmon-Lite $(1)/usr/bin/
endef

$(eval $(call BuildPackage,Tfmon-Lite))
