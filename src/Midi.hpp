#pragma once
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/audio.h>
#include <libopencm3/usb/midi.h>

class Midi
{
private:
  usbd_device* usbd_dev;
  usb_device_descriptor dev_descr;
  // Midi specific endpoint descriptors.
  usb_midi_endpoint_descriptor midi_bulk_endp[2];
  usb_endpoint_descriptor bulk_endp[2]; //Standard endpoint descriptors
  usb_interface_descriptor midi_streaming_iface[1]; //FIXME why size1

  struct
  {
    struct usb_audio_header_descriptor_head header_head;
    struct usb_audio_header_descriptor_body header_body;
  } __attribute__((packed)) audio_control_functional_descriptors;

  usb_interface_descriptor audio_control_iface[1]; //FIXME why size 1

  /*
  * Class-specific MIDI streaming interface descriptor
  */
  struct
  {
    struct usb_midi_header_descriptor header;
    struct usb_midi_in_jack_descriptor in_embedded;
    struct usb_midi_in_jack_descriptor in_external;
    struct usb_midi_out_jack_descriptor out_embedded;
    struct usb_midi_out_jack_descriptor out_external;
  } __attribute__((packed)) midi_streaming_functional_descriptors;

  usb_config_descriptor config;
  const char* usb_strings[3];
  /* Buffer to be used for control requests. */
  uint8_t usbd_control_buffer[128];

  void init();
public:
  Midi();
  void update();
};

