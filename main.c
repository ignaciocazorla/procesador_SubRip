#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "args.h"
#include "funciones.h"



int main(int argc,char * argv[]){
	struct arreglo_opciones arreglo_opciones;
	recuperar_args(&arreglo_opciones, argc, argv);

	process_operation(&arreglo_opciones);

	liberar_arreglo_opciones(&arreglo_opciones);
}
