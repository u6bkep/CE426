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
	;r12 = i
	;r11 = j
	;r10 = bitmask
	MOV R12, R0
	MOV R11, #0
	
loop1
	LDR R10, =bitmask
	
	
	
	
	END