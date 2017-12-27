#pragma once
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/audio.h>
#include <libopencm3/usb/midi.h>
#include <vector>
#include "MidiMessage.hpp"

//FIXME const corectness :)

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

   * @warning ranges are enforced by clamping
   * @note update has to be called after every send. otherwise messsages will get lost.
           If you need to send more than one msg, use the bulk version below.
   * @return False in case of error*/
  bool sendCC(const CCMessage& msg);
  
  
  //FIXME why am I only able to send 16 instead of 19 messages without breaking the stack?
  
  /** Sends up to 16 messages in bulk.
   *  @warning messages.size() will be clamped to 16.
   */
  bool sendCC(const CCMessage* messages, const uint8_t numMessages);
  
  /** Sends a note on/off message.
    * Calls errorHandler in case of error
    * @warning ranges are enforced by clamping, no waring is generated
    * @note update has to be called after every send. otherwise messages wil get lost.
    *       If you need to send more than one msg, use the bulk version below.
    * @return False in case of error. */
  bool sendNote(const NoteMessage& msg);
  bool sendNote(const NoteMessage* messages, const uint8_t numMessages);

  /** @param errorHandler will be called in case of any errors */
  Midi(MidiErrHandler errorHandler);
  
  //FIXME wait till buffers are cleared?!
  /** @warning method returns before buffers are cleared?! No idea why */
  void update();
};

