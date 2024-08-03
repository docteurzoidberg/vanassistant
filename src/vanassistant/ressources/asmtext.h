#pragma once

#ifdef ARDUINO
#include <avr/pgmspace.h>
#define PROGMEM_ATTRIBUTE PROGMEM
#define READ_BYTE(address) pgm_read_byte_near(address)
#define STRLEN_P(str) strlen_P(str)
#else
#define PROGMEM_ATTRIBUTE
#define READ_BYTE(address) (*(address))
#define STRLEN_P(str) strlen(str)
#endif

const char asm_text_buffer[] PROGMEM_ATTRIBUTE = R"(

;"Frantic Diamonds"
;Written by Six of Style in 6 hours on May 30, 2006
;For the 2006 minigame compo.
;
;Source donated to The Fridge because Judd is my hero.
;
	org $0801
	processor 6502

;============================================================================
;MACROS
	MAC blt
	bcc {1}
	ENDM

	MAC bge
	bcs {1}
	ENDM
;============================================================================
;EQUATES
BULLTEMP = $b0
MOVTMP	 = $b2
MOVTM2   = $b4

BULLET   = $51
PLAYER   = $58
DIAMOND  = $5a
TOPMARK  = $53
BLANK    = $20

;============================================================================
BASIC   ;SYS 2064
	dc.b $0c,$08,$06,$00,$9e,$20,$32,$30
	dc.b $36,$34,$00,$00,$00,$00,$00

START
	lda #$0f
	sta $d418          ;SID Volume to max
	
	lda #$40
	sta difficulty     ;reset difficulty to easy
	
	lda #$00
	tax
START0
	sta $d400,x        ;clear SID (except volume
	inx
	cpx #$17
	bne START0
	
	jsr InitScr        ;Initialize the screen
	jsr PopTop         ;Draw a row of diamonds
	jsr DrawScore      ;draw the initial scoreboard
LOOPACK1 
	lda #$10
	bit $dc00          ;wait for fire button not pressed
	beq LOOPACK1       ;to avoid making bad initial move
LOOP
	dec playerlagtick  ;only read joystick movement periodicly
	bne LOOP1          ;this prevents crazy fast movement

	jsr GetInput       ;read joystick
	lda playerlag      ;reset player lag counter      
	sta playerlagtick
LOOP1	
	jsr DrawPlayer     ;draw player and top pointer

	dec bulltick       ;tick for bullet speed	
	bne LOOP2
	lda bullspeed      ;reset bullet speed counter
	sta bulltick

	lda #BLANK
	jsr DrawBulls      ;erase bullets
	
	jsr MoveBulls      ;move bullets
	
	lda #BULLET
	jsr DrawBulls      ;draw bullets (at new positions)
LOOP2	
	lda #$10           ;wait for fire button up before the player
	bit $dc00          ;can shoot again.  This prevents nasty
	beq LOOP4          ;doubleshots
	lda #$01           
	sta canshoot       ;set shoot enable flag
LOOP4
	dec downtick1      ;dec our subticker
	bne LOOPx          ;Thus each master tick = 255 subticks
	dec downtick       ;dec our master down move ticker
	bne LOOPx
	lda difficulty		;set our delay for the next move down
	sta downtick
	
	lda #BLANK				;erase our bullets
	jsr DrawBulls	
	jsr MoveDown			;move the playfield down
	dec difficulty
	dec difficulty    ;next move will be more difficult
	jsr PopTop        ;populate the top line with diamonds
	jsr CheckBulls    ;check to see if this move has caused any hits
	lda #BULLET       ;redraw our bullets
	jsr DrawBulls
LOOPx
	jsr CheckBottom   ;check for end of game
	bcc LOOPx1
LOOPACK	lda #$10    ;wait for no fire button 
	bit $dc00         ;so it doesn't immediately start a new game
	beq LOOPACK
LOOPLOCK
	ldx #$00          
LOOPL1
	jsr Random
	sta $d400,x       ;make wierd noises
	inx
	cpx #$17
	bne LOOPL1	
	jsr Random				;flash the border
	sta $d020

	lda #$10
	bit $dc00
	bne LOOPLOCK   		;restart on firebutton
	jmp START
LOOPx1
	jsr WaitRast  ;waste a little time
	jmp LOOP


;============================================================================
InitScr ;Setup Screen
	lda #$00
	sta $d020     ;border = black
	sta $d021     ;background = black
	
	lda #$01
	sta $0286     ;char color = white	
	jsr $e544
	ldx #$00
IS0
	lda #$40
	sta $0428,x     ;draw the divider at the top
	lda #DIAMOND
	sta $0450,x     ;draw a row of diamonds
	inx
	cpx #$28
	bne IS0
	rts

;============================================================================
MoveDown ;Move all blocks down and populate the top row with random blocks
	jsr beep
	lda #$07
	sta MOVTMP+1
	sta MOVTM2+1
	lda #$70
	sta MOVTMP
	lda #$98
	sta MOVTM2
	jsr MoveWonk  ;move screen ram down by one line
	lda #$db
	sta MOVTMP+1
	sta MOVTM2+1
	lda #$70
	sta MOVTMP
	lda #$98
	sta MOVTM2
	jsr MoveWonk  ;move color ram down by one line
	rts


;============================================================================
MoveWonk ;move a block of data, expects MOVTMP and MOVTMP2 to be set
	ldx #$00
MW0
	ldy #$00
MW1
	lda (MOVTMP),y
	sta (MOVTM2),y	
	iny
	cpy #$28	;move first row
	bne MW1
	lda MOVTMP
	sec
	sbc #$28
	sta MOVTMP
	bcs MW2
	dec MOVTMP+1    ;sub $28 (40) from source address
MW2
	lda MOVTM2
	sec
	sbc #$28
	sta MOVTM2
	bcs MW3
	dec MOVTM2+1   ;sub $28 (40) from dest address
MW3
	inx
	cpx #21        ;do 21 lines
	bne MW0
	rts

	rts

;============================================================================
PopTop ; Populate top line

	ldx #$00
PT0
	jsr Random   ;get a random number
	and #$10     ;if bit 4 is set   
	bne PT3      ;duplicate the color below us
	             ; otherwise, get a new color
PT5	jsr Random   ;get a random number
	and #$0f     ;mask out bits 4-7
	beq PT3      ;if it's black, dupe the color below us
	sta $d850,x  ;else stash it.
	jmp PT2      ;and go on to the next one
PT3
	;dupe color below
	lda $d878,x  ;get color below us
	beq PT5      ;black?  dammit!  try again
PT2
	jsr Random   ;gen a random number
	cmp #$40     ;is it greater than or equal to $40 (64)?
	blt PT1      ;If not, skip this slot
	lda #DIAMOND ;else 
	sta $0450,x  ;stash a diamond there.
PT1
	inx        
	cpx #$28     ;and carry on like this for the whole line
	bne PT0
	rts


;============================================================================
CheckBottom ; check for lose, sec on lose, clc on n/a
	ldx #$00
CB0
	lda $0750,x    
	cmp #DIAMOND  ;diamond here?
	beq CB1
	inx
	cpx #$28
	bne CB0
	clc
	rts
CB1
	sec
	rts

;============================================================================
GetInput ;reads joystick input
	lda #$01
	bit $dc00
	bne GI0
	inc playercol
	lda playercol
	cmp #$10
	bne GIU0
	lda #$01
	sta playercol
GIU0
	rts
GI0 ;Dn
	lda #$02
	bit $dc00
	bne GI1 ;Left
	dec playercol
	lda playercol
	cmp #$00
	bne GID0
	lda #$0f
	sta playercol
GID0
	rts
GI1; Left
	lda #$04
	bit $dc00
	bne GI2 ;Right
	lda playerx
	beq GIL0
	dec playerx
GIL0
	rts
GI2; Right
	lda #$08
	bit $dc00
	bne GI3;fire
	lda playerx
	cmp #$27
	beq GIR0
	inc playerx
GIR0
	rts
GI3
	lda #$10
	bit $dc00
	bne GIF0
	jsr Shoot	
GIF0
	rts

;============================================================================
DrawPlayer ;draw the player and the pointer
	ldx #$00
DP1
	cpx playerx
	bne DP0
	lda #PLAYER
	sta $07c0,x
	lda #TOPMARK
	sta $0428,x
	lda playercol
	sta $dbc0,x
	sta $d828,x
	jmp DP2
DP0
	lda #BLANK
	sta $07c0,x
	lda #$40
	sta $0428,x
	lda #$01
	sta $dbc0,x
	sta $d828,x
DP2	inx
	cpx #$28
	bne DP1
	rts

;============================================================================
Shoot ;launch a diamond
	lda canshoot
	beq Shootx
	jsr dropsnd
	jsr FreeBull
	bcs Shootx
	;flag this bullet as active
	lda #$01
	sta bullstat,x
	lda playercol
	sta bullcol,x
	;Initial location is player location - $28
	lda #$07
	sta bulladdrsh,x
	lda #$c0
	clc
	adc playerx
	sec
	sbc #$28
	sta bulladdrsl,x
	lda #$00
	sta canshoot
	
Shootx
	rts

	
;============================================================================
dropsnd

	lda #$20
	sta $d40b

	lda #$80
	sta $d407
	lda #$07
	sta $d408

	lda #$24
	sta $d40c
	lda #$00
	sta $d40d

	lda #$21
	sta $d40b
	rts

;============================================================================
beep
	lda #$10
	sta $d404

	lda #$04
	sta $d400
	lda #$08
	sta $d401

	lda #$09
	sta $d405
	lda #$00
	sta $d406

	lda #$11
	sta $d404

	rts

;BULLET ROUTINES====================================================	
FreeBull ;returns sec on no free, x and clc if free
	ldx #$00
FrB0	
	lda bullstat,x
	
	beq FrB1        ;is this bullet slot free?
	inx
	cpx #$06
	bne FrB0
	sec
	rts
FrB1
	clc
	rts

;============================================================================
MoveBulls ;each bullet moves to bulladdr-40 unless bulladdr-40
	  ;contains a diamond
	ldx #$00
MoVB0
	lda bullstat,x
	beq MoVB1

	;get target addy of curr addy - $28
	lda bulladdrsh,x
	sta BULLTEMP+1
	lda bulladdrsl,x
	sec
	sbc #$28
	sta BULLTEMP
	bcs MoVB2
	dec BULLTEMP+1
MoVB2
	;check target addy for block
	ldy #$00
	lda (BULLTEMP),y

	cmp #DIAMOND
	beq MoVB4
	;or top
	lda BULLTEMP+1
	cmp #$04
	bne MoVB3
	lda BULLTEMP
	cmp #$50
	bge MoVB3

MoVB4	jsr ProcBull ;Process this bullet landing at it's current loc
	
MoVB3
	lda BULLTEMP
	sta bulladdrsl,x
	lda BULLTEMP+1
	sta bulladdrsh,x
MoVB1
	inx
	cpx #$06
	bne MoVB0	
	rts

;============================================================================
DrawBulls ;a=BLANK erase a=BULLET draw (self modifying routine)
	tay
	ldx #$00
DBl0
	lda bullstat,x
	beq DBl1
	lda bulladdrsh,x
	sta DBl2+2
	clc
	adc #$d4
	sta DBl3+2
	lda bulladdrsl,x
	sta DBl2+1
	sta DBl3+1
	tya
DBl2	sta $0400      ;(this address gets modified) store bullet
	lda bullcol,x
DBl3	sta $d800      ;(this address gets modified) store color
DBl1
	inx
	cpx #$06
	bne DBl0
	rts

;============================================================================
ProcBull ;bull number in x
	lda #$00
	tay
	sta bullstat,x    ;bullet is now inactive

	jsr GetBullAd     ;get its address in BULLTEMP

	lda #DIAMOND
	sta (BULLTEMP),y  ;store a diamond there
	lda BULLTEMP+1
	clc
	adc #$d4
	sta BULLTEMP+1
	lda bullcol,x
	sta (BULLTEMP),y  ;store bullet color

	lda BULLTEMP+1
	sec
	sbc #$d4
	sta BULLTEMP+1    ;reset BULLTEMP

	jsr CheckBull     
	
	rts

;============================================================================
CheckBulls ;check each bullet for a match
	ldx #$00
CBs0
	jsr GetBullAd   ;get bullet address in BULLTEMP
	jsr CheckBull   ;check it to see if it has landed
	bcc CBs1        
	lda #$00        ;if it has, deactivate it.
	sta bullstat,x
CBs1
	inx
	cpx #$06
	bne CBs0
	rts

;============================================================================
CheckBull ;Bull# in x, bulladd in BULLTEMP, returns carry if killed

	jsr CheckUp
	bcc CBlx
	
	;if they match, kill both, follow on up
CheckLoop
	jsr CheckUp
	bcc CBlxx
	jmp CheckLoop	
CBlxx
	;finally, kill our bullet
	jsr GetBullAd
	jsr BullBlank
	
	sec
	rts
CBlx
	clc
	rts

;============================================================================
CheckUp ;checks bullad-$28 for diamond of bullcol,x sec on found

	;If the next slot up is a diamond
	lda BULLTEMP
	sec
	sbc #$28
	sta BULLTEMP
	bcs CU0
	dec BULLTEMP+1
CU0
	lda (BULLTEMP),y
	cmp #DIAMOND
	bne CUx
	;and it is our color
	lda BULLTEMP+1
	clc
	adc #$d4
	sta BULLTEMP+1
	lda (BULLTEMP),y
	and #$0f
	cmp bullcol,x

	bne CUx
	;blank it
	lda BULLTEMP+1
	sec
	sbc #$d4
	sta BULLTEMP+1
	jsr BullBlank
	;inc score
	jsr incscore
	;and return
	sec
	rts
CUx
	clc
	rts

;============================================================================
GetBullAd ;load BULLTEMP with bulladdr x
	lda bulladdrsh,x
	sta BULLTEMP+1
	lda bulladdrsl,x
	sta BULLTEMP
	rts

;============================================================================
BullBlank  ;draw a blank space at (BULLTEMP)
	lda #BLANK
	sta (BULLTEMP),y
	rts

;============================================================================
Random   ;returns a semi-random number in a
	lda $dc04
	eor $dc05
	eor $dd04
	adc $dd05
	eor $dd06
	eor $dd07
	rts

;============================================================================
WaitRast           ;waste a rasterline or so of time
	ldy $d012
WV0
	cpy $d012
	beq WV0
	rts

;============================================================================
incscore          ;Increments score by running through the digits
	txa
	pha
	
	ldx #$06
incs0
	inc $0405,x
	lda $0405,x
	cmp #$3a     ;overflow?
	bne isx      ;no, we're done.
 	lda #$30     ;then set to 0 and inc the previous one.
	sta $0405,x
	dex
	bne incs0
isx
	pla
	tax
	rts

;============================================================================
DrawScore         ;Draw initial SCORE:000000
	ldx #$00
DrSc0
	lda scorestr,x  
	sta $0400,x
	inx
	cpx #$0c
	bne DrSc0
	rts

;============================================================================
DATA

scorestr	dc.b $13,$03,$0f,$12,$05,$3a,$30,$30,$30,$30,$30,$30

playerx		dc.b $00 ;x position of player
playercol	dc.b $01 ;current color of player
playerlag	dc.b $20 ;lag of player movement
playerlagtick	dc.b $04 ;ticker for player movement

difficulty	dc.b $40 ;ticker reset value for down move
downtick	dc.b $10 ;counter for down move
downtick1	dc.b $ff ;sub-ticker for down move

bulladdrsh	dc.b $00,$00,$00,$00,$00,$00 ;hi-byte of bullet screen addy
bulladdrsl	dc.b $00,$00,$00,$00,$00,$00		

bullstat	dc.b $00,$00,$00,$00,$00,$00 ;lo-byte of bullet screen addy
bullcol		dc.b $00,$00,$00,$00,$00,$00

bullspeed	dc.b $20 ;lag for bullet movement
bulltick	dc.b $20 ;ticker for bullet movement

canshoot	dc.b $01 ;flag to allow player to shoot


)";