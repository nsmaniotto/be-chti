#include "gassp72.h"
#include "etat.h"

// Variables externes du fichier Son en ASM (bruitverre.asm)
extern int LongueurSon;
extern int PeriodeSonMicroSec;
extern short Son[];

// La fonction callback du script ASM (script.asm)
// Cette fonction gère le Son
extern void timer_callback(void);

// Variables permetant la gestion du Son (structure def dans etat.h)
// Les constantes d'offset pour acceder au champs en assembleur sont dans le fichier etat.inc
type_etat etat;
// Variables qui permettent de faire matcher les echantillons de son avec la plage d'entree de la PWM
short int offset;
short int v_max;

// Fonctions trouvant l'echantillon min et max du SON
short int find_min(short Son[], int taille) {
	int index_min = 0;
	if (taille > 0) {
		int i;
		for (i=1; i<taille; i++) {
			if (Son[index_min] > Son[i]) {
				index_min = i;
			}
		}
	} else {
		index_min = -1;
	}
	
	return Son[index_min];
}

short int find_max(short Son[], int taille) {
	int index_max = 0;
	if (taille > 0) {
		int i;
		for (i=1; i<taille; i++) {
			if (Son[index_max] < Son[i]) {
				index_max = i;
			}
		}
	} else {
		index_max = -1;
	}
	
	return Son[index_max];
}


int main(void)
{
	// Initialisation de la variable etat
	etat.position = 0;
	etat.taille = LongueurSon;
	etat.son = Son;
	etat.periode_ticks = 72*PeriodeSonMicroSec;
	
	offset = - find_min(etat.son, etat.taille);
	v_max = find_max(etat.son, etat.taille) + offset;
	
	// Rapport entre les periodes du SON et de la PWM
	int rapport_periodes = 10;
	
	// activation de la PLL qui multiplie la fréquence du quartz par 9
	CLOCK_Configure();
	// config port PB0 pour être utilisé par TIM3-CH3
	GPIO_Configure(GPIOB, 0, OUTPUT, ALT_PPULL);
	// config TIM3-CH3 en mode PWM
	etat.resolution = PWM_Init_ff( TIM3, 3, etat.periode_ticks/rapport_periodes) -1;
	// initialisation du timer 4
	// Periode_en_Tck doit fournir la durée entre interruptions,
	// exprimée en périodes Tck de l'horloge principale du STM32 (72 MHz)
	Timer_1234_Init_ff( TIM4, etat.periode_ticks );
	// enregistrement de la fonction de traitement de l'interruption timer
	// ici le 2 est la priorité, timer_callback est l'adresse de cette fonction, a créér en asm,
	// cette fonction doit être conforme à l'AAPCS
	Active_IT_Debordement_Timer( TIM4, 2, timer_callback );
	// lancement des timers
	Run_Timer( TIM3 );
	Run_Timer( TIM4 );
	
while	(1)
	{
	}
}
