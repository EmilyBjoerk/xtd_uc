#ifndef XTD_UC_USB_DEVICE_IMPL_HPP
#define XTD_UC_USB_DEVICE_IMPL_HPP
#include "common.hpp"
#include "cstdint.hpp"
#include "tmpl.hpp"
#include "usb_class.hpp"
#include "usb_description.hpp"

namespace xtd {
  namespace detail {

    using tmpl::counter;
    using tmpl::increment;

    enum usb_descriptor_type : uint8_t {
      device = 0x01,
      configuration = 0x02,
      string = 0x03,
      interface = 0x04,
      endpoint = 0x05
    };

    template <uint8_t size, usb_descriptor_type type>
    struct usb_descriptor_base {
      uint8_t length = size;
      usb_descriptor_type descriptor_type = type;
    };

    template <typename E, typename count>
    struct usb_descriptor_endpoint : usb_descriptor_base<7, usb_descriptor_type::endpoint> {
      static_assert(count::index < 5, "AVR only supports 5 endpoints");

      using num_ep = increment<count>;

      uint8_t endpoint_address = count::index | (E::direction == usb_transfer_dir::in ? 0x80 : 0);
      uint8_t attributes = E::type | (E::sync << 2) | (E::usage << 4);
      uint16_t max_packet_size = E::max_packet_size;
      uint8_t polling_interval = E::polling_interval;
    };

    template <int size, typename tuple, typename ep_count>
    struct endpoint_list {
      using next = endpoint_list<size - 1, tuple, ep_count>;
      using endpoint_type =
          usb_descriptor_endpoint<decltype(get<size>(tuple())), typename next::num_endpoints>;
      using num_endpoints = increment<typename next::num_endpoints>;

      next n;
      endpoint_type endpoint;
    };

    template <typename tuple, typename ep_count>
    struct endpoint_list<0, tuple, ep_count> {
      using endpoint_type = usb_descriptor_endpoint<decltype(get<0>(tuple())), ep_count>;
      using num_endpoints = increment<ep_count>;

      endpoint_type endpoint = endpoint_type();
    };

    template <typename I, typename ep_count, typename if_count, typename string_list>
    struct usb_descriptor_interface : usb_descriptor_base<9, usb_descriptor_type::interface> {
      static_assert(usb_class_allowed_on_interface<I::interface_class::clazz>::value,
                    "The given device class is not allowed on interfaces by the USB spec!");

      using num_if = increment<if_count>;
      using description_string = tmpl::string_list_add<string_list, I::description>;
      using endpoints_type =
          endpoint_list<I::endpoints_tuple::size(), typename I::endpoints_tuple, ep_count>;

      using total_endpoints = typename endpoints_type::num_endpoints;

      const uint8_t interface_number = if_count::index;
      const uint8_t alternate_setting = 0;
      const uint8_t num_endpoints = I::endpoints_tuple::size();
      const uint8_t interface_class = I::interface_class::clazz;
      const uint8_t interface_sub_class = I::interface_class::sub_clazz;
      const uint8_t interface_protocol = I::interface_class::protocol;
      const uint8_t description_string_index = description_string::index;

      const endpoints_type endpoints;
    };

    template <typename C, typename string_list_head>
    struct usb_descriptor_configuration
        : usb_descriptor_base<usb_descriptor_configuration<C, string_list_head>,
                              usb_descriptor_type::configuration> {
    private:
      using description_string = usb_string_list<C::description, string_list_head>;

    public:
      uint16_t total_length = sizeof(usb_descriptor_configuration);
      uint8_t num_interfaces = C::ifs::size();
      uint8_t config_value;
      uint8_t description_index = description_string::index;
      uint8_t attributes;
      uint8_t max_current_mA = C::max_current;

      using string_list_head = ...;
    };

    template <typename D>
    struct usb_descriptor_device
        : usb_descriptor_base<usb_descriptor_device<D>, usb_descriptor_type::device> {
    private:
      static_assert(usb_class_allowed_on_device<D::device_clazz::clazz>::value,
                    "The given device class is not allowed on devices by the USB spec!");

      using vendor_string = usb_string_list<D::vendor_name>;
      using product_string = usb_string_list<D::product_name, vendor_string>;
      using serial_string = usb_string_list<D::product_serial, product_string>;

      using configs = configuration_list<D::configurations, serial_string>;

    public:
      constexpr usb_descriptor_device() = default;

      const usb_bcd bcd_usb = D::usb_version;
      const uint8_t device_class = clazz::device_class;
      const uint8_t device_sub_class = clazz::device_subclass;
      const uint8_t device_protocol = clazz::device_protocol;
      const uint8_t max_packet_size = D::control_endpoint::max_packet;
      const uint16_t vendor_id = D::vendor_id;
      const uint16_t product_id = D::product_id;
      const uint16_t product_ver_bcd = D::product_ver;
      const uint8_t manufacturer_index = vendor_string::index;
      const uint8_t product_index = product_string::index;
      const uint8_t serial_number_index = serial_string::index;
      const uint8_t num_configurations = D::configurations::size();

      const configs::data_type = configs::data_type();

      using string_list_head = configs::string_list;

      static_assert(sizeof(usb_descriptor_device) == 18, "Device descriptor must be 18 bytes!");
    };
  }  // namespace detail
}  // namespace xtd

#endif
