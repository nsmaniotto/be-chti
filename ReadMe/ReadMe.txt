BE-CHTI       OBJECTIF-4        SMANIOTTO-FAURE      2019-2020


NB : Ce document ne présente pas le contenu du code relatif à cet OBJECTIF, celui-ci est suffisament commenté



Quel est l'objectif de ce projet :
	- Utilisation de la fonction calc_M2 pour détecter un tir de pistolet	
	- Lecture régulière dans le buffer et analyse avec calc_M2 grâce à un timer et une fonction callback
	- Gestion des scores dans la fonction main
	- Un son est lancé dès qu'un tir est détecté


Comment tester cet objectif : 
	- Ouvrir le fichier "Project.uvprojx"
	- Cliquer sur Rebuild
	- Lancer une session de débug
	- Le fichier de configuration de la session de débug a été volontairement laissé, ainsi, tous les éléments à observer sont déjà affichés
	- Sélectionner l'onglet du Logic Analyzer
	- Cliquer sur Run (ou F5)
	- Jouer sur "Out" et "In" dans le menu Zoom du Logic Analyser pour voir de 0s à 1.2s
	- 7 signaux sont visibles, le premier correspond au son, les 6 autres aux points des 6 pistolets
	- Les points sont aussi visibles dans la Watch1 en bas au gauche
	- Résultats attendus : 
		- Seul le pistolet 6 marque, à chaque point, un son est emis

		- NB : Les résultats attendus sont aussi visibles dans la photo "LogicAnalyzer.png"
		- NB : Le son est BruitDeVerre, mais il a été raccourci pour éviter les chevauchements
		- NB : Pour changer les résultats attendus Cf la section "Comment tester d'autres signaux"


Comment tester d'autres signaux : 
	- Sortir de la session débug 
	- Changer le second paramètre de la fonction "Init_TimingADC_ActiveADC_ff( ADC1, 0x52 );" (ligne 107 de principal.c)
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

	- ATTENTION : Si les pistolets (1, 2, 3, 4, 5) tirent en même temps que le 6 (0x52 par exemple)
		      Il y aura un recouvrement, lorsque le pistolet 6 tirera le son reprendra du début, et il n'aura pas le temps de se terminer
		      Cf "LogicAnalyzerRecouvrement.png"

	- Cf le man de la Fake Lib de M.Noullet pour un choix plus fin