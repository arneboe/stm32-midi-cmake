#pragma once
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/audio.h>
#include <libopencm3/usb/midi.h>

class Midi
{
public:
  //TODO implement more errors
  enum MidiError
  {
    USB_WRITE_ERROR
  };
  
  typedef void (*MidiErrHandler)(MidiError);
  
  
private:
  usbd_device* usbd_dev;
  usb_device_descriptor dev_descr;
  usb_midi_endpoint_descriptor midi_bulk_endp[2];/* << Midi specific endpoint descriptors */
  usb_endpoint_descriptor bulk_endp[2]; /* << Standard endpoint descriptors */
  usb_interface_descriptor midi_streaming_iface[1];

  struct
  {
    struct usb_audio_header_descriptor_head header_head;
    struct usb_audio_header_descriptor_body header_body;
  } __attribute__((packed)) audio_control_functional_descriptors;

  usb_interface_descriptor audio_control_iface[1];

  /*Class-specific MIDI streaming interface descriptor  */
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
  uint8_t usbd_control_buffer[128]; /* << Buffer to be used for control requests. */
  
  MidiErrHandler errorHandler;

  void init();
public:
  
  /** Send a control change message
   * Calls errorHandler in case of error
   * @param virtualCable range [0x0 .. 0xF]
   * @param channel range [0x0 .. 0xF] 
   * @param controlChannel range [0 .. 119]
   *                       see https://www.midi.org/specifications/item/table-3-control-change-messages-data-bytes-2
   *                       for details about the meaning of different control channel numbers
   * @param value range [0 .. 127] 
   * @warning ranges are enforced by clamping
   * @return False in case of error*/
  bool sendCC(uint8_t virtualCable, uint8_t channel, uint8_t controlChannel, uint8_t value);
  
  
  

  /** @param errorHandler will be called in case of any errors */
  Midi(MidiErrHandler errorHandler);
  
  void update();
};

