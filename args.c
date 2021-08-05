#include "args.h"


char *opc[] = {
	"-b",
	"-i",
	"-v",
	"-f",
	"-o"
};

void terminar_con_error(char * mensaje, int status, struct arreglo_opciones * arreglo_opciones){
	fprintf(stderr,mensaje);
	liberar_arreglo_opciones(arreglo_opciones);
	exit(status);
}

static void init_arreglo(struct arreglo_opciones *arreglo_opciones) {
	struct arreglo_opciones a = *arreglo_opciones;
	a.opciones = NULL;
	a.tamanio = 0;
  	a.ocupado = 0;
	*arreglo_opciones = a;
}

void agregar_cadena(void ** s, char * cadena){
	char * t = *s;
	t = malloc(strlen(cadena) + 1);
	strcpy(t,cadena);
	*s = t;
}

void cadena_a_entero(void ** n, char * cadena){
	char * cadena_num = cadena;
	int * num = malloc(sizeof(int));
	*num = atoi(cadena_num);
	*n = num;
}

void agregar_sub(void ** s){
	struct sub * sub = malloc (sizeof(struct sub));
	sub->indice = 0;
	*s = sub;
}

void cadena_a_millisec(size_t * n, char * cadena){
	char * cadena_num = cadena;
	*n = atoi(cadena_num);
}

void insertar_opciones(struct arr * dato, int opt, int cantidad, ...){	
	struct arr s_arr = *dato;

	va_list ap;
	va_start(ap,cantidad);

	s_arr.opcion = opt;

	switch (opt)
	{
		case BORRAR:
			cadena_a_entero(&(s_arr.args),va_arg(ap,char *));
			break;
		case INSERTAR:
			agregar_sub(&s_arr.args);
			cadena_a_millisec((&((struct sub *) s_arr.args)->inicio) ,va_arg(ap,char *));
			cadena_a_millisec((&((struct sub *) s_arr.args)->fin),va_arg(ap,char *));
			agregar_cadena((void *) &(((struct sub *) s_arr.args)->texto),va_arg(ap,char *));
			break;
		case VALIDAR:
			s_arr.args = NULL;
			break;
		case IN: case OUT:
			agregar_cadena(&(s_arr.args),va_arg(ap,char *));
			break;
	}
	
	*dato = s_arr;

	va_end(ap);	
}

void insert_operand(struct arreglo_opciones *a, struct arr *dato){
  	a->tamanio += 1;
	a->opciones = (struct arr *)realloc(a->opciones, a->tamanio * sizeof(struct arr));
	
	(*a).opciones[(*a).ocupado++] = *dato;
}

void agregar_filename(struct arreglo_opciones * a, int * validador, int opt, char * filename, char * err_msg){
	struct arr dato;
	if((*validador) == 0){
		insertar_opciones(&dato, opt , 1, filename);
		insert_operand(a, &dato);
		(*validador)++;
	}
	else{
		terminar_con_error(err_msg, 1, a);
	}
}


// Primer recorrido para recuperar los argumentos -f y -o (en caso que haya).
void procesar_args_filenames(struct arreglo_opciones * arreglo_opciones, int argc, char **argv){

	int entrada = 0 ,salida = 0;
	for(int i = 0; i < argc; i++){

		// Para agregar -f
		if(strcasecmp (argv[i],"-f") == 0){
			agregar_filename(arreglo_opciones, &entrada, IN, argv[i+1],"Solo se puede ingresar una vez el parametro -f [archivo].\n");
		}
		else{ 
			// Para agregar -o
			if(strcasecmp (argv[i],"-o") == 0){
				agregar_filename(arreglo_opciones, &salida, OUT, argv[i+1],"Solo se puede ingresar una vez el parametro -o [archivo].\n");
			}
		}
	}

	if(entrada == 0){	
		terminar_con_error("El argumento -f es obligatorio.\n", 2, arreglo_opciones);
	}
}

// Procesa el resto de los argumentos al programa en un nuevo recorrido.
void procesar_operaciones(struct arreglo_opciones * arreglo_opciones, int argc, char **argv){
	struct arr dato;
	for(int i = 0; i < argc; i++){
		for(int optindex = 0; optindex < 3; optindex++){	
			if(strcasecmp(argv[i],opc[optindex]) == 0){
				switch (optindex)
				{
				case 0: case 2:
					insertar_opciones(&dato, (optindex + 1), 1, argv[i+1]);
					break;
				case 1:
					insertar_opciones(&dato, INSERTAR , 3, argv[i+1], argv[i+2], argv[i+3]);
					break;
				}

				insert_operand(arreglo_opciones, &dato);
			}
		}
	}
}

void recuperar_args(struct arreglo_opciones * arreglo_opciones, int argc, char **argv){	
	init_arreglo(arreglo_opciones);

	procesar_args_filenames(arreglo_opciones, argc, argv);	

	procesar_operaciones(arreglo_opciones, argc, argv);
}

void liberar_opt_insertar(struct arr * dato){
	void * s = dato->args;
	struct sub * sub = (struct sub *) s;
	free(sub->texto);
	free(sub);
}

void liberar_opt_borrar(struct arr * dato){
	void * n = dato->args;
	int * nump = (int *) n;
	free(nump);
}

void liberar_arreglo_opciones(struct arreglo_opciones * arreglo_opciones){
	struct arr arr_element;
	for(int i = 0; i < arreglo_opciones->ocupado ;i++){
		arr_element = arreglo_opciones->opciones[i];

		switch (arr_element.opcion)
		{
			case BORRAR:
				liberar_opt_borrar(&arr_element);
				break;
			case INSERTAR:
				liberar_opt_insertar(&arr_element);
				break;
			case IN: case OUT:
				free(arr_element.args);
				break;
		}
	}
	free((*arreglo_opciones).opciones);
}