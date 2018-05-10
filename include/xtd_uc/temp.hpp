using endpoint_zero = ...;

using mouse_ep1 = usb_endpoint<usb_direction::in, 0, usb_transfer_attribute::bulk, 64,
                               usb_memory::double_port, on_mouse_in>;

using mouse_interface =
    usb_interface<usb_class::hid, usb_sub_class::none, 0, "Mouse HID", ep0, ep1, ...>;

using configuration = usb_configuration<"Default Config", usb_config_attributes::bus_powered,
                                        500_mA, mouse_interface>;

using device =
    usb_device<endpoint_zero, uint16_t vendor_id, uint16_t product_id, uint16_t device_bcd,
               "manufacturer", "product", "serial number", configuration>;

constexpr device descriptor_root PROGMEM;



int main(){

    usb_configure(descriptor_root);

    usb_enable();
    usb_attach();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

template <usb_class clazz, usb_sub_class sub_clazz, uint8_t interface_nr, const char* description,
          class... endpoints>
struct usb_interface {};

template <const char* description, usb_config_attributes attributes, power pwr, class... interfaces>
struct usb_configuration {};

template <class ep0, uint16_t vendor_id, uint16_t product_id, uint16_t version_bcd, const char* mfg,
          const char* product, const char* serial, class... configuration>
struct usb_device {};
