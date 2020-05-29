BE-CHTI       OBJECTIF-1        SMANIOTTO-FAURE      2019-2020


NB : Ce document ne présente pas le contenu du code relatif à cet OBJECTIF, celui ci est suffisament commenté



Quel est l'objectif de ce projet :
	- Mise en place de la fonction "int calc_M2(short * add_signal, int k);"
	   
	  Arguments : 
		- short * add_signal : adresse du signal dont on veut calculer la FFT
		- int k              : valeur de k pour laquelle on veut calculer la FFT
	  Retour : 
		- int : la FFT du signal pour la valeur de k (au carré)
	
	- Mise en place du main() afin de tester calc_M2


Comment tester cet objectif : 
	- Ouvrir le fichier "Project.uvprojx"
(1)	- Cliquer sur Rebuild, deux warnings s'affichent, ignorez les
	- Lancer une session de débug
	- Cliquer sur Run (ou F5)
	- Le fichier de configuration de la session de débug a été volontairement laissé, ainsi, tout les points d'arrets sont déjà présents
	- Le programme s'est arreté sur l'instruction 26 du programme.c : "int point_arret = 0;"
	- Il vient d'appeler calc_M2 pour toutes les valeurs de k dont le résultat est stocké dans le tableau M2
	- Il ne reste plus qu'a regarder les valeurs dans M2 dans la fenêtre "Call Stack + Locals"
	- Cliquer sur le + à gauche de M2
	- Faire défiler les valeurs, elles doivent toutes être de presque nulles, sauf 17 et 18 (et 46 et 47 leur reflet)

	- NB : De base, le test se fait sur le fichier signal "f17p30_f18p135.asm"
	- Si vous souhaitez changer le fichier signal :
		- Sortir de la session débug
		- Clic droit sur "f17p30_f18p135.asm" (dans l'arbre du projet depuis Keil)
		- Remove File 'f17p30_f18p135.asm'
		- Valider la suppression
		- Clic droit sur le dossier "Tables" (dans l'arbre du projet depuis Keil)
		- Add Existing Files To Group 'Tables'
		- Aller dans Src
		- Cliquer sur un des trois fichiers signal .asm (ils commencent par un f)
		- Cliquer sur Add
		- Le fichier doit être apparu dans l'arbre
		- Cliquer sur Close
		- Vous avez changé le fichier
		- Repartir à l'étape (1), ATTENTION : le fichier signal a changé les résultats attendus ont changé aussi (Cf section "Comment identifier les résultats attendus")


Comment identifier les résultats attendus : 
	- Le nom du fichier signal est de la forme suivant : fXXpYY.asm
	  (XX et YY représente des nombres)
	  (Il peut y avoir un '-' entre le p et le nombre YY)
	  (Il peut y avoir plusieurs séquences fXXpYY séparées par des '_')
	- Chaque XX après un f indique la valeur de k pour laquelle on attend un résultat non nul
	- Chaque YY après un p indique la puissance du pic attendu (donné à titre indicatif)

	- Exemple : 
		- Nom du fichier : "f1p-56_f4p-568_f25p42.asm" (NB ce fichier n'existe pas)
		- On doit avoir 3 valeurs de k non négligeables : 1, 4 et 25
		- Le pic 4 sera le plus grand, le pic 25 le plus petit et le pic 1 sera entre les deux (568 > 56 > 42)