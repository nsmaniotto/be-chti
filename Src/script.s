; ce programme est pour l'assembleur RealView (Keil)
	thumb
	area	madata, data, readwrite
	extern  offset
	extern  v_max
	extern 	etat
		
	area	moncode, code, readonly
	include etat.inc
	export timer_callback
		
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
	; Si morceau fini, alors on lui affecte resolution/2
	ldr r0, =etat
	ldr r0, [r0, #E_RES]
	lsr	r0, #1 ; on divise resolution (dans r0) par 2 en faisant un décallage de 1 bit à droite
	ldr r1, =TIM3_CCR3
	str r0, [r1]
	
	
fin_prog	
	pop {r4}
	
	bx	lr
	endp

	end