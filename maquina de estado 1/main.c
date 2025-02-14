#include <stdio.h>
#include <stdlib.h>
#define ESTADO_INICIAL 0
#define ESTADO_ERROR 1
#define ESTADO_ABRIENDO 2
#define ESTADO_CERRANDO 3
#define ESTADO_ABIERTO 4
#define ESTADO_CERRADO 5
#define ESTADO_DETENIDO 6
#define ESTADO_DESCONOCIDO 7
#define LM_ACTIVO 1
#define LM _NOACTIVO 0
#define MORIOR_OFF 0
#define MOTOR_ON 1
#define LAMP_OFF

int Func_ESTADO_INICIAL(void);
int Func_ESTADO_ERROR(void);
int Func_ESTADO_ABRIENDO(void);
int Func_ESTADO_ABIERTO(void);
int Func_ESTADO_CERRANDO(void);
int Func_ESTADO_CERRADO(void);
int Func_ESTADO_DETENIDO(void);
int Func_ESTADO_DESCONICIDO(void);

/*typedef enum
    {
      ESTADO_INICIAL,
      ESTADO_ERROR,
      ESTADO_ABRIENDO,
      ESTADO_CERRANDO,
      ESTADO_ABIERTO,
      ESTADO_CERRADO,
      ESTADO_FINAL,
    } Estado;
*/

    int ESTADO_SIGUIENTE = ESTADO_INICIA;
    int ESTADO_ANTERIOR = ESTADO_INICIAL;
    int ESTADO_ACTUAL = ESTADO_INICIAL;

/*info de los comportamientos dip sw
dpsw_fd
 1-1 de desconocido a cerrar
 1-0 de descocnocido a espera comando
 0-1 de desconocido a abrir
 0-0 de descocnocido a parpadear
 dpsw_fpp
 0 - seguir en el estado antes de detener
 1 - estado contrario de donde estaba antes
 si estaba cerrando entonces empieza a abrir
 */

struct SYSTEM_IO
 {
    unsigned int lsc:1;//limit switch puerta cerrado
    unsigned int lsa:1;//limit switch puerta abierto
    unsigned int ftc:1;
    unsigned int ma:1;//motor abierto
    unsigned int mc:1;//motor cerrado
    unsigned int lamp:1;
    unsigned int keya:1;
    unsigned int keyc:1;
    unsigned int pp:1;
    unsigned int dpsw_fd:2;//dipsw de dos pin para funcion de desconocido
    unsigned int dpsw_pp:2;

 }io;

int main()
{
    for(;;){

    }
     if(ESTADO_SIGUIENTE == ESTADO_INICIAL)
     {
         ESTADO_SIGUIENTE = Func_ESTADO_INICIAL();
     }

     if(ESTADO_SIGUIENTE == ESTADO_ERROR)
     {
         ESTADO_SIGUIENTE = Func_ESTADO_ERROR();
     }

     if(ESTADO_SIGUIENTE == ESTADO_ABRIENDO)
     {
         ESTADO_SIGUIENTE = Func_ESTADO_ABRIENDO();
     }

     if(ESTADO_SIGUIENTE == ESTADO_ABIERTO)
     {
         ESTADO_SIGUIENTE = Func_ESTADO_ABIERTO();
     }

     if(ESTADO_SIGUIENTE == ESTADO_CERRANDO)
     {
         ESTADO_SIGUIENTE = Func_ESTADO_CERRANDO();
     }

     if(ESTADO_SIGUIENTE == ESTADO_CERRADO)
     {
         ESTADO_SIGUIENTE = Func_ESTADO_CERRADO();
     }

     if(ESTADO_SIGUIENTE == ESTADO_DETENIDO)
     {
         ESTADO_SIGUIENTE = Func_ESTADO_DETENIDO();
     }

     if(ESTADO_SIGUIENTE == ESTADO_DESCONOCIDO)
     {
         ESTADO_SIGUIENTE = Func_ESTADO_DESCONICIDO();
     }


       printf("Hello world!\n");
    return 0;


}


int Func_ESTADO_INICIAL(void)
{
   // inicial
   ESTADO_ANTERIOR = ESTADO_ACTUAL;
   ESTADO_ACTUAL = ESTADO_INICIAL;
   //inicializar el estado
   io.ma = MOTOR_OFF
   io.mc = MOTOR_OFF
   io.lamp = LAMP_OFF
   //ciclo de estado
   for(;;)
   {
       //detectar error en limit sw
       if(io.lsa == LM_ACTIVO && io.lsc == LM_ACTIVO)
       {
           return ESTADO_ERROR;
       }

       if(io.lsa == LM_ACTIVO && io.lsc == LM_NOACTIVO)
       {
           return ESTADO_ABIERTO;
       }

       if(io.lsa == LM_NOACTIVO && io.lsc == LM_NOACTIVO)
       {
           return ESTADO_DESCONOCIDO;
       }
   }
}
int Func_ESTADO_ERROR(void)
{
    return ESTADO_INICIAL;
}

int Func_ESTADO_ABRIENDO(void)
{
    io.ma = MOTOR_ON;
    io.mc = MOTOR_OFF;
    if (io.lsa == LM_ACTIVO) {
        return ESTADO_ABIERTO;
    }
    return ESTADO_ABRIENDO;
}

int Func_ESTADO_ABIERTO(void)
{
    io.ma = MOTOR_OFF;
    io.mc = MOTOR_OFF;
    return ESTADO_ABIERTO;
}

int Func_ESTADO_CERRANDO(void)
{
    io.ma = MOTOR_OFF;
    io.mc = MOTOR_ON;
    if (io.lsc == LM_ACTIVO) {
        return ESTADO_CERRADO;
}

int Func_ESTADO_CERRADO(void)
{
    io.ma = MOTOR_OFF;
    io.mc = MOTOR_OFF;
    return ESTADO_CERRADO;
}

int Func_ESTADO_DETENIDO(void)
{
    io.ma = MOTOR_OFF;
    io.mc = MOTOR_OFF;
    return ESTADO_DETENIDO;
}
int Func_ESTADO_DESCONICIDO(void)
{
    if (io.dpsw_fd == 1) {
        return ESTADO_CERRANDO;
    }
    if (io.dpsw_fd == 0) {
        return ESTADO_ABRIENDO;
    }
    return ESTADO_DETENIDO;
}

