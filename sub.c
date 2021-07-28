#define _GNU_SOURCE

#include "sub.h"
#include "tiempo.h"

// crea la estructura para manejar los datos del archivo
struct arreglo_sub *init_arreglo(struct arreglo_sub *array, int initial_size)
{
	array->a = (struct sub *)malloc(initial_size * sizeof(struct sub));
	array->tamanio = initial_size;
	array->ocupado = 0;
	return array;
}

// inserta un subtítulo en el arreglo de subtítulos en memoria
struct arreglo_sub *insert_sub(struct arreglo_sub *array, struct sub *dato)
{
	// si el arreglo de subtítulos está lleno, duplica su tamaño
	if (array->ocupado == array->tamanio)
	{
		array->tamanio *= 2;
		array->a = (struct sub *)realloc(array->a, array->tamanio * sizeof(struct sub));
	}
	// asigna el dato e incrementa la ocupación
	array->a[array->ocupado++] = *dato;
	return array;
}

// llena el arreglo inicial con los subtítulos del archivo "entrada"
struct arreglo_sub *inicializar(FILE *entrada)
{
	// inicializa arreglo de subtítulos
	struct arreglo_sub *arreglo = calloc(1, sizeof(struct arreglo_sub));
	arreglo = init_arreglo(arreglo, 1);


	struct sub *dato;		// subtítulo auxiliar de lectura
	char *texto = NULL;		// string de texto para getline()
	size_t len = 0;			// caracteres leídos por getline()
	int indice;				// índice auxiliar de subtítulos
	int i_hh = 0, i_mm = 0, i_ss = 0, i_ms = 0,		// tiempos de inicio de subs
		f_hh = 0, f_mm = 0, f_ss = 0, f_ms = 0;		// tiempos de fin de subs

	// aloca el struct auxiliar
	dato = malloc(sizeof(struct sub));

	// empieza a leer

	while (fscanf(entrada, "%d\n", &indice) != EOF)
	{
		dato->indice = indice;
		fscanf(entrada, "%d:%d:%d,%d --> %d:%d:%d,%d\n",
			   &i_hh, &i_mm, &i_ss, &i_ms,
			   &f_hh, &f_mm, &f_ss, &f_ms);
		
		// convierte los datos numéricos a milisegundos
		dato->inicio = tm_to_millisec(i_hh, i_mm, i_ss, i_ms);
		dato->fin = tm_to_millisec(f_hh, f_mm, f_ss, f_ms);

		// aloca el texto de entrada
		dato->texto = malloc(sizeof(char) * 100);

		// concatena hasta leer sólamente \n o \r\n 
		while ((len = getline(&texto, &len, entrada)) > 2)
		{
			strcat(dato->texto, texto);
		}
		
		arreglo = insert_sub(arreglo, dato);
	}

	// libera los punteros
	free(dato);
	if(texto != NULL)
		free(texto);

	return arreglo;
}

// crea el archivo de salida en base al arreglo de subtitulos
void crear_salida(struct arreglo_sub *array, FILE **salida)
{
	// dos estructuras t_tiempo, para inicio y fin
	t_tiempo *inicio = (t_tiempo *)calloc(1, sizeof(t_tiempo));
	t_tiempo *fin = (t_tiempo *)calloc(1, sizeof(t_tiempo));
	
	for (int i = 0; i < array->ocupado; i++)
	{
		millisec_to_tm(array->a[i].inicio, inicio);
		millisec_to_tm(array->a[i].fin, fin);

		fprintf(*salida, "%d\n", array->a[i].indice); 
		fprintf(*salida, "%02d:%02d:%02d,%03d --> %02d:%02d:%02d,%03d\n", 
						inicio->hh, inicio->mm, inicio->ss, inicio->ms,
						fin->hh	  , fin->mm   , fin->ss   , fin->ms
				);
		fprintf(*salida, "%s\n", array->a[i].texto);
	}

	free(inicio);
	free(fin);
}
