#include "gassp72.h"
#include "etat.h"
#include "stdlib.h"

// Variables et fonctions relatives a la gestion du Son
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
short int offset = 0x8000;
short int v_max = 0xFFFF;

// Variables et fonctions relatives a la gestion des scores
extern int calc_M2(volatile unsigned short * add_signal, int k);

extern int val_N;

const int nombre_pistolets = 6;

// Tableau dans lequel on enregistre les valeurs de k correspondant au fréquences des pistolets 
const int tab_k[nombre_pistolets] = {17, 18, 19, 20, 23, 24};

// Valeur seuil de M² au dela de laquelle on considère qu'il y a eu tir
const int M2TIR = 2033600;

// Nombre de fois consécutives minimales où l'on doit détecter un tir pour le valider
// Si on le détecte seulement une ou deux fois cela peut provenir d'un bruit 
const int occurence_mini = 3;
// Tableau dans lequel on stockera les occurences
int compteur_occurences[nombre_pistolets];

// Buffer dans lequel on récupèrera les données depuis le CAN
volatile unsigned short * dma_buf;

// Structures pour la gestion des points
//      int points : on enregistre le nombre de points
//      int vient_de_marquer : On s'en sert de booleen (un char aurait en théorie suffit)
//                             Ce booleen est nécessaire pour ne pas compter un tir deux fois 
struct score {
	int points;
	int vient_de_marquer;
};
// Tableau des structures (une pour chaque pistolet
struct score scores[nombre_pistolets];

// Fonction callback lancée pour lire la sortie de l'ADC et calculer la FFT 
void sys_callback(void) {
	// Démarrage DMA pour 64 points
	Start_DMA1(64);
	Wait_On_End_Of_DMA1();
	Stop_DMA1;
	
	int pistolet;
	int retour;
	
	for (pistolet=0; pistolet<nombre_pistolets; pistolet++) {
		// On calcule la FFT pour chaque valeur de k correspondant à un pistolet
		retour = calc_M2(dma_buf, tab_k[pistolet]);
		// Si on detecte une tir (la valeur de la FFT est supérieure au seuil)
		// On enregistre une occurence de tir
		// Sinon on remet les occurences à zero (les occurences successives nous interessent)
		if (retour > M2TIR) {
			compteur_occurences[pistolet]++;
		} else {
			compteur_occurences[pistolet] = 0;
		}
	}
}

int main(void)
{
	// Initialisations des variables, allocation des ressources mémoire
	etat.position = LongueurSon;
	etat.taille = LongueurSon;
	etat.son = Son;
	etat.periode_ticks = 72*PeriodeSonMicroSec;
	
	dma_buf = (volatile unsigned short *)malloc(sizeof(volatile unsigned short) * 64);
	
	int indice_initialisation;
	for (indice_initialisation=0; indice_initialisation<nombre_pistolets; indice_initialisation++) {
		compteur_occurences[indice_initialisation] = 0;
		scores[indice_initialisation].points = 0;
		scores[indice_initialisation].vient_de_marquer = 0;
	}	
	
	// Pour la PWM (rapport entre la periode du son et de la PWM)
	int rapport_periodes = 10;
	
	// activation de la PLL qui multiplie la fréquence du quartz par 9
	CLOCK_Configure();
	// config port PB0 pour être utilisé par TIM3-CH3
	GPIO_Configure(GPIOB, 0, OUTPUT, ALT_PPULL);	
	// PA2 (ADC voie 2) = entrée analog
	GPIO_Configure(GPIOA, 2, INPUT, ANALOG);
	
	// activation ADC, sampling time 1us
	Init_TimingADC_ActiveADC_ff( ADC1, 0x50 );
	Single_Channel_ADC( ADC1, 2 );
	// Déclenchement ADC par timer2, periode (72MHz/320kHz)ticks
	Init_Conversion_On_Trig_Timer_ff( ADC1, TIM2_CC2, 225 );
	// Config DMA pour utilisation du buffer dma_buf
	Init_ADC1_DMA1( 0, dma_buf );
	
	// Config Timer, période exprimée en périodes horloge CPU (72 MHz)
	int SYSTICK_PER = 360000;
	Systick_Period_ff( SYSTICK_PER );
	// enregistrement de la fonction de traitement de l'interruption timer
	// ici le 3 est la priorité, sys_callback est l'adresse de cette fonction, a créér en C
	Systick_Prio_IT( 3, sys_callback );
	SysTick_On;
	SysTick_Enable_IT;
	
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
	
	// Une fois toutes l'initialisation fait, on passe sur la gestion des scores qui se fait a l'infini
	while	(1)
	{
		int indice_pistolet;
		
		// Pour chaque pistolet : 
		// Si on détecte le nombre minimal d'occurence (et que le pistolet ne vient pas de marquer), il y a un tir.
		// Si le nombre d'occurence retombe à 0, le tir est fini, donc on remet vient_de_marquer a FAUX
		for (indice_pistolet=0; indice_pistolet<nombre_pistolets; indice_pistolet++) {
			if (compteur_occurences[indice_pistolet] == occurence_mini && !scores[indice_pistolet].vient_de_marquer) {
				scores[indice_pistolet].points++;
				scores[indice_pistolet].vient_de_marquer = 1;
				etat.position = 0;
			}
			if (compteur_occurences[indice_pistolet] == 0) {
				scores[indice_pistolet].vient_de_marquer = 0;
			}
		}	
	}
}
