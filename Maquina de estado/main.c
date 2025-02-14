#include <stdio.h>
#include <stdlib.h>

// Definición de estados
#define ESTADO_INICIAL 0
#define ESTADO_ERROR 1
#define ESTADO_ABRIENDO 2
#define ESTADO_CERRANDO 3
#define ESTADO_ABIERTO 4
#define ESTADO_CERRADO 5
#define ESTADO_DETENIDO 6
#define ESTADO_DESCONOCIDO 7

// Definición de constantes
#define LM_ACTIVO 1
#define LM_NOACTIVO 0
#define MOTOR_OFF 0
#define MOTOR_ON 1
#define LAMP_OFF 0
#define LAMP_ON 1

// Prototipos de funciones
int Func_ESTADO_INICIAL(void);
int Func_ESTADO_ERROR(void);
int Func_ESTADO_ABRIENDO(void);
int Func_ESTADO_ABIERTO(void);
int Func_ESTADO_CERRANDO(void);
int Func_ESTADO_CERRADO(void);
int Func_ESTADO_DETENIDO(void);
int Func_ESTADO_DESCONOCIDO(void);

// Variables globales
int ESTADO_SIGUIENTE = ESTADO_INICIAL;
int ESTADO_ACTUAL = ESTADO_INICIAL;
int ESTADO_ANTERIOR = ESTADO_INICIAL;

// Estructura del sistema de I/O
struct SYSTEM_IO {
    unsigned int lsc : 1;      // Limit switch puerta cerrada
    unsigned int lsa : 1;      // Limit switch puerta abierta
    unsigned int ma : 1;       // Motor abrir
    unsigned int mc : 1;       // Motor cerrar
    unsigned int lamp : 1;     // Luz indicadora
    unsigned int dpsw_fd : 2;  // Dip switch función desconocido
} io;

int main() {
    for (;;) {
        switch (ESTADO_SIGUIENTE) {
            case ESTADO_INICIAL:
                ESTADO_SIGUIENTE = Func_ESTADO_INICIAL();
                break;
            case ESTADO_ERROR:
                ESTADO_SIGUIENTE = Func_ESTADO_ERROR();
                break;
            case ESTADO_ABRIENDO:
                ESTADO_SIGUIENTE = Func_ESTADO_ABRIENDO();
                break;
            case ESTADO_ABIERTO:
                ESTADO_SIGUIENTE = Func_ESTADO_ABIERTO();
                break;
            case ESTADO_CERRANDO:
                ESTADO_SIGUIENTE = Func_ESTADO_CERRANDO();
                break;
            case ESTADO_CERRADO:
                ESTADO_SIGUIENTE = Func_ESTADO_CERRADO();
                break;
            case ESTADO_DETENIDO:
                ESTADO_SIGUIENTE = Func_ESTADO_DETENIDO();
                break;
            case ESTADO_DESCONOCIDO:
                ESTADO_SIGUIENTE = Func_ESTADO_DESCONOCIDO();
                break;
            default:
                ESTADO_SIGUIENTE = ESTADO_ERROR;
                break;
        }
    }
    return 0;
}

// Función para el estado inicial
int Func_ESTADO_INICIAL(void) {
    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_INICIAL;

    // Inicializar salidas
    io.ma = MOTOR_OFF;
    io.mc = MOTOR_OFF;
    io.lamp = LAMP_OFF;

    // Detectar estado inicial
    if (io.lsa == LM_ACTIVO && io.lsc == LM_ACTIVO) {
        return ESTADO_ERROR;
    } else if (io.lsa == LM_ACTIVO) {
        return ESTADO_ABIERTO;
    } else if (io.lsc == LM_ACTIVO) {
        return ESTADO_CERRADO;
    } else {
        return ESTADO_DESCONOCIDO;
    }
}

// Función para el estado de error
int Func_ESTADO_ERROR(void) {
    io.ma = MOTOR_OFF;
    io.mc = MOTOR_OFF;
    io.lamp = LAMP_ON; // Indicar error
    return ESTADO_INICIAL;
}

// Función para el estado abriendo
int Func_ESTADO_ABRIENDO(void) {
    io.ma = MOTOR_ON;
    io.mc = MOTOR_OFF;
    if (io.lsa == LM_ACTIVO) {
        return ESTADO_ABIERTO;
    }
    return ESTADO_ABRIENDO;
}

// Función para el estado abierto
int Func_ESTADO_ABIERTO(void) {
    io.ma = MOTOR_OFF;
    io.mc = MOTOR_OFF;
    io.lamp = LAMP_ON; // Indicar puerta abierta
    return ESTADO_ABIERTO;
}

// Función para el estado cerrando
int Func_ESTADO_CERRANDO(void) {
    io.ma = MOTOR_OFF;
    io.mc = MOTOR_ON;
    if (io.lsc == LM_ACTIVO) {
        return ESTADO_CERRADO;
    }
    return ESTADO_CERRANDO;
}

// Función para el estado cerrado
int Func_ESTADO_CERRADO(void) {
    io.ma = MOTOR_OFF;
    io.mc = MOTOR_OFF;
    io.lamp = LAMP_OFF; // Indicar puerta cerrada
    return ESTADO_CERRADO;
}

// Función para el estado detenido
int Func_ESTADO_DETENIDO(void) {
    io.ma = MOTOR_OFF;
    io.mc = MOTOR_OFF;
    return ESTADO_DETENIDO;
}

// Función para el estado desconocido
int Func_ESTADO_DESCONOCIDO(void) {
    if (io.dpsw_fd == 1) {
        return ESTADO_CERRANDO;
    } else if (io.dpsw_fd == 0) {
        return ESTADO_ABRIENDO;
    }
    return ESTADO_DETENIDO;
}
