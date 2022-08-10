//=============================================================================
//                       ARP
//                      (c) Copyright 2021
//=============================================================================
#ifndef _ANALOGINPUT_H_
#define _ANALOGINPUT_H_

  #define MINIMA_MEDIDA_ULTRASONIDOS    71
  #define MAXIMA_MEDIDA_ULTRASONIDOS    800

  extern void AnalogInput_setup (void);
  extern void AnalogInput_loop (unsigned long t_actual);

#endif    //_ANALOGINPUT_H_