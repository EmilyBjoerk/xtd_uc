#ifndef XTD_UC_USB_DESCRIPTION_HPP
#define XTD_UC_USB_DESCRIPTION_HPP
// This file contains the necessary data structures to describe your USB device.
// NOTE: These are not the USB descriptors themselves, but input to generate
// them.
//
// These datastructures are defined as templates and are used to construct the
// USB descriptors to send to the host as well as configure the hardware to
// match your endpoints etc. Because they are templates they will not occupy any
// RAM.
//
// Typically you generate class descriptions (usb_class.hpp), then endpoints,
// then interfaces containing endpoints, then configurations containing
// interfaces and then finally a device descriptor containing the configurations
// and the control endpoint. The USB descriptor tree is instantiated with
// usb_build_descriptors<...>() from (usb_device.hpp), this builds the device
// descriptor as a datastructure that can be directly transmitted to the host.
// The user is required to do this step in order to chose if they want to put
// the descriptor in RAM or PROGMEM. The instantiated descriptor object is then
// passed to usb_create_device(_p)<...>() to create the device object which must
// be in RAM. Please refer to usb_device.hpp for further documentation.
//

#include "common.hpp"
#include "cstdint.hpp"
#include "tuple.hpp"

namespace xtd {
  enum usb_transfer_dir : uint8_t { out = 0x0, in = 0x1 };

  enum usb_transfer_type : uint8_t {
    control = 0b00,
    isochronous = 0b01,
    bulk = 0b10,
    interrupt = 0b11,
  };

  enum usb_sync_type : uint8_t {
    no_sync = 0b00,
    async = 0b01,
    adaptive = 0b10,
    sync = 0b11,
  };

  enum usb_usage_type : uint8_t {
    data = 0b00,
    feedback = 0b01,
    explicit_feedback = 0b10,
  };

  enum usb_hw_flags : uint8_t { none = 0, double_buffer = 1 };

  enum usb_config_flags : uint8_t {
    bus_powered = (1 << 7),
    self_powered = (1 << 6),
    remote_wakeup = (1 << 5)
  };

  constexpr uint16_t semver_to_bcd(uint8_t major, uint8_t minor, uint8_t patch) {
    return (uint16_t(major) << 8) | ((minor & 0xF) << 4) | (patch & 0xF);
  }

  enum usb_bcd : uint16_t {
    USB_1_0 = semver_to_bcd(1, 0, 0),
    USB_1_1 = semver_to_bcd(1, 1, 0),
    USB_2_0 = semver_to_bcd(2, 0, 0)
  };

  // These traits describe an USB endpoint.
  //
  // After much debate I have chose to implement this as a template instead of a straight up
  // struct. The motivation given below:
  //
  // * It allows definition of compile time constants that are different per endpoint but
  //   that the compiler may remove in the optimization step.
  // * The above allows a split of data between what is directly transferred as part of a
  //   descriptor request from the USB host and data that is used to configure the hardware.
  //   However it also keeps the data together in the same class. This is important because
  //   it allows to easily store the descriptor data in program space while accessing the constants
  //   normally because they are not part of the structs memory layout.
  // * It allows separation of describing the endpoint and instantiating it in memory in the
  //   user's source code.
  //
  // Caveat: It is important to not declare any functions inside of this template as they will
  // be instantiated once per endpoint which is suboptimal from a program space usage point
  // of view.
  template <usb_transfer_type type_,  // If control, only max_packet_size and dual_bank needed
            uint16_t packet_size_,    // Only 8, 16, 32 or 64 supported on AVR
            uint8_t hw_flags_,        // Bitmap of usb_hw_flags to use.
            usb_transfer_dir dir_ = usb_transfer_dir::out,  // Out for control endpoints
            uint8_t interval_ = 1,  // Must be '1' for isochronouse, or [1,255] for interrupt
            usb_sync_type sync_ = usb_sync_type::no_sync,  // Only for isochronous
            usb_usage_type usage_ = usb_usage_type::data   // Only for isochronous
            >
  struct usb_endpoint_desc {
  public:
    constexpr static usb_transfer_type type = type_;
    constexpr static usb_transfer_dir direction = dir_;
    constexpr static uint16_t max_packet_size = packet_size_;

    constexpr static usb_sync_type sync = sync_;
    constexpr static usb_usage_type usage = usage_;
    constexpr static uint8_t polling_interval = interval_;

    // Describe HW requirements
    constexpr static uint8_t hw_flags = hw_flags_;
  };

  template <typename usb_class_type, const char* descr, typename... if_endpoints>
  struct usb_interface_desc {
  public:
    using endpoints_tuple = tuple<if_endpoints...>;
    using interface_class = usb_class_type;
    constexpr static const char* description = descr;
  };

  template <const char* descr, uint8_t flags, int max_current_mA, typename... ifs>
  struct usb_config_desc {
    using interfaces = tuple<ifs...>;
    constexpr static const char* description = descr;
    constexpr static uint8_t max_current = max_current_mA;
  };

  template <uint16_t usb_ver, typename usb_class_type, uint16_t vendor_id_, uint16_t product_id_,
            uint16_t product_ver_, const char* mfg_name, const char* product_name_,
            const char* serial, typename control_ep, typename... configs>
  struct usb_device_desc {
    using device_class = usb_class_type;
    using configurations = tuple<configs...>;
    using control_endpoint = control_ep;

    constexpr static uint16_t usb_version = usb_ver;
    constexpr static uint16_t vendor_id = vendor_id_;
    constexpr static uint16_t product_id = product_id_;
    constexpr static uint16_t product_ver = product_ver_;

    constexpr static const char* vendor_name = mfg_name;
    constexpr static const char* product_name = product_name_;
    constexpr static const char* product_serial = product_serial;
  };

}  // namespace xtd

#endif
