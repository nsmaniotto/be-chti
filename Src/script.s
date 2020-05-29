; ce programme est pour l'assembleur RealView (Keil)
	thumb
	area	moncode, code, readonly
	export 	calc_M2
	export	val_N
	import 	TabSin
	import 	TabCos
	
val_N	equ	 0x40
	
calc_ImRe	proc
	; arguments : 
	;     			RO = @Signal
	;     			R1 = k
	;     			R2 = @Table sin/cos
	; retour : 
	;     			R12 = Valeur de Im/Re
	
	; On enregistre la valeur du compteur de boucle dans R3
	; On enregistre la valeur du signal dans R4
	; On enregistre la valeur du cos ou sin dans R5
	; On enregistre la valeur du masque dans R6
	; On enregistre la somme dans R12
	; ON NE MODIFIE JAMAIS R0, R1, R2, pour simplifier le code de calc_M2
	
	; Initialisation de la somme et du compteur à 0
	mov r3, #0
	mov r12, #0
	; Push des registres (autre que R0, R1, R2, R3 et R12) que l'on utilisera
	push {r4, r5, r6}
	; Un "MOV r6, #0x40" pourrait suffire, mais on prefere privilegier la clarté avec une constante
	ldr r6, =val_N
	; Pour effectuer une boucle de 64 points en commencant à 0, on va jusqu'a 63, donc N-1
	; De plus R6 pourra aussi servir de masque (cad modulo) au produit k*i
	sub r6, r6, #1
	
	
	; On charge la valeur x(i)
for	ldrsh r4, [r0, r3, LSL #1]
	; On enregistre ponctuellement le décalage dans R5 => (k*i)%63
	mul r5, r1, r3
	and r5, r5, r6
	; On charge la valeur cos(i) ou sin(i), la differenciation est faite en fontion de l'adresse passée dans R2 lors de l'appel
	ldrsh r5, [r2, r5, LSL #1]
	
	; On fait le produit qu'on somme avec R12 => R12 += cos(i)*x(i) ou sin(i)*x(i)
	mla r12, r4, r5, r12
	
	; Incrémentation du compteur de boucle
	add r3, r3, #1
	
	; Si i<=63, on retourne au label for
	cmp r3, r6
	bls for	
	
	; On restaure les registres
	pop {r4, r5, r6}
	bx	lr
	endp
		
		
		
calc_M2	proc
	; arguments : 
	;     			RO = @Signal
	;     			R1 = k
	; retour : 
	;     			R0 = Valeur de M2
	
	; On recupere Im/Re dans R12
	; On enregistre M2 (poids fort) dans R4
	; On enregistre M2 (poids faible) dans R5
	
	; On push les registres nécessaires (plus lr puisque l'on va faire un BL)
	push {r4, r5, lr}
	
	; Initialisation des registres stockant la somme
	mov r4, #0
	mov r5, #0
	
	; Chargement de l'adresse du cos
	ldr r2, =TabCos
	; Calcul de Re(k), la valeur sera retournée dans R12
	bl calc_ImRe
	; Somme += Re(k)²
	; Avec somme = 0
	; Donc Somme = Re(k)²
	smlal r5, r4, r12, r12
	
	; Chargement de l'adresse du sin
	ldr r2, =TabSin
	; Calcul de -Im(k), la valeur sera retournée dans R12
	bl calc_ImRe
	; Somme += (-Im(k))² 
	; <=> Somme += Im(k)²
	; Avec somme = Re(k)²
	; Donc Somme = Re(k)² + Im(k)²
	smlal r5, r4, r12, r12
	
	; On place les bits de poids fort de Re(k)² + Im(k)² dans R0
	mov r0, r4
	
	; On pop ce qu'on avait push
	pop {r4, r5, lr}
	bx lr
	endp 
	end