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

void insertar_opciones(struct arr * dato, int opt, int cantidad, ...){	
	struct arr s_arr = *dato;

	va_list ap;
	va_start(ap,cantidad);

	s_arr.opcion = opt;
	
	if (opt == 1){
		//char * num = va_arg(ap,char *);
		//int * n = malloc(sizeof(int));
		//*n = atoi(num);
		//s_arr.args = n;
		cadena_a_entero(&(s_arr.args),va_arg(ap,char *));
	}
	if(opt == 2){ 
		struct sub * s = malloc (sizeof(struct sub));
		s->indice = 0;
	
		cadena_a_entero((void *) &(s->inicio),va_arg(ap,char *));
		cadena_a_entero((void *) &(s->fin),va_arg(ap,char *));

		char * buf = va_arg(ap,char *);
		s->texto = malloc(strlen(buf) + 1);
		strcpy(s->texto,buf);
		s_arr.args=s;

	}
	if(opt == 3){
		s_arr.args = NULL;
	}
	if(opt == 4 || opt == 5){
		agregar_cadena(&(s_arr.args),va_arg(ap,char *));
	}
	*dato = s_arr;

	va_end(ap);	
}

void insert_operand(struct arreglo_opciones *a, struct arr *dato){
  	a->tamanio += 1;
	a->opciones = (struct arr *)realloc(a->opciones, a->tamanio * sizeof(struct arr));
	
	(*a).opciones[(*a).ocupado++] = *dato;
}

// Primer recorrido para recuperar los argumentos -f y -o (en caso que haya).
void procesar_args_filenames(struct arreglo_opciones * arreglo_opciones, int argc, char **argv){

	int entrada = 0 ,salida = 0;
	struct arr dato;
	for(int i = 0; i < argc; i++){
		
		// Para agregar -f
		if(strcasecmp (argv[i],"-f") == 0){
			if(entrada == 0){
				insertar_opciones(&dato, 4, 1, argv[i+1]);
				insert_operand(arreglo_opciones, &dato);
				entrada++;
			}
			else{
				terminar_con_error("Solo se puede ingresar una vez el parametro -f [archivo].\n", 1, arreglo_opciones);
			}
		}
	
		// Para agregar -o
		if(strcasecmp (argv[i],"-o") == 0){
			if(salida == 0){
				insertar_opciones(&dato, 5, 1, argv[i+1]);
				insert_operand(arreglo_opciones, &dato);
				salida++;
			}
			else {
				terminar_con_error("Solo se puede ingresar una vez el parametro -o [archivo].\n", 1, arreglo_opciones);
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
		int optindex;	
		for(optindex = 0; optindex < 3; optindex++){	
			if(strcasecmp(argv[i],opc[optindex])== 0){
				if(optindex == 0){		
					insertar_opciones(&dato,  optindex + 1, 1, argv[i+1]);
				}
				if(optindex == 1){
					insertar_opciones(&dato,  optindex + 1, 3, argv[i+1], argv[i+2], argv[i+3]);
				}
				if(optindex == 2){
					insertar_opciones(&dato,  optindex + 1, 1, argv[i+1]);
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

void liberar_arreglo_opciones(struct arreglo_opciones * arreglo_opciones){
	for(int i = 0; i < arreglo_opciones->ocupado ;i++){
		if(arreglo_opciones->opciones[i].args != NULL && arreglo_opciones->opciones[i].opcion != 1 && arreglo_opciones->opciones[i].opcion != 2){
			free(arreglo_opciones->opciones[i].args);
		}

		if(arreglo_opciones->opciones[i].opcion == 2){
			void * s = arreglo_opciones->opciones[i].args;
			struct sub * sub = (struct sub *) s;
			printf("EL NUMERO DE SUB->INICIO : %d\n", (*(size_t *) sub->inicio));
			printf("EL NUMERO DE SUB->FIN : %d\n", (*(size_t *) sub->fin));
			printf("EL TEXTO DE SUB->TEXTO : %s\n", sub->texto);
			free(sub->inicio);
			free(sub->fin);
			free(sub->texto);
			free(sub);
		}

		if(arreglo_opciones->opciones[i].opcion == 1){
			void * n = arreglo_opciones->opciones[i].args;
			int * nump = (int *) n;
			free(nump);
		}
	}
	free((*arreglo_opciones).opciones);
}