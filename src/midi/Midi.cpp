/*
 * Copyright (C) 2016 Alexey Bolshakov <ua3mqj@gmail.com>
 * Copyright (C) 2016 Paul Fertser <fercerpav@gmail.com>
 * Copyright (C) 2014 Daniel Thompson <daniel@redfelineninja.org.uk>
 * Copyright (C) 2010 Gareth McMullin <gareth@blacksphere.co.nz>
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2013 Stephen Dwyer <scdwyer@ualberta.ca>
 * Copyright (C) 2014 Laurent Barattero <laurentba@larueluberlu.net>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "Midi.hpp"
#include <stdlib.h>


#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/desig.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/stm32/spi.h>
#include <libopencmsis/core_cm3.h>
#include <Clock.hpp>





/*
 * All references in this file come from Universal Serial Bus Device Class
 * Definition for MIDI Devices, release 1.0.
 */



//FIXME for some reason ifaces cannot be part of the header?!
usb_interface ifaces[2];




static void usbmidi_data_rx_cb(usbd_device* dev, uint8_t ep)
{

  /* SysEx identity message, preformatted with correct USB framing information */
  const uint8_t sysex_identity[] =
  {
    0x04,   /* USB Framing (3 byte SysEx) */
    0xf0,   /* SysEx start */
    0x7e,   /* non-realtime */
    0x00,   /* Channel 0 */
    0x04,   /* USB Framing (3 byte SysEx) */
    0x7d,   /* Educational/prototype manufacturer ID */
    0x66,   /* Family code (byte 1) */
    0x66,   /* Family code (byte 2) */
    0x04,   /* USB Framing (3 byte SysEx) */
    0x51,   /* Model number (byte 1) */
    0x19,   /* Model number (byte 2) */
    0x00,   /* Version number (byte 1) */
    0x04,   /* USB Framing (3 byte SysEx) */
    0x00,   /* Version number (byte 2) */
    0x01,   /* Version number (byte 3) */
    0x00,   /* Version number (byte 4) */
    0x05,   /* USB Framing (1 byte SysEx) */
    0xf7,   /* SysEx end */
    0x00,   /* Padding */
    0x00,   /* Padding */
  };


  static bool keys[128] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  char buf[64];
  int len = usbd_ep_read_packet(dev, 0x01, buf, 64);

  /* This implementation treats any message from the host as a SysEx
   * identity request. This works well enough providing the host
   * packs the identify request in a single 8 byte USB message.
   */
  /* if (len) {
      while (usbd_ep_write_packet(dev, 0x81, sysex_identity,
                      sizeof(sysex_identity)) == 0);
  } */

  if(len)
  {
    if(buf[0] == 0x09)
    {
      //note on
      keys[(uint8_t)(buf[2])] = 1;
    }
    else if(buf[0] == 0x08)
    {
      //note off
      keys[(uint8_t)(buf[2])] = 0;
    }

    bool note_on = false;
    for(unsigned int i = 0; i < 128; i++)
    {
      if(keys[i] == 1)
      {
        note_on = true;
      }
    }
    if(note_on)
    {
      gpio_clear(GPIOC, GPIO13);

//       spi_enable_ss_output(SPI2);

//       spi_send(SPI2, (uint8_t) buf[2]);

//       while(!(SPI_SR(SPI2) & SPI_SR_TXE));

//       while(SPI_SR(SPI2) & SPI_SR_BSY);

//       spi_disable_ss_output(SPI2);
    }
    else
    {
      gpio_set(GPIOC, GPIO13);
    }
  }
}

Midi::Midi(MidiErrHandler errorHandler) : errorHandler(errorHandler)
{
  init();
}


void Midi::update()
{
  usbd_poll(usbd_dev);
}

void Midi::init()
{
  /*
  * Table B-1: MIDI Adapter Device Descriptor
  */
  dev_descr.bLength = USB_DT_DEVICE_SIZE;
  dev_descr.bDescriptorType = USB_DT_DEVICE;
  dev_descr.bcdUSB = 0x0200;   /* was 0x0110 in Table B-1 example descriptor */
  dev_descr.bDeviceClass = 0;  /* device defined at interface level */
  dev_descr.bDeviceSubClass = 0;
  dev_descr.bDeviceProtocol = 0;
  dev_descr.bMaxPacketSize0 = 64, //no other value allowed for usb high speed devices
            dev_descr.idVendor = 0x6666;  //vendor id (6666 is for prototype devices)
  dev_descr.idProduct = 0x4242; //product id (shows up in lsusb)
  dev_descr.bcdDevice = 0x0100;
  dev_descr.iManufacturer = 1;  /* index to string desc */ //FIXME index is 1-based? wtf?
  dev_descr.iProduct = 2;       /* index to string desc */
  dev_descr.iSerialNumber = 3;  /* index to string desc */
  dev_descr.bNumConfigurations = 1;


  /* Table B-12: MIDI Adapter Class-specific Bulk OUT Endpoint Descriptor  */
  midi_bulk_endp[0].head.bLength = sizeof(struct usb_midi_endpoint_descriptor);
  midi_bulk_endp[0].head.bDescriptorType = USB_AUDIO_DT_CS_ENDPOINT;
  midi_bulk_endp[0].head.bDescriptorSubType = USB_MIDI_SUBTYPE_MS_GENERAL;
  midi_bulk_endp[0].head.bNumEmbMIDIJack = 1;
  midi_bulk_endp[0].jack[0].baAssocJackID = 0x01;

  /* Table B-14: MIDI Adapter Class-specific Bulk IN Endpoint Descriptor  */
  midi_bulk_endp[1].head.bLength = sizeof(struct usb_midi_endpoint_descriptor);
  midi_bulk_endp[1].head.bDescriptorType = USB_AUDIO_DT_CS_ENDPOINT;
  midi_bulk_endp[1].head.bDescriptorSubType = USB_MIDI_SUBTYPE_MS_GENERAL;
  midi_bulk_endp[1].head.bNumEmbMIDIJack = 1;
  midi_bulk_endp[1].jack[0].baAssocJackID = 0x03;


  /* Table B-11: MIDI Adapter Standard Bulk OUT Endpoint Descriptor */
  bulk_endp[0].bLength = USB_DT_ENDPOINT_SIZE;
  bulk_endp[0].bDescriptorType = USB_DT_ENDPOINT;
  bulk_endp[0].bEndpointAddress = 0x01;
  bulk_endp[0].bmAttributes = USB_ENDPOINT_ATTR_BULK;
  bulk_endp[0].wMaxPacketSize = 64;
  bulk_endp[0].bInterval = 0x00;
  bulk_endp[0].extra = &midi_bulk_endp[0];
  bulk_endp[0].extralen = sizeof(midi_bulk_endp[0]);

  bulk_endp[1].bLength = USB_DT_ENDPOINT_SIZE;
  bulk_endp[1].bDescriptorType = USB_DT_ENDPOINT;
  bulk_endp[1].bEndpointAddress = 0x81;
  bulk_endp[1].bmAttributes = USB_ENDPOINT_ATTR_BULK;
  bulk_endp[1].wMaxPacketSize = 64; 
  bulk_endp[1].bInterval = 0x00;
  bulk_endp[1].extra = &midi_bulk_endp[1];
  bulk_endp[1].extralen = sizeof(midi_bulk_endp[1]);


  /* Table B-4: MIDI Adapter Class-specific AC Interface Descriptor */
  audio_control_functional_descriptors.header_head.bLength = sizeof(struct usb_audio_header_descriptor_head) +
      1 * sizeof(struct usb_audio_header_descriptor_body);
  audio_control_functional_descriptors.header_head.bDescriptorType = USB_AUDIO_DT_CS_INTERFACE;
  audio_control_functional_descriptors.header_head.bDescriptorSubtype = USB_AUDIO_TYPE_HEADER;
  audio_control_functional_descriptors.header_head.bcdADC = 0x0100;
  audio_control_functional_descriptors.header_head.wTotalLength =
    sizeof(struct usb_audio_header_descriptor_head) +
    1 * sizeof(struct usb_audio_header_descriptor_body);
  audio_control_functional_descriptors.header_head.binCollection = 1;
  audio_control_functional_descriptors.header_body .baInterfaceNr = 0x01;



  /* Table B-6: Midi Adapter Class-specific MS Interface Descriptor */
  midi_streaming_functional_descriptors.header.bLength = sizeof(struct usb_midi_header_descriptor);
  midi_streaming_functional_descriptors.header.bDescriptorType = USB_AUDIO_DT_CS_INTERFACE;
  midi_streaming_functional_descriptors.header.bDescriptorSubtype = USB_MIDI_SUBTYPE_MS_HEADER;
  midi_streaming_functional_descriptors.header.bcdMSC = 0x0100;
  midi_streaming_functional_descriptors.header.wTotalLength = sizeof(midi_streaming_functional_descriptors);

  /* Table B-7: MIDI Adapter MIDI IN Jack Descriptor (Embedded) */
  midi_streaming_functional_descriptors.in_embedded.bLength = sizeof(struct usb_midi_in_jack_descriptor);
  midi_streaming_functional_descriptors.in_embedded.bDescriptorType = USB_AUDIO_DT_CS_INTERFACE;
  midi_streaming_functional_descriptors.in_embedded.bDescriptorSubtype = USB_MIDI_SUBTYPE_MIDI_IN_JACK;
  midi_streaming_functional_descriptors.in_embedded.bJackType = USB_MIDI_JACK_TYPE_EMBEDDED;
  midi_streaming_functional_descriptors.in_embedded.bJackID = 0x01;
  midi_streaming_functional_descriptors.in_embedded.iJack = 0x00;
  /* Table B-8: MIDI Adapter MIDI IN Jack Descriptor (External) */
  midi_streaming_functional_descriptors.in_external.bLength = sizeof(struct usb_midi_in_jack_descriptor);
  midi_streaming_functional_descriptors.in_external.bDescriptorType = USB_AUDIO_DT_CS_INTERFACE;
  midi_streaming_functional_descriptors.in_external.bDescriptorSubtype = USB_MIDI_SUBTYPE_MIDI_IN_JACK;
  midi_streaming_functional_descriptors.in_external.bJackType = USB_MIDI_JACK_TYPE_EXTERNAL;
  midi_streaming_functional_descriptors.in_external.bJackID = 0x02;
  midi_streaming_functional_descriptors.in_external.iJack = 0x00;
  /* Table B-9: MIDI Adapter MIDI OUT Jack Descriptor (Embedded) */
  midi_streaming_functional_descriptors.out_embedded.head.bLength = sizeof(struct usb_midi_out_jack_descriptor);
  midi_streaming_functional_descriptors.out_embedded.head.bDescriptorType = USB_AUDIO_DT_CS_INTERFACE;
  midi_streaming_functional_descriptors.out_embedded.head.bDescriptorSubtype = USB_MIDI_SUBTYPE_MIDI_OUT_JACK;
  midi_streaming_functional_descriptors.out_embedded.head.bJackType = USB_MIDI_JACK_TYPE_EMBEDDED;
  midi_streaming_functional_descriptors.out_embedded.head.bJackID = 0x03;
  midi_streaming_functional_descriptors.out_embedded.head.bNrInputPins = 1;
  midi_streaming_functional_descriptors.out_embedded.source[0].baSourceID = 0x02;
  midi_streaming_functional_descriptors.out_embedded.source[0].baSourcePin = 0x01;
  midi_streaming_functional_descriptors.out_embedded.tail.iJack = 0x00;
  /* Table B-10: MIDI Adapter MIDI OUT Jack Descriptor (External) */
  midi_streaming_functional_descriptors.out_external.head.bLength = sizeof(struct usb_midi_out_jack_descriptor);
  midi_streaming_functional_descriptors.out_external.head.bDescriptorType = USB_AUDIO_DT_CS_INTERFACE;
  midi_streaming_functional_descriptors.out_external.head.bDescriptorSubtype = USB_MIDI_SUBTYPE_MIDI_OUT_JACK;
  midi_streaming_functional_descriptors.out_external.head.bJackType = USB_MIDI_JACK_TYPE_EXTERNAL;
  midi_streaming_functional_descriptors.out_external.head.bJackID = 0x04;
  midi_streaming_functional_descriptors.out_external.head.bNrInputPins = 1;
  midi_streaming_functional_descriptors.out_external.source[0].baSourceID = 0x01;
  midi_streaming_functional_descriptors.out_external.source[0].baSourcePin = 0x01;
  midi_streaming_functional_descriptors.out_external.tail.iJack = 0x00;


  /* Table B-5: MIDI Adapter Standard MS Interface Descriptor */
  midi_streaming_iface[0].bLength = USB_DT_INTERFACE_SIZE;
  midi_streaming_iface[0].bDescriptorType = USB_DT_INTERFACE;
  midi_streaming_iface[0].bInterfaceNumber = 1;
  midi_streaming_iface[0].bAlternateSetting = 0;
  midi_streaming_iface[0].bNumEndpoints = 2;
  midi_streaming_iface[0].bInterfaceClass = USB_CLASS_AUDIO;
  midi_streaming_iface[0].bInterfaceSubClass = USB_AUDIO_SUBCLASS_MIDISTREAMING;
  midi_streaming_iface[0].bInterfaceProtocol = 0;
  midi_streaming_iface[0].iInterface = 0;
  midi_streaming_iface[0].endpoint = bulk_endp;
  midi_streaming_iface[0].extra = &midi_streaming_functional_descriptors;
  midi_streaming_iface[0].extralen = sizeof(midi_streaming_functional_descriptors);

  /* Table B-3: MIDI Adapter Standard AC Interface Descriptor */
  audio_control_iface[0].bLength = USB_DT_INTERFACE_SIZE;
  audio_control_iface[0].bDescriptorType = USB_DT_INTERFACE;
  audio_control_iface[0].bInterfaceNumber = 0;
  audio_control_iface[0].bAlternateSetting = 0;
  audio_control_iface[0].bNumEndpoints = 0;
  audio_control_iface[0].bInterfaceClass = USB_CLASS_AUDIO;
  audio_control_iface[0].bInterfaceSubClass = USB_AUDIO_SUBCLASS_CONTROL;
  audio_control_iface[0].bInterfaceProtocol = 0;
  audio_control_iface[0].iInterface = 0;
  audio_control_iface[0].extra = &audio_control_functional_descriptors;
  audio_control_iface[0].extralen = sizeof(audio_control_functional_descriptors);


  ifaces[0].num_altsetting = 1;
  ifaces[0].altsetting = audio_control_iface;
  ifaces[1].num_altsetting = 1;
  ifaces[1].altsetting = midi_streaming_iface;

  /* Table B-2: MIDI Adapter Configuration Descriptor */
  config.bLength = USB_DT_CONFIGURATION_SIZE;
  config.bDescriptorType = USB_DT_CONFIGURATION;
  config.wTotalLength = 0; /* can be anything, it is updated automatically
                  when the usb code prepares the descriptor */
  config.bNumInterfaces = 2; /* control and data */
  config.bConfigurationValue = 1;
  config.iConfiguration = 0;
  config.bmAttributes = 0x80; /* bus powered */
  config.bMaxPower = 0x32;
  config.interface = ifaces;

  usb_strings[0] = "https://github.com/arneboe/stm32-midi-cmake"; //manufacturer
  usb_strings[1] = "stm32 usb midi"; //product
  usb_strings[2] = "0000000000\0";//serial number //FIXME why do i need the \0? strings are null terminated anyway?

  rcc_periph_clock_enable(RCC_GPIOA);


  /* lower hotplug and leave enough time for the host to notice */
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO11 | GPIO12);
  gpio_clear(GPIOA, GPIO11 || GPIO12);

  Clock::delayMs(5);

  usbd_dev = usbd_init(&st_usbfs_v1_usb_driver, &dev_descr, &config,
                       usb_strings, 3,
                       usbd_control_buffer, sizeof(usbd_control_buffer));


  usbd_register_set_config_callback(usbd_dev, [](usbd_device * dev, uint16_t wValue)
  {
    //receive on this endpoint
    usbd_ep_setup(dev, 0x01, USB_ENDPOINT_ATTR_BULK, 64,
                  usbmidi_data_rx_cb);
    //send on this endpoint
    usbd_ep_setup(dev, 0x81, USB_ENDPOINT_ATTR_BULK, 64, NULL);
  });

  //FIXME here we should wait for the config lambda to be called, otherwise the user might send
  //      a message before the endpoints are created. However this is not possible because we cannot
  //      access object state from within the callback :/

}

void Midi::messageToBuffer(const Midi::CCMessage& msg, char* buffer)
{
  //see chapter 4 "USB-MIDI Event Packets" in
  // "Universal Serial Bus Device Class Definition for MIDI Devices"
  // http://www.usb.org/developers/docs/devclass_docs/midi10.pdf

  //virtualCable is clamped by shifting (see below)
  // USB framing for CC message: 4 bit | 4bit
  //                             cable | 0xB
  const uint8_t usbFrame = (msg.virtualCable << 4) | 0x0B;
  
  //MIDI command for CC message: 4 bit | 4 bit
  //                             0xB   | midi channel
  const uint8_t midiCommand = 0xB0 | (msg.channel > 0xF ? 0xF : msg.channel);
  
  //FIXME there might be a faster way to clamp the values
  buffer[0] = usbFrame;
  buffer[1] = midiCommand;
  buffer[2] = msg.controlChannel > 119 ? 119 : msg.controlChannel;
  buffer[3] = msg.value > 127 ? 127 : msg.value;
}


bool Midi::sendCC(const CCMessage& msg)
{

  char buffer[4];
  messageToBuffer(msg, buffer);
  if(usbd_ep_write_packet(usbd_dev, 0x81, buffer, 4) != 4)
  {
    errorHandler(USB_WRITE_ERROR);
    return false;
  }
  //FIXME increase fifo size if possible
  
  return true;
}

bool Midi::sendCC(const CCMessage* messages, const uint8_t numMessages)
{
  if(numMessages == 0) return true;
  
  char buffer[64];
  
  const int size = numMessages > 16 ? 16 : numMessages;
  for(int i = 0; i < size; ++i)
  {
    messageToBuffer(messages[i], buffer + 4 * i);
  }
  
  const int usedBufferSize = size * 4;
  if(usbd_ep_write_packet(usbd_dev, 0x81, buffer, usedBufferSize) != usedBufferSize)
  {
    errorHandler(USB_WRITE_ERROR);
    return false;
  }
  return true;
}

 

//some test code

/*
void midiTest(Midi& m)
{
  uint32_t lastTime = 0;

  uint8_t channel = 0;
  
  while(1)
  {
    m.update();
    if(Clock::ticks - lastTime > 500)
    {
      lastTime = Clock::ticks;
      
      m.sendCC(Midi::CCMessage(0, channel, 0, 42));     
      
      ++channel;
      if(channel > 20) break;
    }
  }
  
  uint8_t controlChannel = 0;
  while(1)
  {
    m.update();
    if(Clock::ticks - lastTime > 50)
    {
      lastTime = Clock::ticks;
      
      m.sendCC(Midi::CCMessage(0, 0, controlChannel, 42));
      ++controlChannel;
      if(controlChannel > 140) break;
    }
  }
  
  uint8_t value = 0;
  while(1)
  {
    m.update();
    if(Clock::ticks - lastTime > 50)
    {
      lastTime = Clock::ticks;
      
      m.sendCC(Midi::CCMessage(0, 0, 42, value));
      ++value;
      if(value > 140) break;
    }
  }
  
  m.update();

  m.update();
  Midi::CCMessage messages2[22];
  for(int i = 0; i < 22; ++i)
  {
    messages2[i] = Midi::CCMessage(0, 0, 0, 16);
  }
  m.sendCC(messages2, 22);

  m.update();
  Clock::delayMs(100);
  
  Midi::CCMessage messages[10];
  for(int i = 0; i < 10; ++i)
  {
    messages[i] = Midi::CCMessage(0, 0, 0, 10);
  }
  m.sendCC(messages, 10);
  m.update();
  

  m.update();
  
}*/


