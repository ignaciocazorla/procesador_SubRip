#include "funciones.h"
#include "sub.h"

void abrir_arch_entrada(struct arreglo_sub * sub, void *filename, FILE *entrada)
{
	entrada = fopen((char *) filename, "r+");
	if (entrada == NULL)
	{
		fprintf(stderr, "No se pudo abrir el archivo %s.\n", (char *)filename);
	}

	inicializar(sub, entrada);
	fclose(entrada);
}

void init_arch_salida(void *filename, FILE **salida)
{
	printf("El archivo de salida va a ser: %s.\nPresione Enter para continuar.\n ", (char *)filename);
	getchar();
	*salida = fopen((char *)filename, "w+");
	if (*salida == NULL)
	{
		fprintf(stderr, "No se pudo abrir el archivo %s.\n", (char *)filename);
	}
}

// Almacena en el archivo de salida las modificaciones realizadas.
void escribir_salida(FILE **salida, struct arreglo_sub *arr_sub)
{
	if (salida != NULL){
		crear_salida(arr_sub, salida);
		fclose(*salida);
	}
}

// Compara el índice buscado con el índice de un subtítulo.
int comparar(const void * key, const void * sub){
	int k = (*(int *) key);
	struct sub * s = (struct sub *) sub;
	if (k == s->indice){
		return 0;
	}else{
		if(k > s->indice){
			return 1;
		}else{
			return -1;
		}
	}
}

// Borra un indice y ordena los siguientes restandoles uno.
void borrar_indice(void *args, struct arreglo_sub *arr_sub)
{
	int indice = (*(int *) args);

	struct sub * s = bsearch(&indice, arr_sub->a, arr_sub->ocupado, sizeof(struct sub), comparar);

	// Se calcula el indice siguiente a s, para poder modificar el resto de 
	// los indices, restandole 1 a cada uno.
	int i = ((s - (arr_sub->a))) + 1;

	if(s != NULL){
		// aplica borrado logico
		s->indice = -1;
		// se modifica el resto de los indices.
		for (; i < arr_sub->ocupado; i++)
		{
			arr_sub->a[i].indice -= 1;
		}
	}else{
		printf("No se encontro el indice a borrar.\n");
	}
}

/*struct arreglo_sub *insertar(void *args, struct arreglo_sub *arreglo)*/
void insertar(void *args, struct arreglo_sub *arreglo)
{

	struct sub *dato = (struct sub *)args;
	int j;

	if (arreglo->ocupado == arreglo->tamanio)
	{
		arreglo->a = (struct sub *)realloc(arreglo->a, arreglo->tamanio * sizeof(struct sub) + 1 * sizeof(struct sub));
	}

	if (dato->inicio <= arreglo->a[0].inicio)
	{
		j = 0;
	}
	else
	{
		j = 1;
		while (j < arreglo->ocupado && dato->inicio > arreglo->a[j].inicio)
		{
			j++;
		}
	}
	if (j < arreglo->ocupado)
	{
		for (int i = arreglo->ocupado; i >= j; i--)
		{
			arreglo->a[i].indice += 1;
			arreglo->a[i + 1] = arreglo->a[i];
		}
		dato->texto = realloc(dato->texto, strlen(dato->texto) + sizeof(char));
		strcat(dato->texto, "\n");
	}

	dato->indice = j + 1;
	arreglo->a[j] = *dato;
	arreglo->ocupado += 1;
}

void validar(struct arreglo_sub *arr_sub)
{

	if ((arr_sub->a[0].indice) != 1)
	{
		error(1, "El primer indice no es 1.");
	}

	printf("%d\n", minimo_duracion_sub(90, 10));
	//se trabaja con un sub
	for (int i = 0; i < arr_sub->ocupado; i++)
	{
		int total_carac = procesar_texto(arr_sub->a[i].texto, arr_sub->a[i].indice);
		if (minimo_duracion_sub(arr_sub->a[i].inicio, arr_sub->a[i].fin) == 1)
		{
			error(arr_sub->a[i].indice, "El subtitulo dura menos de 1 seg.");
		}

		if (maximo_duracion_sub(arr_sub->a[i].inicio, arr_sub->a[i].fin))
		{
			error(arr_sub->a[i].indice, "El subtitulo dura mas de 7 seg.");
		}

		if (chars_por_seg(total_carac, arr_sub->a[i].inicio, arr_sub->a[i].fin))
		{
			error(arr_sub->a[i].indice, "El subtitulo tiene demasiados caracteres por segundo.");
		}
		if (i != arr_sub->ocupado - 1)
		{

			if ((arr_sub->a[i].indice + 1) != arr_sub->a[i + 1].indice)
			{
				error(arr_sub->a[i].indice, "Los indices no son consecutivos ordenados.");
			}

			if (solapados_sub(arr_sub->a[i].fin, arr_sub->a[i + 1].inicio) == 1)
			{
				error(arr_sub->a[i].indice, "El subtitulo esta solapado con el siguiente.");
			}
			else
			{

				if (separacion_sub(arr_sub->a[i].fin, arr_sub->a[i].inicio))
				{
					error(arr_sub->a[i].indice, "Hay menos de 75 ms. entre el subtitulo y el siguiente.");
				}
			}
		}
	}
}

void liberar_arreglo_subtitulos(struct arreglo_sub * arreglo){
	struct arreglo_sub arr_sub = *arreglo;
	for (int i = 0; i < arr_sub.ocupado; i++)
	{
		free(arr_sub.a[i].texto);
	}
	free(arr_sub.a);
}

void process_operation(struct arreglo_opciones *optargs)
{
	FILE *entrada = NULL, *salida = NULL;
	struct arreglo_sub arr_sub;

	for (int i = 0; i < optargs->ocupado; i++)
	{
		printf("Entrada nro: %d", i);
		getchar();
		switch (optargs->opciones[i].opcion)
		{
		case IN:
			printf("Abriendo archivo...\n");
			abrir_arch_entrada(&arr_sub, optargs->opciones[i].args, entrada);
			break;

		case OUT:
			printf("Abriendo archivo de salida...\n");
			init_arch_salida(optargs->opciones[i].args, &salida);
			break;

		case BORRAR:
			if (salida != NULL){
				printf("Borrando...\n");
				borrar_indice(optargs->opciones[i].args, &arr_sub);
			}else{
				printf("No se puede borrar, no hay archivo de salida.\n El argumento -o es obligatorio cuando hay opciones que modifican el archivo de entrada.");
			}
			break;

		case INSERTAR:
			if (salida != NULL){
				printf("Insertando...\n");
				insertar(optargs->opciones[i].args, &arr_sub);
			}else{
				printf("No se puede Insertar, no hay archivo de salida.\n El argumento -o es obligatorio cuando hay opciones que modifican el archivo de entrada.");
			}
			break;

		case VALIDAR:
			printf("Validando...\n");
			validar(&arr_sub);
			break;
		}
	}
	
	escribir_salida(&salida, &arr_sub);

	liberar_arreglo_subtitulos(&arr_sub);
}
