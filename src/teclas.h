//=============================================================================
//                       IMPOREXO
//                      (c) Copyright 2020
//=============================================================================
#ifndef _TECLAS_H_
#define _TECLAS_H_


#define TIMEOUT_FILTRO        35    //50ms para capturar una tecla para evitar el efecto rebote.
#define TIMEOUT_MENSAJE       500   //Timeout para mostrar mensaje de apagar FOCOIRIS.
#define TIMEOUT_CAMBIO        3000  //Timeout para el cambio de color, por encima del cual se apagará el foco.
#define TIMEOUT_FINALCARRERA  250
//#define TIME_TEST_FABRICA 2000  //Timeout para entrar en el Test de Fabrica.

#define BOTON_OFF     -1
#define BOTON_3S      0
#define BOTON_ON      1
#define BOTON_MENSAJE 2
#define BOTON_CAMBIO  3
#define BOTON_TEST_ON 4

#define DETECCION_BOTON 0
//=============================================================================
// Public data declarations
//=============================================================================

  //extern void teclas_Iniciar_Variables (unsigned long TIME_actual);
  extern void teclas_Iniciar_Variables_F (unsigned long TIME_actual);

  //extern int teclas_filtro_Teclado_Timeout (int teclas, unsigned long T_actual);
  extern int teclas_filtro_Teclado_FAVORITO (int teclas, unsigned long T_actual);
  extern int teclas_ESTIRAR (int teclas, unsigned long T_actual);
  extern int teclas_RECOGER (int teclas, unsigned long T_actual);
 
#endif // _TECLAS_H_
