#include "xtd_uc/usb_description.hpp"

#include "xtd_uc/usb_class.hpp"

#include <gtest/gtest.h>

using namespace xtd;
#define PROG_MEM

// -----------------------------------------------------------------------------
// USB descriptor strings
// -----------------------------------------------------------------------------
constexpr char if_desc[] PROG_MEM = "My USB Drive";
constexpr char config_desc[] PROG_MEM = "Default Configuration";
constexpr char mfg_name[] PROG_MEM = "Manufacturer";
constexpr char product_name[] PROG_MEM = "Product";
constexpr char product_serial[] PROG_MEM = "Serial";

// -----------------------------------------------------------------------------
// Describe USB classes
// -----------------------------------------------------------------------------
using class_mass_storage = usb_class_spec<usb_class::mass_storage, usb_subclass_mass_storage::atapi,
                                          usb_protocol_mass_storage::cbi_cci>;

// -----------------------------------------------------------------------------
// Describe USB endpoints
// -----------------------------------------------------------------------------
using ep_control = usb_endpoint_desc<usb_transfer_type::control, 32, usb_hw_flags::double_buffer>;
using ep_bulk_in =
    usb_endpoint_desc<usb_transfer_type::bulk, 64, usb_hw_flags::none, usb_transfer_dir::in>;

// -----------------------------------------------------------------------------
// Describe USB interfaces
// -----------------------------------------------------------------------------
using if_block_dev = usb_interface_desc<class_mass_storage, if_desc, ep_bulk_in>;

// -----------------------------------------------------------------------------
// Describe USB configrations
// -----------------------------------------------------------------------------
using config_traits =
    usb_config_desc<config_desc, usb_config_flags::bus_powered | usb_config_flags::remote_wakeup,
                    500, if_block_dev>;

// -----------------------------------------------------------------------------
// Describe USB device
// -----------------------------------------------------------------------------
constexpr uint16_t vendor_id = 0x52ab;
constexpr uint16_t product_id = 0x52ab;
using device_desc = usb_device_desc<usb_bcd::USB_2_0, usb_class_use_interface_class, vendor_id,
                                    product_id, semver_to_bcd(0, 0, 1), mfg_name, product_name,
                                    product_serial, ep_control, config_traits>;

//constexpr auto usb_descriptors PROG_MEM = usb_build_descriptors<device_traits>();

//auto usb_device = usb_create_device_p(usb_descriptors);
