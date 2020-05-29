; ce programme est pour l'assembleur RealView (Keil)
	thumb
	area	madata, data, readwrite
	extern 	etat
		
	area	moncode, code, readonly
	include etat.inc
	extern  offset
	extern  v_max
	export timer_callback
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
	
	mov r3, #0
	mov r12, #0
	push {r4, r5, r6}
	ldr r6, =val_N
	sub r6, r6, #1
	
	
	; On charge la valeur x(i)
for	ldrsh r4, [r0, r3, LSL #1]
	; On enregistre ponctuellement le décalage dans R5
	mul r5, r1, r3
	and r5, r5, r6
	; On charge la valeur cos(i)/sin(i)
	ldrsh r5, [r2, r5, LSL #1]
	
	; On fait le produit qu'on somme avec R12
	mla r12, r4, r5, r12
	add r3, r3, #1
	
	cmp r3, r6
	bls for	
	
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
	
	push {r4, r5, lr}
	
	mov r4, #0
	mov r5, #0
	
	ldr r2, =TabCos
	bl calc_ImRe
	smlal r5, r4, r12, r12
	
	ldr r2, =TabSin
	bl calc_ImRe
	smlal r5, r4, r12, r12
	
	mov r0, r4
	pop {r4, r5, lr}
	bx lr
	endp 
		
TIM3_CCR3	equ	0x4000043C	; adresse registre PWM
		
timer_callback proc
	
	; Push des registres suplémentaires nécéssaires
	push {r4}
	
	; Chargement des valeurs de offset et v_max, utilises pour l'adaptation a la plage d'entrée de la PWM
	ldr	r0, =offset
	ldrh r1, [r0]
	ldr r0, =v_max
	ldrh r2, [r0]
	
	; Chargement des données de l'état
	ldr r0, =etat
	ldr r12, [r0, #E_POS]
	ldr r4, [r0, #E_TAI]
	; Test si l'on a fini de jouer le morceau
	cmp r4, r12
	bls fin_morceau
	; Si morceau non fini
	; Incrémentation de la position
	add r3, r12, #1
	str r3, [r0, #E_POS]
	; Fin du chargement des données
	ldr r3, [r0, #E_SON]
	ldr r4, [r0, #E_RES]
	; Lecture de l'échantillon
	ldrh r0, [r3, r12, LSL #1]
	; Décalage (La valeur donnée doit etre positive)
	add r0, r0, r1
	mov r3, #0x0000FFFF
	and r0, r0, r3
	; Adaptation a la valeur maximale de résolution
	mul r0, r0, r4
	sdiv r0, r0, r2
	; Envoi de l'échantillon
	ldr r1, =TIM3_CCR3
	str r0, [r1]
	b fin_prog
fin_morceau	
	; Si morceau fini
	mov r0, #0
	ldr r1, =TIM3_CCR3
	str r0, [r1]
	
	
fin_prog	
	pop {r4}
	
	bx	lr
	endp

	end