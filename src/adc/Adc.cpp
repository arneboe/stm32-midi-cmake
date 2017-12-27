#include "Adc.hpp"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/adc.h>
#include <Clock.hpp>

Adc::Adc()
{
  rcc_periph_clock_enable(RCC_GPIOA);
  gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO0 | GPIO1 | GPIO2 | GPIO3 | GPIO4 | GPIO5 | GPIO6 | GPIO7);
  
  rcc_periph_clock_enable(RCC_ADC1);
  // Make sure the ADC doesn't run during config.
  adc_power_off(ADC1);
  
  //NOTE adc prescaler is already by opencm3, if framework is switched it might have to be initialized here

  // We configure everything for one single conversion.
  adc_disable_scan_mode(ADC1);
  adc_set_single_conversion_mode(ADC1);
  adc_disable_external_trigger_regular(ADC1);
  adc_set_right_aligned(ADC1);
  //adc_enable_temperature_sensor();
  adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_28DOT5CYC);

  adc_power_on(ADC1);
  //wait for adc to stabilize
  Clock::delayMs(1); //only need to wait 3us but 1ms is the smallest amount we can wait

  adc_reset_calibration(ADC1);
  while((ADC_CR2(ADC1) & ADC_CR2_RSTCAL) != 0);   //wait for calibration reset
  adc_calibrate(ADC1);
  while((ADC_CR2(ADC1) & ADC_CR2_CAL) != 0);  //wait for calibration finish
}

void Adc::update()
{
  //TODO add multiplexing to read more than 8 values
  //TODO use dma

    for(uint8_t channel = 0; channel < NUM_ADC; ++channel)
    {
      //we sample 50 times. With the naiv aproach this takes about 2ms...
      uint32_t sum = 0;
      const uint16_t numSamples = 300;
      
      for(int i = 0; i < numSamples; ++i)
      {
        adc_set_regular_sequence(ADC1, 1, &channel);
        adc_start_conversion_direct(ADC1);
        while(!(adc_eoc(ADC1))); //wait for adc to finish conversion
        sum += adc_read_regular(ADC1);
      }
      values[channel] = (int)(sum / ((float)numSamples) + 0.5f);
    }
}


//test adc dma code:

// static void adc_setup()
// {
//   rcc_periph_clock_enable(RCC_GPIOA);
//   gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO0 | GPIO1 | GPIO2 | GPIO3);
//   
//   rcc_periph_clock_enable(RCC_ADC1);
//   
//     //FIXME add more pins later
// 
//   
//   adc_power_off(ADC1); //needs to be off for config
//   
//   //we want to read multiple inputs automatically
//   adc_enable_scan_mode(ADC1);
//   //we want to continously read in the background
//   adc_set_continuous_conversion_mode(ADC1);
//   adc_disable_external_trigger_regular(ADC1);
//   adc_set_right_aligned(ADC1);
//   adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_71DOT5CYC);
// 
// //   adc_enable_eoc_interrupt(ADC1);
//   
//    //we want to read results with dma
//    adc_enable_dma(ADC1);
//    
//    adc_power_on(ADC1);
//    
//   /* Wait for ADC starting up. */
//   for(int i = 0; i < 800000; i++)   //FIXME use delayms
//     __asm__("nop");
// 
//   adc_reset_calibration(ADC1);
//   while ((ADC_CR2(ADC1) & ADC_CR2_RSTCAL) != 0); 
//   adc_calibration(ADC1);
//   while ((ADC_CR2(ADC1) & ADC_CR2_CAL) != 0);
//   
//   
//     //FIXME not sure if this should be done before or after power on
//    //we want to read the first 4 channels
//    uint8_t channels[] = {0, 1, 2, 3};
//    adc_set_regular_sequence(ADC1, 4, channels);
// }


// //copy 4 values from adc1 to an array
// static void dma_setup()
// {
//   
//   //ADC1 ist h DMA1 channel 1 
//   rcc_periph_clock_enable(RCC_DMA1);
//   dma_disable_channel(DMA1, DMA_CHANNEL1);
//   dma_channel_reset(DMA1, DMA_CHANNEL1);
//   
//   //from
//   dma_set_peripheral_address(DMA1, DMA_CHANNEL1, ADC1_DR);
//   dma_set_peripheral_size(DMA1, DMA_CHANNEL1, DMA_CCR_PSIZE_16BIT); //the register size is 16 bit 
// 
//   //read the register 4
//   dma_set_number_of_data(DMA1, DMA_CHANNEL1, 4); //FIXME later change 4 to the real thing
//   
//   //to
//   dma_set_memory_address(DMA1, DMA_CHANNEL1, (uint32_t)&dmaBuffer);
//   dma_set_memory_size(DMA1, DMA_CHANNEL1, DMA_CCR_MSIZE_16BIT); //one entry in the array is 16 bit long
//   dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL1);  //increment memory adress after each transfer
//   dma_enable_circular_mode(DMA1, DMA_CHANNEL1); 
// 
//   //transfer from register to memory
//   dma_set_read_from_peripheral(DMA1, DMA_CHANNEL1);//we want to read from the adc register
//   
//   
//   dma_set_priority(DMA1, DMA_CHANNEL1, DMA_CCR_PL_MEDIUM);
//   dma_enable_channel(DMA1, DMA_CHANNEL1);
//   
// }
