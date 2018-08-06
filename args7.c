#include "args.h"


char *opc[] = {
	"-b",
	"-i",
	"-v",
	"-f",
	"-o"
};

static void init_arreglo(struct arreglo_opciones *a, int initial_size) {
  a->opciones = (struct arr *)malloc(initial_size * sizeof(struct arr));
  a->tamanio = initial_size;
  a->ocupado = 0;
}

struct arr *insertar_opciones(int opt, int cantidad,...){	
  struct arr *tmp;
	va_list ap;
	va_start(ap,cantidad);
	tmp = (struct arr *) malloc(sizeof(struct arr));
	tmp->opcion = opt;
	if (opt == 1){
		tmp->args = malloc(sizeof(int ));
		char *  dato = va_arg(ap,char *);
		tmp->args = atoi(dato);
	}
	if(opt == 2){ 
		struct sub * s = malloc (sizeof(struct sub));
		s->inicio = atoi(va_arg(ap,char *));
		s->fin = atoi(va_arg(ap,char *));
		char * buf = va_arg(ap,char *);
		s->texto = malloc(strlen(buf));
		strcpy(s->texto,buf);
		tmp->args=s;
	}
	if(opt == 3){
		tmp->args = NULL;
	}
	if(opt == 4 || opt == 5){
		char * buf = va_arg(ap,char *);
		tmp->args = malloc(strlen(buf));
		strcpy(tmp->args,buf);
	}
	va_end(ap);
	return tmp;
	
}

struct arreglo_opciones *insert_operand(struct arreglo_opciones *a, struct arr * dato){
  if (a->ocupado == a->tamanio) {
    a->tamanio *= 2;
    a->opciones = (struct arr *)realloc(a->opciones, a->tamanio * sizeof(dato));
  }
  a->opciones[a->ocupado++] = *dato;
  return a;
}



struct arreglo_opciones *recuperar_args(int argc, char **argv){
	struct arreglo_opciones * arreglo = calloc (1, sizeof(struct arreglo_opciones)); 
	init_arreglo(arreglo,sizeof(struct arr));

	for(int i = 0;  i< argc ; i++){
		printf("%s ", argv[i]);
	}

	int entrada = 0 ,salida = 0;
	//primer recorrido para recuperar -f y -o (en caso que haya). Voy a guardar estos datos en un arreglo separado para poder trabajar cuando necesite con estos datos.
	for(int i = 0; i < argc; i++){
		
		//para agregar -f
		if(strcasecmp (argv[i],"-f") == 0){
			if(entrada == 0){
				
				arreglo = insert_operand(arreglo,insertar_opciones(4,1,argv[i+1]));
				printf("%s",argv[i+1]);
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
					arreglo = insert_operand(arreglo,insertar_opciones(5,1,argv[i+1]));
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

	printf("Nombre del archivo de entrada:%s",(char *) arreglo->opciones->args);
	getchar();
	

	//agarrar el resto de los argumentos e imprimir los valores que tiene el supuesto arreglo de las opciones que llegan como argumentos
	for(int i = 0; i < argc; i++){
	
		int optindex;	
		for(optindex = 0; optindex < 3; optindex++){	
			if(strcasecmp(argv[i],opc[optindex])== 0){
				if(optindex == 0){		
					arreglo = insert_operand(arreglo,insertar_opciones(optindex + 1,1,argv[i+1]));
				}
				if(optindex == 1){
					arreglo = insert_operand(arreglo,insertar_opciones(optindex + 1,3,argv[i+1],argv[i+2],argv[i+3]));
				}
				if(optindex == 2){
					arreglo = insert_operand(arreglo,insertar_opciones(optindex + 1,1,NULL));
				}
			}
		}
	
	}
	printf("El arreglo:\n");
	for(int i = 0; i < arreglo->ocupado;i++){
		printf("%d ",arreglo->opciones[i].opcion);
	}
	printf("\n");
	
	return arreglo;
}
