#include "gassp72.h"
#include "stdlib.h"

extern int calc_M2(volatile unsigned short * add_signal, int k);

const int nombre_pistolets = 6;
// Tableau dans lequel on enregistre les valeurs de k correspondant au fr�quences des pistolets 
const int tab_k[nombre_pistolets] = {17, 18, 19, 20, 23, 24};

// Valeur seuil de M� au dela de laquelle on consid�re qu'il y a eu tir
const int M2TIR = 2033600;

// Nombre de fois cons�cutives minimales o� l'on doit d�tecter un tir pour le valider
// Si on le d�tecte seulement une ou deux fois cela peut provenir d'un bruit 
const int occurence_mini = 3;
// Tableau dans lequel on stockera les occurences
int compteur_occurences[nombre_pistolets];

// Buffer dans lequel on r�cup�rera les donn�es depuis le CAN
volatile unsigned short * dma_buf;

// Structures pour la gestion des points
//      int points : on enregistre le nombre de points
//      int vient_de_marquer : On s'en sert de booleen (un char aurait en th�orie suffit)
//                             Ce booleen est n�cessaire pour ne pas compter un tir deux fois 
struct score {
	int points;
	int vient_de_marquer;
};
// Tableau des structures (une pour chaque pistolet
struct score scores[nombre_pistolets];

// Fonction callback lanc�e pour lire la sortie de l'ADC et calculer la FFT 
void sys_callback(void) {
	// D�marrage DMA pour 64 points
	Start_DMA1(64);
	Wait_On_End_Of_DMA1();
	Stop_DMA1;
	
	int pistolet;
	int retour;
	
	for (pistolet=0; pistolet<nombre_pistolets; pistolet++) {
		// On calcule la FFT pour chaque valeur de k correspondant � un pistolet
		retour = calc_M2(dma_buf, tab_k[pistolet]);
		// Si on detecte une tir (la valeur de la FFT est sup�rieure au seuil)
		// On enregistre une occurence de tir
		// Sinon on remet les occurences � zero (les occurences successives nous interessent)
		if (retour > M2TIR) {
			compteur_occurences[pistolet]++;
		} else {
			compteur_occurences[pistolet] = 0;
		}
	}
}


int main(void)
{
	
	// Allocation d'espace m�moire pour le buffer
	dma_buf = (volatile unsigned short *)malloc(sizeof(volatile unsigned short) * 64);
	
	// Initialisation de tout les tableaux
	int indice_initialisation;
	for (indice_initialisation=0; indice_initialisation<nombre_pistolets; indice_initialisation++) {
		compteur_occurences[indice_initialisation] = 0;
		scores[indice_initialisation].points = 0;
		scores[indice_initialisation].vient_de_marquer = 0;
	}	
	
	// activation de la PLL qui multiplie la fr�quence du quartz par 9
	CLOCK_Configure();
	// PA2 (ADC voie 2) = entr�e analog
	GPIO_Configure(GPIOA, 2, INPUT, ANALOG);

	// activation ADC, sampling time 1us
	Init_TimingADC_ActiveADC_ff( ADC1, 0x52 );
	Single_Channel_ADC( ADC1, 2 );
	// D�clenchement ADC par timer2, periode (72MHz/320kHz)ticks
	Init_Conversion_On_Trig_Timer_ff( ADC1, TIM2_CC2, 225 );
	// Config DMA pour utilisation du buffer dma_buf
	Init_ADC1_DMA1( 0, dma_buf );

	// Config Timer, p�riode exprim�e en p�riodes horloge CPU (72 MHz)
	int SYSTICK_PER = 360000;
	Systick_Period_ff( SYSTICK_PER );
	// enregistrement de la fonction de traitement de l'interruption timer
	// ici le 3 est la priorit�, sys_callback est l'adresse de cette fonction, a cr��r en C
	Systick_Prio_IT( 3, sys_callback );
	SysTick_On;
	SysTick_Enable_IT;
	
	// Une fois toutes l'initialisation fait, on passe sur la gestion des scores qui se fait a l'infini
	while(1)
	{
		int indice_pistolet;
		
		// Pour chaque pistolet : 
		// Si on d�tecte le nombre minimal d'occurence (et que le pistolet ne vient pas de marquer), il y a un tir.
		// Si le nombre d'occurence retombe � 0, le tir est fini, donc on remet vient_de_marquer a FAUX
		for (indice_pistolet=0; indice_pistolet<nombre_pistolets; indice_pistolet++) {
			if (compteur_occurences[indice_pistolet] == occurence_mini && !scores[indice_pistolet].vient_de_marquer) {
				scores[indice_pistolet].points++;
				scores[indice_pistolet].vient_de_marquer = 1;
			}
			if (compteur_occurences[indice_pistolet] == 0) {
				scores[indice_pistolet].vient_de_marquer = 0;
			}
		}	
	}
}
