#pragma once
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/audio.h>
#include <libopencm3/usb/midi.h>
#include <vector>

class Midi
{
public:
  //TODO implement more errors
  enum MidiError
  {
    USB_WRITE_ERROR
    
  };
  
  typedef void (*MidiErrHandler)(MidiError);
  
  struct CCMessage
  {
    uint8_t virtualCable; // range [0x0 .. 0xF]
    uint8_t channel; // range [0x0 .. 0xF] 
    uint8_t controlChannel; // range [0 .. 119]
    uint8_t value; // range [0 .. 127] 
    CCMessage() : virtualCable(0), channel(0), controlChannel(0), value(0) {}
    CCMessage(uint8_t virtualCable, uint8_t channel, uint8_t controlChannel, uint8_t value) : 
      virtualCable(virtualCable), channel(channel), controlChannel(controlChannel), value(value) {}
  };
    
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
  
  void messageToBuffer(const CCMessage& msg, char* buffer);
  
public:
  
  /** Send a control change message
   * Calls errorHandler in case of error

   * @warning ranges are enforced by clamping
   * @note update has to be called after every send. otherwise messsages will get lost.
   * 
   * @return False in case of error*/
  bool sendCC(const CCMessage& msg);
  
  
  //FIXME why am I only able to send 16 instead of 19 messages without breaking the stack?
  /** Sends up to 16 messages in bulk.
   *  @warning messages.size() will be clamped to 16.
   *  @note In theory we should be able to send upt to 19 messages at the same time.
   *        However the stack seems to break if we end more than 16. No idea why. */
  bool sendCC(const CCMessage* messages, const uint8_t numMessages);

  /** @param errorHandler will be called in case of any errors */
  Midi(MidiErrHandler errorHandler);
  
  //FIXME wait till buffers are cleared?!
  /** @warning method returns before buffers are cleared?! No idea why */
  void update();
};

