#include "sub.h"
#include "tiempo.h"

//quiero crear la estructura para manejar los datos del archivo
struct arreglo_sub *init_arreglo(struct arreglo_sub *a, int initial_size)
{
	a->a = (struct sub *)malloc(initial_size * sizeof(struct sub));
	a->tamanio = initial_size;
	a->ocupado = 0;
	return a;
}

struct arreglo_sub *insert_sub(struct arreglo_sub *a, struct sub *dato)
{
	if (a->ocupado == a->tamanio)
	{
		a->tamanio *= 2;
		a->a = (struct sub *)realloc(a->a, a->tamanio * sizeof(struct sub));
	}
	a->a[a->ocupado++] = *dato;
	return a;
}

struct arreglo_sub *inicializar(FILE *entrada)
{
	struct arreglo_sub *arreglo = calloc(1, sizeof(struct arreglo_sub));
	arreglo = init_arreglo(arreglo, 1);
	struct sub *dato;
	char dato1[100];
	int indice;
	int i_hh = 0, i_mm = 0, i_ss = 0, i_ms = 0,
		f_hh = 0, f_mm = 0, f_ss = 0, f_ms = 0;

	//empiezo a leer

	while (fscanf(entrada, "%d\n", &indice) != EOF)
	{
		dato = malloc(sizeof(struct sub));
		dato->indice = indice;
		fscanf(entrada, "%d:%d:%d,%d --> %d:%d:%d,%d\n",
			   &i_hh, &i_mm, &i_ss, &i_ms,
			   &f_hh, &f_mm, &f_ss, &f_ms);
		dato->inicio = tm_to_millisec(i_hh, i_mm, i_ss, i_ms);
		dato->fin = tm_to_millisec(f_hh, f_mm, f_ss, f_ms);
		dato->texto = malloc(sizeof(char) * 100);
		while (3 != strcasecmp((fgets(dato1, 100, entrada)), "\n"))
		{
			//aca podria poner una sentencia de control para que haga realloc de ser necesario	!!!!

			strcat(dato->texto, dato1);
		}
		printf("%s", dato->texto);
		arreglo = insert_sub(arreglo, dato);
	}
	printf("Salio");
	getchar();

	return arreglo;
}

void crear_salida(struct arreglo_sub *sub, FILE **salida)
{
	// dos estructuras t_tiempo, para inicio y fin
	t_tiempo *inicio = (t_tiempo *)calloc(1, sizeof(t_tiempo));
	t_tiempo *fin = (t_tiempo *)calloc(1, sizeof(t_tiempo));
	
	for (int i = 0; i < sub->ocupado - 1; i++)
	{
		inicio = millisec_to_tm(sub->a[i].inicio);
		fin = millisec_to_tm(sub->a[i].fin);

		fprintf(*salida, "%d\n", sub->a[i].indice); 
		fprintf(*salida, "%02d:%02d:%02d,%03d --> %02d:%02d:%02d,%03d\n", 
						inicio->hh, inicio->mm, inicio->ss, inicio->ms,
						fin->hh	  , fin->mm   , fin->ss   , fin->ms
				);
		fprintf(*salida, "%s\n", sub->a[i].texto);
	}
}
