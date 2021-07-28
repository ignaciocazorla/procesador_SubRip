#include "args.h"


char *opc[] = {
	"-b",
	"-i",
	"-v",
	"-f",
	"-o"
};

static void init_arreglo(struct arreglo_opciones *arreglo_opciones) {
	struct arreglo_opciones a = *arreglo_opciones;
	a.opciones = NULL;
	a.tamanio = 0;
  	a.ocupado = 0;
	*arreglo_opciones = a;
}

void insertar_opciones(struct arr * dato, int opt, int cantidad,...){	
	struct arr s_arr = *dato;
	
	va_list ap;	
	va_start(ap,cantidad);

	s_arr.opcion = opt;
	
	if (opt == 1){
		s_arr.args = malloc(sizeof(int));
		char * num = va_arg(ap,char *);
		s_arr.args = atoi(num);
	}
	if(opt == 2){ 
		struct sub * s = malloc (sizeof(struct sub));
		s->inicio = atoi(va_arg(ap,char *));
		s->fin = atoi(va_arg(ap,char *));
		char * buf = va_arg(ap,char *);
		s->texto = malloc(strlen(buf));
		strcpy(s->texto,buf);
		dato->args=s;
	}
	if(opt == 3){
		s_arr.args = NULL;
	}
	if(opt == 4 || opt == 5){
		char * buf = va_arg(ap,char *);
		s_arr.args = malloc(strlen(buf) + 1);
		strcpy(s_arr.args,buf);
	}
	*dato = s_arr;

	va_end(ap);	
}

void insert_operand(struct arreglo_opciones *a, int opt, int cant_args, char * argv){
	struct arr dato;
  	insertar_opciones(&dato, opt, cant_args, argv);
  	a->tamanio += 1;
	a->opciones = (struct arr *)realloc(a->opciones, a->tamanio * sizeof(struct arr));
	
	(*a).opciones[(*a).ocupado++] = dato;
}

void recuperar_args(struct arreglo_opciones * arreglo_opciones, int argc, char **argv){	
	init_arreglo(arreglo_opciones);

	//for(int i = 0;  i< argc ; i++){
	//	printf("%s ", argv[i]);
	//}

	int entrada = 0 ,salida = 0;
	//primer recorrido para recuperar -f y -o (en caso que haya). Voy a guardar estos datos en un arreglo separado para poder trabajar cuando necesite con estos datos.
	for(int i = 0; i < argc; i++){
		
		//para agregar -f
		if(strcasecmp (argv[i],"-f") == 0){
			if(entrada == 0){

				insert_operand(arreglo_opciones, 4, 1, argv[i+1]);
				//printf("%s",argv[i+1]);
				entrada++;
			}
			else{
				fprintf(stderr,"Solo se puede ingresar una vez el parametro -f [archivo].\n");
				exit (1);
			}
		}
	
		//para agregar -o
		if(strcasecmp (argv[i],"-o") == 0){
				if(salida == 0){
					//arreglo = insert_operand(arreglo,insertar_opciones(5,1,argv[i+1]));
					insert_operand(arreglo_opciones, 5, 1, argv[i+1]);
					salida++;
				}
				else {
					fprintf(stderr,"Solo se puede ingresar una vez el parametro -o [archivo].\n");
					exit (1);
				}
		}
	}
	if(entrada == 0){
		fprintf(stderr,"El argumento -f es obligatorio.\n");
		exit(2);
		
	}

	
	//printf("Nombre del archivo de entrada:%s",(char *) arreglo->opciones->args);
	//getchar();
	
	
	//agarrar el resto de los argumentos e imprimir los valores que tiene el supuesto arreglo de las opciones que llegan como argumentos
	for(int i = 0; i < argc; i++){
	
		int optindex;	
		for(optindex = 0; optindex < 3; optindex++){	
			if(strcasecmp(argv[i],opc[optindex])== 0){
				if(optindex == 0){		
					//arreglo = insert_operand(arreglo,insertar_opciones(optindex + 1,1,argv[i+1]));
					insert_operand(arreglo_opciones, optindex + 1, 1, argv[i+1]);
				}
				if(optindex == 1){
					//arreglo = insert_operand(arreglo,insertar_opciones(optindex + 1,3,argv[i+1],argv[i+2],argv[i+3]));
				}
				if(optindex == 2){
					//arreglo = insert_operand(arreglo,insertar_opciones(optindex + 1,1,NULL));
					insert_operand(arreglo_opciones, optindex + 1, 1, argv[i+1]);
				}
			}
		}
	
	}
	/*
	printf("El arreglo:\n");
	for(int i = 0; i < arreglo->ocupado;i++){
		printf("%d ",arreglo->opciones[i].opcion);
	}
	printf("\n");*/
	
	//return arreglo;
}


void liberar_mem_args(struct arreglo_opciones * arreglo_opciones){
	for(int i = 0; i <= arreglo_opciones->ocupado ;i++){
		if(arreglo_opciones->opciones[i].args != NULL){
			free(arreglo_opciones->opciones[i].args);
		}
	}
	free((*arreglo_opciones).opciones);
}