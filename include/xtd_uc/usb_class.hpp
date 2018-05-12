#ifndef XTD_UC_USB_CLASS_HPP
#define XTD_UC_USB_CLASS_HPP
#include "common.hpp"
#include "cstdint.hpp"
#include "type_traits.hpp"

namespace xtd {
  // Taken from: http://www.usb.org/developers/defined_class
  enum class usb_class : uint8_t {
    use_interface_classes = 0x00,  // Only on device level
    audio = 0x01,
    comm_cdc = 0x02,
    hid = 0x03,
    physical = 0x05,
    image = 0x06,
    printer = 0x07,
    mass_storage = 0x08,
    hub = 0x09,  // Only on device level
    data_cdc = 0x0A,
    smart_card = 0x0B,
    content_security = 0x0D,
    video = 0x0E,
    personal_healthcare = 0x0F,
    audio_video = 0x10,
    billboard = 0x11,  // Only on device level
    usb_type_c_bridge = 0x12,
    diagnostic_device = 0xDC,
    wireless_controller = 0xE0,
    misc = 0xEF,
    application_specific = 0xFE,
    vendor_specific_class = 0xFF
  };

  // ---------------------------------------------------------------------------
  // HID Inteface (tautology, I know)
  // ---------------------------------------------------------------------------
  enum usb_subclass_hid : uint8_t { hid_default = 0x00, hid_boot = 0x01 };

  enum usb_protocol_hid : uint8_t { hid_none = 0x00, hid_keyboard = 0x01, hid_mouse = 0x02 };

  // ---------------------------------------------------------------------------
  // Mass Storage Inteface
  // ---------------------------------------------------------------------------
  enum usb_subclass_mass_storage : uint8_t {
    scsi_command_set_not_reported = 0x00,
    rbc = 0x01,
    atapi = 0x02,
    obsolete_qic157 = 0x03,
    ufi = 0x04,
    obsolete_sff8070i = 0x05,
    scs_transparent_command_set = 0x06,
    lsd_fs = 0x07,
    iee1668 = 0x08,
    vendor_specific_subclass = 0xFF
  };

  enum usb_protocol_mass_storage : uint8_t {
    cbi_cci = 0x00,
    cbi_no_cci = 0x01,
    bbb = 0x50,
    uas = 0x62,
    vendor_specific_protocol = 0xFF
  };

  // ---------------------------------------------------------------------------
  // Traits for the main classes
  // ---------------------------------------------------------------------------
  template <usb_class clazz>
  struct usb_class_allowed_on_device : false_type {};

  template <>
  struct usb_class_allowed_on_device<usb_class::use_interface_classes> : true_type {};
  template <>
  struct usb_class_allowed_on_device<usb_class::comm_cdc> : true_type {};
  template <>
  struct usb_class_allowed_on_device<usb_class::hub> : true_type {};
  template <>
  struct usb_class_allowed_on_device<usb_class::billboard> : true_type {};
  template <>
  struct usb_class_allowed_on_device<usb_class::diagnostic_device> : true_type {};
  template <>
  struct usb_class_allowed_on_device<usb_class::misc> : true_type {};
  template <>
  struct usb_class_allowed_on_device<usb_class::vendor_specific_class> : true_type {};

  template <usb_class clazz>
  struct usb_class_allowed_on_interface : true_type {};

  template <>
  struct usb_class_allowed_on_interface<usb_class::use_interface_classes> : false_type {};
  template <>
  struct usb_class_allowed_on_interface<usb_class::hub> : false_type {};
  template <>
  struct usb_class_allowed_on_interface<usb_class::billboard> : false_type {};

  // ---------------------------------------------------------------------------
  // Class specification triple
  // ---------------------------------------------------------------------------
  template <usb_class clazz_, uint8_t subclazz_ = 0, uint8_t protocol_ = 0>
  struct usb_class_spec {
    constexpr static usb_class clazz = clazz_;
    constexpr static uint8_t subclazz = subclazz_;
    constexpr static uint8_t protocol = protocol_;
  };

  // ---------------------------------------------------------------------------
  // Common class aliases
  // ---------------------------------------------------------------------------
  using usb_class_use_interface_class = usb_class_spec<usb_class::use_interface_classes>;

  using usb_class_hid_mouse_boot =
      usb_class_spec<usb_class::hid, usb_subclass_hid::hid_boot, usb_protocol_hid::hid_mouse>;
  using usb_class_hid_mouse =
      usb_class_spec<usb_class::hid, usb_subclass_hid::hid_default, usb_protocol_hid::hid_mouse>;
  using usb_class_hid_keyboard_boot =
      usb_class_spec<usb_class::hid, usb_subclass_hid::hid_boot, usb_protocol_hid::hid_keyboard>;
  using usb_class_hid_keyboard =
      usb_class_spec<usb_class::hid, usb_subclass_hid::hid_default, usb_protocol_hid::hid_keyboard>;

}  // namespace xtd

#endif
