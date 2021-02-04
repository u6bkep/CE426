;**********************************************************************
; CE-426 Real-Time Embedded Systems
; Instructor: Professor Tewolde
; Author: Roelle Cruz, Megha Ramanan, Ben Kepner 
; Lab2: Embedded C & Assembly Programming on Cortex-M3
;
; Description: 
;
; Hardware: none (Program will be run in a simulation environment)
;
; Program input/output: 
;
; Last modified: 01/28/2021 12:20 PM
;**********************************************************************


	AREA data
	
		
bitmask EQU 0x80000000
	
	AREA    |.text|, CODE, READONLY
	EXPORT findHighBitASM
				
findHighBitASM
	;r0 = nelements
	;r1 = arrays[i]
	;r12 = i
	;r11 = j
	;r10 = bitmask
	STM R13, {R4-R11}
	MOV R12, R1
	MOV R11, #0
	;MOV R7, #32
	;MOV R6, #1
	
loop1
	CMP R12, #0        ;handle for loop logic for i
	BGE continue1
	LDM R13, {R4-R11}
	BX LR
continue1
	SUB R12, R12, #1       ;decrement i
	LDR R10, =bitmask      ;reset bitmask to starting value
	LDR R9, [R0, R12, LSL #2]   ;load array[i - 1] into R9
loop2
	CMP R11, #32           ;handle for loop logic for j
	BLS continue2
	MOV R11, #0
	B loop1
continue2
	TST R9, R10            ;perform bitwise AND to se if bit is set
	BEQ else1              ;if bit j is not set, continue looping
	ADD R12, R12, #1       ;compinsate for decrement happening at start of for loop
	LSL R0, R12, #5        ;if we found the bit, calculate answer and return.
	SUB R0, R0, R11
	SUB R0, R0, #1
	LDM R13, {R4-R11}
	BX LR	
else1
	LSR R10, R10, #1       ;shift bitmask right once
	ADD R11, R11, #1       ;increment j
	B loop2                ;return to start of loop2
	
	

	END