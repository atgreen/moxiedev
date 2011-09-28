.text
	xor	$r0, $r0 # Zero out $r0
	mov	$r1, $r0
	mov	$r2, $r0
	mov	$r3, $r0
	mov	$r4, $r0
loop:	inc	$r0, 0x1 # Increment $r0
	inc	$r1, 0x1
	inc	$r2, 0x1
	inc	$r3, 0x1 
	inc	$r4, 0x1 
	jmpa	loop+0x1000 # Offset hack
	nop
	nop
	

	
	
