/*************************************************************************************/
/*************************************************************************************/
/**NB : LA COMPILATION DE CE FICHIER PRODUIT DEUX WARNINGS, ILS DOIVENT ETRE IGNORES**/
/*************************************************************************************/
/*************************************************************************************/


// Primitive permetant de calculer la valeur de M² (FFT) (sur 64 points) pour une valeur de k (script.s)
extern int calc_M2(short * add_signal, int k);
// Tableau de short contenant les echantillons du signal (f17p30_f18p135.asm)
extern short TabSig[];


int main(void)
{
	// On boucle sur k allant de 0 à 63 (64 points)
	int k = 0;
	// On enregistrera le retour de calc_M2 dans ce tableau
	int M2[64];
	for (k=0; k<64; k++) {
		M2[k] = calc_M2(TabSig, k);
	}
	// Variable inutile, uniquement pour placer un point d'arret
	int point_arret = 0;
	while(1)
	{
	}
}
