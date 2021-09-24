;******************************************************************
;*                    CSE 334 Exercise 1                          *
;*           Created by Rumeysa KARAKAVAK on 18.03.2017           *
;*                                                                *
;*  This program reads the string from a specific address and     *
;*  computes according to operator minus or plus                  *
;*                                                                *
;******************************************************************
;*                      Additional Data                           *
;******************************************************************
;*    input must be like that -> 28.33 op 17.28 =                 *
;*    must contain space character end of numbers and operator    *
;*                                                                *
;*    My program cant calculate if integer part has more than two *
;*    character. I tried so hard but failed.                      *
;*                                                                *
;*    Address 1500 has integer part of result                     *
;*    Address 1501 has decimal part of result                     *
;******************************************************************

; export symbols
            XDEF Entry, _Startup  ;
            ABSENTRY Entry        ; 



; Include derivative-specific definitions 
		INCLUDE 'derivative.inc' 

ROMStart      EQU  $4000  ; 

myString      EQU  $1200  ;

result        EQU  $1500  ;

coefficient   EQU  $1548  ;

coefficient1  EQU  $1550  ;
            
length        EQU  $1552  ;

totalLength   EQU  $1554  ;

convert       EQU  $1556  ;

firstInteger  EQU  $1570  ;

firstDecimal  EQU  $1572  ;

secondInteger EQU  $1574  ;

secondDecimal EQU  $1576  ;

integerPart   EQU  $1578  ;

decimalPart   EQU  $1580  ;

DOT           EQU  $1560  ;

MINUS         EQU  $1562  ;

PLUS          EQU  $1564  ;

ASSIGN        EQU  $1566  ;

SPACE         EQU  $1568  ;

hundred       EQU  $1600  ;

warn          EQU  $1602  ;





; variable/data section

            ORG RAMStart
            ORG myString
            FCC "28.33 + 17.28 ="
            ORG coefficient
            DC.B 9 
            ORG coefficient1
            DC.B 99
            ORG length
            DC.B  0
            ORG totalLength
            DC.W  0
            ORG DOT
            DC.B  2EH
            ORG convert
            DC.B  48
            ORG firstInteger
            DC.B  0
            ORG MINUS
            DC.B  2DH
            ORG PLUS
            DC.B 2BH
            ORG ASSIGN
            DC.B 3DH
            ORG SPACE
            DC.B 20H
            ORG hundred
            DC.B 100
            ORG warn
            DC.B 255



; code section
            ORG   ROMStart

;************************************************************************************
                              ;MAIN
Entry:
_Startup:         LDX #myString
                  LDAA myString        ;Acc A = first character of string
                  JSR loopForLength    ;returned length
                  LDX #myString
                  LDAA myString
                  LDAB myString
                  JSR stringToInteger  ; get integer part of first number
                  
                  LDAB totalLength
                  LDX #myString
                  ABX 
                  LDAA 0,X
                  JSR loopForLength1   ; get length of decimal part
                  LDX #myString
                  ABX
                  LDAA 0,X
                  LDAB 0,X
                  JSR stringToInteger1 ; get decimal part of first number
                  
                  LDAB totalLength
                  LDX #myString
                  ABX
                  LDAA 0,X
                  JSR loopForLength2   ; get length of second integer part
                  LDX #myString
                  ABX
                  LDAA 0,X
                  LDAB 0,X
                  JSR stringToInteger2 ; get integer part of second number
                  
                  LDAB totalLength
                  LDX #myString
                  ABX
                  JSR loopForLength3   ; get length of second decimal part
                  LDX #myString
                  ABX
                  LDAA 0,X
                  LDAB 0,X
                  JSR stringToInteger3 ; get decimal part of second number
                  
                  LDX #myString
                  LDAA myString        ;Acc A = first character of string
                  JSR calculate        ;calculates -/+ operation

                  LDAA integerPart
                  LDAB decimalPart

                  STD result           ;stores the result in Acc D
                  LDAA $55  
                  CLRB           
                  STAA PORTB                 
                  JMP endFile
                  
                 
;************************************************************************************
                               ;integer part of first number
loopForLength:      
                  SUBA DOT             ;character - '.'
                  BEQ endLoopForLength ;if character is '.' finish the loop
                  INC length           ;length ++
                  INC totalLength      ;totalLength ++
                  INX
                  LDAA 0,X
                  BRA loopForLength    ;loop again
                  
endLoopForLength: INC totalLength
                  RTS                  ; return length

stringToInteger:  
                  SUBA convert         ;convert character to integer
                  SUBB convert         ;
                  DEC length           ;length --
                  BGT multiply         ;if number has 2 digits
turn:             
                  LDAA  #9
                  STAA coefficient
                  RTS
                  
                  
multiply:         ABA                  ;loop for like 2 to 20
                  DEC coefficient      ;coeff --
                  BEQ takeSecond       ;if coef == 0 branch
                  BRA multiply
                  
takeSecond:       
                  STAA firstInteger
                  INX                  ;get second character 
                  LDAA 0,X
                  LDAB 0,X
                  SUBA convert
                  ADDA firstInteger
                  STAA firstInteger    ;add B to A -> like 20 + 8
                  DEC length           
                  BEQ turn             ;return first integer 
;*********************************************************************************
                            ;decimal part of first number
 loopForLength1:      
                  SUBA MINUS           ;character - '-'
                  BEQ endLoopForLength1;if character is '.' finish the loop
                  LDAA 0,X
                  SUBA PLUS            ;character - '+'
                  BEQ endLoopForLength1;if character is '.' finish the loop
                  LDAA 0,X
                  SUBA SPACE           ;if character is ' ' increment total length
                  BEQ incTotalLength   ;
                  INC length           ;length ++
                  INC totalLength      ;totalLength ++
                  INX
                  LDAA 0,X             ;
                  BRA loopForLength1   ;loop again
                  
incTotalLength:   INC totalLength      ;
                  
endLoopForLength1:INC totalLength
                  RTS                  ; return length
                  
stringToInteger1:  
                  SUBA convert         ;convert character to integer
                  SUBB convert         ;
                  DEC length           ;length --
                  BGT multiply1        ;if number has 2 digits
turn1:            STAA firstDecimal    ;store Acc A to firts decimal
                  LDAA  #9
                  STAA coefficient
                  RTS
                  
                  
multiply1:        ABA                  ;loop for like 2 to 20
                  DEC coefficient      ;coeff --
                  BEQ addSecond1        ;if coef == 0 branch
                  BRA multiply1
                  
addSecond1        STAA firstDecimal    ;store Acc A to first integer
                  INX                  ;get second character 
                  LDAA 0,X
                  SUBA convert         ;convert character to integer
                  LDAB firstDecimal
                  ABA                  ;add B to A -> like 20 + 8
                  DEC length           
                  BEQ turn1             ;return first decimal 
;*********************************************************************************
                            ;integer part of second number
loopForLength2:      
                  SUBA DOT             ;character - '.'
                  BEQ endLoopForLength2;if character is '.' finish the loop
                  LDAA 0,X
                  SUBA SPACE           ;if character is  ' ' increment total length
                  BEQ incrTotalLength
                  INC length           ;length ++
                  INC totalLength      ;totalLength ++
turnBack          INX
                  LDAA 0,X
                  BRA loopForLength2    ;loop again

incrTotalLength:  INC totalLength       ;totalLength++
                  JMP turnBack

                  
endLoopForLength2:INC totalLength
                  RTS                   ;return length
                  
stringToInteger2: SUBA SPACE            ;if character is  ' ' ignore it
                  BEQ  pass
                  LDAA 0, X
                  LDAB 0, X
turnback:       
                  SUBA convert         ;convert character to integer
                  SUBB convert         ;
                  DEC length           ;length --
                  BGT multiply2        ;if number has 2 digits
turn2:            STAA secondInteger
                  LDAA  #9             ;for loop
                  STAA coefficient
                  RTS
                  
                  
multiply2:        ABA                   ;loop for like 2 to 20
                  DEC coefficient       ;coeff --
                  BEQ addSecond2        ;if coef == 0 branch
                  BRA multiply2
                  
addSecond2        STAA secondInteger    ;store Acc A to first integer
                  INX                   ;get second character 
                  LDAA 1,X              ;***does not work if there isnt space character***
                  SUBA convert          ;convert character to integer
                  LDAB secondInteger
                  ABA                   ;add B to A -> like 20 + 8
                  DEC length           
                  BEQ turn2             ;return first integer

pass:             LDAA 1,X              ;ignore space and take the other integer
                  LDAB 1,X
                  JMP turnback
                  
;*********************************************************************************
                            ;decimal part of second number
 loopForLength3:      
                  SUBA ASSIGN           ;character - '.'
                  BEQ endLoopForLength3;if character is '.' finish the loop
                  LDAA 0,X
                  SUBA SPACE           ;if character is ' ' increment total length
                  BEQ inTotalLength     ;
                  INC length           ;length ++
                  INC totalLength      ;totalLength ++
                  INX
                  LDAA 0,X
                  BRA loopForLength3    ;loop again

inTotalLength:   INC totalLength      ;
                                    
endLoopForLength3:INC totalLength
                  RTS                  ; return length
                  
stringToInteger3:  
                  SUBA convert         ;convert to character
                  SUBB convert         ;
                  DEC length           ;length --
                  BGT multiply3        ;if number has 2 digits
turn3             STAA secondDecimal    ;store Acc A to firts Integer
                  LDAA  #9
                  STAA coefficient
                  RTS
                  
                  
multiply3:        ABA                  ;loop for like 2 to 20
                  DEC coefficient      ;coeff --
                  BEQ addSecond3        ;if coef == 0 branch
                  BRA multiply3
                  
addSecond3        STAA secondDecimal    ;store Acc A to first integer
                  INX                  ;get second character 
                  LDAA 0,X
                  SUBA convert         ;convert character to integer
                  LDAB secondDecimal
                  ABA                  ;add B to A -> like 20 + 8
                  DEC length           
                  BEQ turn3             ;return first integer
                  
;*********************************************************************************
                            ;calculate            
                            
calculate:      
                  SUBA MINUS           ;character -- '-'
                  BEQ subtraction      ;if character is '-' branch subtraction
                  LDAA 0,X             ;character -- '+'
                  SUBA PLUS            ;if character is '+' branch addition
                  BEQ addition         
                  INC length           ;length ++
                  INC totalLength      ;totalLength ++
                  INX
                  LDAA 0,X
                  BRA calculate        ;loop again
subtraction:

                  LDAA firstDecimal    ;Acc A = first decimal
                  LDAB secondDecimal   ;Acc B = second decimal
                  SBA                  ;A = A - B
                  BLT  carry           ;if B>A, carry
goBack:           
                  LDAA firstInteger
                  LDAB secondInteger
                  SBA                  ;first integer - second integer
                  STAA integerPart     ;load it integer part 
                  RTS                  ;return
carry:            
                  LDAA firstDecimal    ;Acc A = first decimal
                  ADDA hundred         ;A = A+100
                  SUBA secondDecimal   ;A - second decimal
                  STAA decimalPart     ;Acc A = firstdecimal+100-seconddecimal
                  LDAA firstInteger    ;
                  DECA                 ;first integer - 1 for carry
                  STAA firstInteger
                  JMP  goBack          ; go back

addition:      
                  LDAA firstDecimal    ;Acc A = first decimal
                  LDAB secondDecimal   ;Acc B = second decimal
                  ABA                  ;A=A-B
                  STAA decimalPart   
                  
                  LDAA firstInteger    ;Acc A = first integer
                  LDAB secondInteger   ;Acc B = second integer
                  ABA                  ;A = A + B
                  STAA integerPart     
                  
                  LDAA decimalPart     ;A = decimal part
                  LDAB hundred         ;B = 100
                  SBA                  ;A = A - 100
                  BGT addCarry         ;if positive add carry
return:           SUBA hundred         ;decimal part = A - 100

                  LDAA integerPart
                  LDAB warn            ; if integer part greater than 255
                  SBA                  ; PORTB = FF
                  BGT warnState
                  
finish:           RTS
addCarry:
                  INC integerPart      ;integer part ++
                  STAA decimalPart  
                  JMP  return
                  
warnState:        LDAA $FF
                  STAA PORTB
                  JMP  finish                                    

endFile:           
                  END
                  
                                          

;**************************************************************
;*                 Interrupt Vectors                          *
;**************************************************************
            ORG   $FFFE
            DC.W  Entry           ; Reset Vector
