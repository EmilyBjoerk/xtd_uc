
namespace xtd {
  enum usb_descriptor_type : uint8_t {
    device = 0x01,
    configuration = 0x02,
    string = 0x03,
    interface = 0x04,
    endpoint = 0x05
  };
  template <typename CRTP, usb_descriptor_type type>
  struct usb_descriptor_base {
    uint8_t length = sizeof(CRTP);  // Length of the descriptor.
    usb_descriptor_type descriptor_type = type;
  };

  /*
  template <typename traits>
  struct usb_descriptor_endpoint
      : usb_descriptor_base<usb_descriptor_endpoint<traits>, usb_descriptor_type::endpoint> {
  public:
    constexpr static usb_transfer_dir direction = dir;
    constexpr static bool avr_dual_bank = dual_bank;

  public:
    // Bits 0..3b endpoint nr. Bits 4..6b reserved (zero). Bits 7 direction 0/1=Out/In.
    uint8_t endpoint_address = (endpoint_nr & 0x3) | (dir == usb_transfer_dir::in ? 0x80 : 0x00);

    // Bits 0..1b transfer type. Bits 2..3 sync type. Bits 4..5 usage type.
    uint8_t attributes = type | (sync << 2) | (usage << 4);

    // AVR HW only supports 8,16,32 or 64 packet sizes AFAICT.
    uint16_t max_packet_size = packet_size;

    // Isochronour endpoints must set this to 1, interrupts may set [1,255] frames, others zero
    uint8_t polling_interval = interval;

    static_assert(sizeof(usb_descriptor_endpoint) == 7, "Interface descriptor must be 7 bytes!");
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
*/

}
