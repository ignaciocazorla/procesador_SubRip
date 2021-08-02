#define _GNU_SOURCE

#include "sub.h"
#include "tiempo.h"

// crea la estructura para manejar los datos del archivo
struct arreglo_sub *init_arreglo(struct arreglo_sub *arreglo)
{
	struct arreglo_sub array = *arreglo;
	array.a = NULL;
	array.tamanio = 0;
	array.ocupado = 0;
	*arreglo = array;
}

// inserta un subtítulo en el arreglo de subtítulos en memoria
void insert_sub(struct arreglo_sub *array, struct sub *dato)
{
	// si el arreglo de subtítulos está lleno, aumenta en uno su tamaño
	array->tamanio += 1;
	array->a = (struct sub *)realloc(array->a, array->tamanio * sizeof(struct sub));
	(*array).a[(*array).ocupado++] = *dato;
}

// llena el arreglo inicial con los subtítulos del archivo "entrada"
void inicializar(struct arreglo_sub * arreglo, FILE *entrada)
{
	// inicializa arreglo de subtítulos
	init_arreglo(arreglo);

	struct sub *dato;							// estructura de subtítulo auxiliar
	char *texto = NULL;							// contenido del subtítulo
	size_t len = 0;								// cantidad de caracteres leídos del subtítulo
	int i_hh = 0, i_mm = 0, i_ss = 0, i_ms = 0, // tiempos de inicio de subs
		f_hh = 0, f_mm = 0, f_ss = 0, f_ms = 0; // tiempos de fin de subs

	// aloca el struct auxiliar
	dato = calloc(1, sizeof(struct sub));

	// empieza a leer
	while (fscanf(entrada, "%d\n", &dato->indice) != EOF)
	{
		fscanf(entrada, "%d:%d:%d,%d --> %d:%d:%d,%d\n",
			   &i_hh, &i_mm, &i_ss, &i_ms,
			   &f_hh, &f_mm, &f_ss, &f_ms);

		dato->inicio = tm_to_millisec(i_hh, i_mm, i_ss, i_ms);
		dato->fin = tm_to_millisec(f_hh, f_mm, f_ss, f_ms);

		// aloca el texto de entrada
		dato->texto = (char *) malloc(sizeof(char));
		*(dato->texto) = '\0';

		// concatena hasta leer sólamente \n o \r\n
		while ((len = getline(&texto, &len, entrada)) > 2)
		{
			dato->texto = (char *) realloc(dato->texto,strlen(dato->texto) + len + 1);
			strcat(dato->texto, texto);
		}

		insert_sub(arreglo, dato);
	}

	// libera los punteros
	free(dato);
	if (texto != NULL)
		free(texto);
}

// crea el archivo de salida en base al arreglo de subtitulos
void crear_salida(struct arreglo_sub *array, FILE **salida)
{
	// dos estructuras t_tiempo, para inicio y fin
	t_tiempo *inicio = (t_tiempo *)calloc(1, sizeof(t_tiempo));
	t_tiempo *fin = (t_tiempo *)calloc(1, sizeof(t_tiempo));

	for (int i = 0; i < array->ocupado; i++)
	{
		if(array->a[i].indice != -1){
			millisec_to_tm(array->a[i].inicio, inicio);
			millisec_to_tm(array->a[i].fin, fin);

			fprintf(*salida, "%d\n", array->a[i].indice);
			fprintf(*salida, "%02d:%02d:%02d,%03d --> %02d:%02d:%02d,%03d\n",
					inicio->hh, inicio->mm, inicio->ss, inicio->ms,
					   fin->hh,    fin->mm,    fin->ss,    fin->ms);
			fprintf(*salida, "%s\n", array->a[i].texto);
		}
	}

	free(inicio);
	free(fin);
}
