;/*****************************************************************************/
; OSasm.s: low-level OS commands, written in assembly                       */
; Runs on LM4F120/TM4C123/MSP432
; Lab 3 starter file
; March 2, 2016
;


        AREA |.text|, CODE, READONLY, ALIGN=2
        THUMB
        REQUIRE8
        PRESERVE8

        EXTERN  RunPt            ; currently running thread
        EXPORT  StartOS
        EXPORT  SysTick_Handler
        IMPORT  Scheduler


; implementation of register state saving, thread switching and register state restoring
SysTick_Handler                ; 1) Saves R0-R3,R12,LR,PC,PSR
    CPSID   I                  ; 2) Prevent interrupt during switch
    PUSH    {R4-R11}           ; 3) Save remaining regs r4-11
    LDR     R0, =RunPt         ; 4) R0=pointer to RunPt, old thread
    LDR     R1, [R0]           ;    R1 = RunPt
    STR     SP, [R1]           ; 5) Save SP into TCB
    PUSH    {R0,LR}            ; Save EXC_RETURN
    BL      Scheduler		   ; Call scheduler 
    POP     {R0,LR}            ; Restore EXC_RETURN
    LDR     R1, [R0]           ; 6) R1 = RunPt, new thread
    LDR     SP, [R1]           ; 7) new thread SP; SP = RunPt->sp;
    POP     {R4-R11}           ; 8) restore regs r4-11
    CPSIE   I                  ; 9) tasks run with interrupts enabled
    BX      LR                 ; 10) restore R0-R3,R12,LR,PC,PSR
	
; implementation of register loading from stack
StartOS
	LDR     R0, =RunPt		   ; Currently running thread which loads addr of RunPt into R0
	LDR     R1, [R0]		   ; R1 = value of RunPt, Loads value of RunPt into R1
	LDR     SP, [R1]		   ; New thread SP; SP = RunPt ->stackPointer; Load thread's SP from TCB into CPU SP
	POP     {R4-R11}		   ; restore regs R4-11 and Pops R4 - R11 from new SP (SP now in the section of R0)
	POP     {R0-R3}			   ; restore regs R0-3 and Pops R0 - R3 from SP (SP = R12)
	POP     {R12}			   ; Pops R12 (SP = LR (R14))
	ADD     SP, SP, #4		   ; discard LR from initial stack menaing it Skips LR by adding SP (so, SP = PC)
						       ; Here, LR is not active as it is the initial run
	POP     {LR}			   ; Start Location, SP = PSR, it Pops PC (R15) into LR
	ADD     SP, SP, #4	       ; Discard PSR by SP (SP = task’s stack)
    CPSIE   I                  ; Enable interrupts at processor level
    BX      LR                 ; start first thread

    ALIGN
    END
