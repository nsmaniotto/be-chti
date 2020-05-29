BE-CHTI       OBJECTIF-3        SMANIOTTO-FAURE      2019-2020


NB : Ce document ne présente pas le contenu du code relatif à cet OBJECTIF, celui-ci est suffisament commenté



Quel est l'objectif de ce projet :
	- Jouer un son 	
	- Envoi de manière régulière (période d'échantillonage du son) grâce à un timer et une fonction callback d'un échantillon de son sur la PWM 


Comment tester cet objectif : 
	- Ouvrir le fichier "Project.uvprojx"
	- Cliquer sur Rebuild
	- Lancer une session de débug
	- Le fichier de configuration de la session de débug a été volontairement laissé, ainsi, tous les éléments à observer sont déjà affichés
	- Sélectionner l'onglet du Logic Analyzer
	- Cliquer sur Run (ou F5)
	- Un signal correspondant au son apparaît
	- Résultats attendus : 
		- Le résultat attendu est visible dans la photo "BruitDeVerre.png"
		- Durée du son, 0.5s environ ce qui correspond bien à 5512 échantillons échantillonnés tout les 91µs

	- NB : On peut aussi :
		- Mettre un point d'arrêt sur la fonction callback (première instruction de la fonction)
		- Lancer la session débug
		- Run
		- Reset WatchTime
		- Run
		- Watch time doit être à 91µs, la durée entre deux échantillons
		

NB : Il est possible de changer le fichier son et donc le son joué.
     Néanmoins, cela perd tout son intérêt vu qu'il ne sera pas possible de l'entendre
     C'est pour cela qu'on ne teste que bruitdeverre qui a une forme caractéristique.