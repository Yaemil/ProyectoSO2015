/**
 * Nuestro Modulo del Proyecto mas que todo son procedimientos los cuales son invocados por el
 * manejador de interrupciones el cual nos indicara cuando recibir y cuando enviar datos a traves
 * de la Tarjeta de Red, y a su vez dependeremos del Firewall el cual este tendra la capacidad de 
 * poder bloquear el envio y recepcion de paquetes. Al inicio se coloco un Random (0 - 1), el cual 
 * simulara las interrupciones 0 Recepcion y 1 envio de paquetes...
**/
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>

#define MAX 21
#define MOD %
#define AND &&
#define OR ||
#define FALSE 0
#define TRUE 1

typedef int bool;

typedef struct
{
	int Buffer;
	sem_t Bloqueado;
}Tarjeta_Red;

pthread_mutex_t Mutex_Requerido = PTHREAD_MUTEX_INITIALIZER;	// Garantiza EM //
pthread_cond_t Cond_Tarjeta_Disponible = PTHREAD_COND_INITIALIZER;	//Semaforo Condicion que duerme miestras no que el Buffer este lleno.//

void Inicio(Tarjeta_Red *TR);
void Envio_Recepcion_Datos(Tarjeta_Red *TR, int IRQ);
void Cierre_Envio_Recepcion_Tarjeta_Red(Tarjeta_Red *TR);
void Habilitacion_Envio_Recepcion_Tarjeta_Red(Tarjeta_Red *TR);
void *Recepcion_Peticion(Tarjeta_Red *TR);
void Insertar_Peticion_Buffer(Tarjeta_Red *TR);
void *Envio_Peticion(Tarjeta_Red *TR);
void Eliminar_Peticion_Buffer(Tarjeta_Red *TR);

int main()
{
	Tarjeta_Red TR;
	
	TR.Buffer = 0;
	sem_init(&TR.Bloqueado, 0, 1);
	Inicio(&TR);
	
	return 0;
}

void Inicio(Tarjeta_Red *TR)
{
	
	int Num;
	srand(time(NULL));
	while(1)
	{
		Num = (rand() MOD 2);
		Cierre_Envio_Recepcion_Tarjeta_Red(TR);   // Este es invocado por el Firewall para bloquear la Tarjeta de Red.//
		Habilitacion_Envio_Recepcion_Tarjeta_Red(TR);
		Envio_Recepcion_Datos(TR, Num);
	}
}

//	Cuando ocurre una IRQ5...	//
void Envio_Recepcion_Datos(Tarjeta_Red *TR, int IRQ)	///Falta una buena sincronizacion aqui...///
{
	int Valor;
	pthread_t P1, P2;
	//~ printf("\nIRQ ->: %d\n", IRQ);
	pthread_mutex_lock(&Mutex_Requerido);
	if((sem_getvalue(&TR->Bloqueado, &Valor) == 0) AND (Valor == 1))
	{
		if(IRQ == 0)
		{
			Recepcion_Peticion(TR);
		}else{
			Envio_Peticion(TR);
		}
		pthread_mutex_unlock(&Mutex_Requerido);	// Desbloquea el semaforo "Mutex_Requerido"	//
		pthread_cond_signal(&Cond_Tarjeta_Disponible);
	}else{
		printf("\nFirewall Bloqueo el Envio y Recepcion de Datos...\n");
		system("sleep 2.0");
	}
}

//	Cuando ocurre una IRQ6...	//
void Cierre_Envio_Recepcion_Tarjeta_Red(Tarjeta_Red *TR)
{
	sem_wait(&TR->Bloqueado);
}

void Habilitacion_Envio_Recepcion_Tarjeta_Red(Tarjeta_Red *TR)
{
	sem_post(&TR->Bloqueado);
}

void *Recepcion_Peticion(Tarjeta_Red *TR)
{
	if(TR->Buffer < MAX)
	{
		Insertar_Peticion_Buffer(TR);
	}else{
		printf("\nBuffer LLeno...\n");
		system("sleep 2.0");
	}
}

void Insertar_Peticion_Buffer(Tarjeta_Red *TR)
{
	TR->Buffer = (TR->Buffer + 1);
	printf("\nNuevo Paquete, para enviar...\n");
	printf("Capacidad Buffer ->: %d\n", TR->Buffer);
	system("sleep 1.0");
}

void *Envio_Peticion(Tarjeta_Red *TR)
{
	if(TR->Buffer >  0)
	{
		Eliminar_Peticion_Buffer(TR);
	}else{
		printf("\nBuffer Vacio...\n");
		system("sleep 2.0");
	}
}

void Eliminar_Peticion_Buffer(Tarjeta_Red *TR)
{
	TR->Buffer = (TR->Buffer - 1);
	printf("\nPaquete, enviado...\n");
	printf("Capacidad Buffer ->: %d\n", TR->Buffer);
	system("sleep 1.0");
}
