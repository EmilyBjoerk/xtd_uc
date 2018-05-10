#ifndef XTD_UC_USB_DESCRIPTOR_HPP
#define XTD_UC_USB_DESCRIPTOR_HPP
#include "common.hpp"

namespace xtd {
  enum class usb_descriptor_type : uint8_t {
    device = 0x01,
    configuration = 0x02,
    string = 0x03,
    interface = 0x04,
    endpoint = 0x05
  };

  enum class usb_transfer_dir : uint8_t { out = 0x0, in = 0x1 };

  enum class usb_transfer_type : uint8_t {
    control = 0b00,
    isochronous = 0b01,
    bulk = 0b10,
    interrupt = 0b11,
  };

  enum class usb_sync_type : uint8_t {
    no_sync = 0b00,
    async = 0b01,
    adaptive = 0b10,
    sync = 0b11,
  };

  enum class usb_usage_type : uint8_t {
    data = 0b00,
    feedback = 0b01,
    explicit_feedback = 0b10,
  };

  // Taken from: http://www.usb.org/developers/defined_class
  enum class usb_device_class : uint8_t {
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
    vendor_specific = 0xFF
  };

  constexpr uint16_t semver_to_bcd(uint8_t major, uint8_t minor, uint8_t patch) {
    return (uint16_t(major) << 8) | ((minor & 0xF) << 4) | (patch & 0xF);
  }

  enum class usb_bcd : uint16_t { USB_1_0 = 0x0100, USB_1_1 = 0x0110, USB_2_0 = 0x0200 };

  template <typename CRTP, usb_descriptor_type type>
  struct usb_descriptor_base {
    uint8_t length = sizeof(CRTP);  // Length of the descriptor.
    usb_descriptor_type descriptor_type = type;
  };

  // Describes an USB endpoint.
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
  template <usb_transfer_type type,   // If control, only max_packet_size and dual_bank needed
            uint16_t packet_size,     // Only 8, 16, 32 or 64 supported on AVR
            bool dual_bank,           // true to use two banks for transmissions on AVR
            uint8_t endpoint_nr = 0,  // 0 for control, must match hw endpoint nr, nr < 8.
            uint8_t interval = 1,     // Must be '1' for isochronouse, or [1,255] for interrupt
            usb_transfer_dir dir = usb_transfer_dir::out,  // Out for control endpoints
            usb_sync_type sync = usb_sync_type::no_sync,   // Only for isochronous
            usb_usage_type usage = usb_usage_type::data    // Only for isochronouse
            >
  struct usb_descriptor_endpoint
      : usb_descriptor_base<usb_descriptor_endpoint, usb_descriptor_type::endpoint> {
  public:
    constexpr static usb_direction direction = dir;
    constexpr static bool avr_dual_bank = dual_bank;

  public:
    // Bits 0..3b endpoint nr. Bits 4..6b reserved (zero). Bits 7 direction 0/1=Out/In.
    uint8_t endpoint_address = (endpoint_nr & 0x3) | (dir == usb_transfer_dir::in ? 0x80 : 0x00);

    // Bits 0..1b transfer type. Bits 2..3 sync type. Bits 4..5 usage type.
    uint8_t attributes = type | (sync << 2) | (usage << 4);

    // AVR HW only supports 8,16,32 or 64 packet sizes AFAICT.
    uin16_t max_packet_size = packet_size;

    // Isochronour endpoints must set this to 1, interrupts may set [1,255] frames, others zero
    uint8_t polling_interval = interval;

    static_assert(sizeof(decltype(*this)) == 7, "Interface descriptor must be 7 bytes!");
  };

  // Describes an USB interface.
  //
  //
  template <uint8_t nr, usb_device_class clazz, uint8_t sub_clazz, typename... if_endpoints>
  struct usb_descriptor_interface
      : usb_descriptor_base<usb_descriptor_interface, usb_descriptor_type::interface> {
  public:
    using endpoints_tuple = tuple<if_endpoints...>;

  public:
    uint8_t interface_number = nr;
    uint8_t alternate_setting = 0;
    uint8_t num_endpoints = endpoints_tuple::size();
    uint8_t interface_class = clazz;
    uint8_t interface_sub_class = sub_clazz;
    uint8_t interface_protocol;
    uint8_t description_string_index;

    endpoints_tuple endpoints;

    static_assert(sizeof(decltype(*this)) == 9, "Interface descriptor must be 9 bytes!");
  };

  struct usb_descriptor_device
      : usb_descriptor_base<usb_descriptor_device, usb_descriptor_type::device> {
    usb_bcd bcd_usb = usb_bcd::USB_2_0;  // USB Specification number the device complies too.
    uint8_t device_class = 0x00;         // Interfaces identify their own classes.
    uint8_t device_sub_class = 0x00;     // -||-
    uint8_t device_protocol = 0x00;      // -||-
    uint8_t max_packet_size;             // Max packet size for zero endpoint (8,16,32 or 64)
    uint16_t id_vendor;
    uint16_t id_product;
    uint16_t bcd_device;
    uint8_t manufacturer_index;
    uint8_t product_index;
    uint8_t serial_number_index;
    uint8_t num_configurations = 1;

    static_assert(sizeof(decltype(*this)) == 18, "Device descriptor must be 18 bytes!");
  };

  struct usb_descriptor_configuration
      : usb_descriptor_base<usb_descriptor_configuration, usb_descriptor_type::configuration> {
    implement this
  };

}  // namespace xtd

#endif
