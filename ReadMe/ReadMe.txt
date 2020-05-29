BE-CHTI       OBJECTIF-2        SMANIOTTO-FAURE      2019-2020


NB : Ce document ne présente pas le contenu du code relatif à cet OBJECTIF, celui-ci est suffisament commenté



Quel est l'objectif de ce projet :
	- Utilisation de la fonction calc_M2 pour détecter un tir de pistolet	
	- Lecture régulière dans le buffer et analyse avec calc_M2 grace à un timer et une fonction callback
	- Gestion des scores dans la fonction main


Comment tester cet objectif : 
	- Ouvrir le fichier "Project.uvprojx"
	- Cliquer sur Rebuild
	- Lancer une session de débug
	- Le fichier de configuration de la session de débug a été volontairement laissé, ainsi, tous les éléments à observer sont déjà affichés
	- Sélectionner l'onglet du Logic Analyzer
	- Cliquer sur Run (ou F5)
	- Jouer sur "Out" et "In" dans le menu Zoom du Logic Analyser pour voir de 0s à 5s
	- 6 signaux avancent, il s'agit des points des 6 pistolets
	- Ils sont aussi visibles dans la Watch1 en bas au gauche
	- Résultats attendus : 
		- Le pistolet 1 marque 1 fois (cad le score passe de 0 à 1)
		- Puis le pistolet 2 marque 2 fois (cad le score passe de 0 à 1 puis de 1 à 2)
		- Puis le pistolet 3 marque 3 fois
		- Puis le pistolet 4 marque 4 fois
		- Puis le pistolet 5 marque 5 fois
		- En parallèle des 5 premiers, le pistolet 6 marque 15 fois 

		- NB : Les résultats attendus sont aussi visibles dans les photos "LogicAnalyzer.png" et "Watch1.png"
		- NB : Pour changer les résultats attendus Cf la section "Comment tester d'autres signaux"


Comment tester d'autres signaux : 
	- Sortir de la session débug 
	- Changer le second paramètre de la fonction "Init_TimingADC_ActiveADC_ff( ADC1, 0x52 );" (ligne 78 de principal.c)
	- Trois valeurs usuelles : 
		- 0x52 : 
			- Le pistolet 1 marque 1 fois (cad le score passe de 0 à 1)
			- Puis le pistolet 2 marque 2 fois (cad le score passe de 0 à 1 puis de 1 à 2)
			- Puis le pistolet 3 marque 3 fois
			- Puis le pistolet 4 marque 4 fois
			- Puis le pistolet 5 marque 5 fois
			- En parallèle des 5 premiers, le pistolet 6 marque 15 fois 
		- 0x33 : 
			- Le pistolet 1 marque 1 fois (cad le score passe de 0 à 1)
			- Puis le pistolet 2 marque 2 fois (cad le score passe de 0 à 1 puis de 1 à 2)
			- Puis le pistolet 3 marque 3 fois
			- Puis le pistolet 4 marque 4 fois
			- Puis le pistolet 5 marque 5 fois
			- le pistolet 6 ne marque pas
		- 0x3C : 
			- Personne ne marque

	- Cf le man de la Fake Lib de M.Noullet pour un choix plus fin