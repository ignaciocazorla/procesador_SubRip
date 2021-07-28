#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "args.h"
#include "funciones.h"



int main(int argc,char * argv[]){
	struct arreglo_opciones arreglo_opciones;
	recuperar_args(&arreglo_opciones, argc, argv);
	printf("salio al main");
	getchar();
	
	//for(int i = 0; i < arreglo_opciones.ocupado;i++){
	//	printf("%d",arreglo_opciones.opciones[i].opcion);
	//}
	process_operation(&arreglo_opciones);

	liberar_mem_args(&arreglo_opciones);
}
