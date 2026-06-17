include $(TOPDIR)/rules.mk

PKG_NAME:=flowmon
PKG_FORMAT:=ipk
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/flowmon
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=FlowMon - Conntrack Flow Monitor
  DEPENDS:=+libstdcpp6
endef

define Package/flowmon/description
  Simple C++ conntrack flow monitor
endef

define Build/Compile
	$(TARGET_CXX) $(TARGET_CXXFLAGS) \
		-std=c++17 \
		-o $(PKG_BUILD_DIR)/flowmon \
		./src/main.cpp
endef

define Package/flowmon/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/flowmon $(1)/usr/bin/
endef

$(eval $(call BuildPackage,flowmon))
