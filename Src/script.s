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
	end