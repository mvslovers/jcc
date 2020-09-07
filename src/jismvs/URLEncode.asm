*
* JIS MVS3.8j WWW Application Server.  (C)2004 Jason Paul Winter,
*                                                All Rights Reserved.
*
* jasonwinter@hotmail.com, use "Subject" wisely - spam gets deleted
*                                                 without being read.
*
*

URLSearchE Rsect
URLSearchE Alias C'URLSearchE'
*int URLSearchE (char * buffer, int length);
*// Returns new position to replace 'at'
*
*Unsafe: (' 'space) < > " # % { } | \ ^ ~ [ ] `
*- and a plus '+' becomes a space too.
*
* ASCII'c'EBCDIC...Hex:
*    20' '40 - Space
*    3C'<'4C
*    3E'>'6E
*    22'"'7F
*    23'#'7B
*    25'%'6C
*    7B'{'C0
*    7D'}'D0
*    7C'|'4F - 037|1047 and '|'BB - 500)
*    5C'\'E0
*   5E('^'5F - 500|1047 and '^'B0 - 037)
*    7E'~'A1
*   5B('['4A - 500, '['AD - 1047 and '['BA - 037)
*   5D(']'5A - 500, ']'BD - 1047 and ']'BB - 037)
*    60'`'79
*
*    2B'+'4E
**
*** and just for interest:
**
*   21('!'4F - 500 and '!'5A - 037|1047)
**
*** NewLines ***
**
*   0A(' '25 - 037|500 and ' '15 - 1047)

         USING URLSearchE,R15

         STM   R2,R3,28(R13)     Save caller's registers

         L     R2,0(0,R1)        buffer
         LR    R0,R2
         ICM   R1,15,4(R1)       length
         BZ    Finish

Search   ds    0h
         SLR   R3,R3
         IC    R3,0(0,R2)
         LA    R3,HexTab(R3)
         ICM   R3,1,0(R3)
         BNZ   Finish

         CLI   0(R2),C' '        Look for ' '
         BZ    Founda

*         CLI   0(R2),C'+'
*         BZ    Finish
*         CLI   0(R2),C'<'
*         BZ    Finish
*         CLI   0(R2),C'>'
*         BZ    Finish
*         CLI   0(R2),C'"'
*         BZ    Finish
*         CLI   0(R2),C'#'
*         BZ    Finish
*         CLI   0(R2),C'%'
*         BZ    Finish
*         CLI   0(R2),C'{'
*         BZ    Finish
*         CLI   0(R2),C'}'
*         BZ    Finish
*         CLI   0(R2),C'|'
*         BZ    Finish
*         CLI   0(R2),C'\'
*         BZ    Finish
*         CLI   0(R2),C'^'
*         BZ    Finish
*         CLI   0(R2),C'~'
*         BZ    Finish
*         CLI   0(R2),C'['
*         BZ    Finish
*         CLI   0(R2),C']'
*         BZ    Finish
*         CLI   0(R2),C'`'
*         BZ    Finish

NotFound ds    0h
         LA    R2,1(0,R2)
         BCT   R1,Search

Finish   ds    0h
         LR    R15,R2
         SLR   R15,R0
         LM    R2,R3,28(R13)     Restore caller's registers
         BR    R14               Return

Founda   ds    0h
         MVI   0(R2),C'+'        Make 'Spaces' +'s!
         B     NotFound          and don't stop...

         DROP  R15

*********************************************************************
*
HTMLSearchE ds    0h
            Entry   HTMLSearchE
HTMLSearchE Alias C'HTMLSearchE'

*int HTMLSearchE (char * buffer, int length)
*// Returns new position to work with...

         USING HTMLSearchE,R15

         L     R2,0(0,R1)        buffer
         LR    R0,R2
         ICM   R1,15,4(R1)       length
         BZ    Finish2

Search2  ds    0h
         CLI   0(R2),C'<'
         BZ    Finish2
         CLI   0(R2),C'>'
         BZ    Finish2
         CLI   0(R2),C'&&'
         BZ    Finish2
         CLI   0(R2),C'"'
         BZ    Finish2

         LA    R2,1(0,R2)
         BCT   R1,Search2

Finish2  ds    0h
         LR    R15,R2
         SLR   R15,R0
         BR    R14               Return

         DROP  R15

*********************************************************************

         LtOrg
* 1047:          0 1 2 3 4 5 6 7 8 9 A B C D E F
HexTab   DC    X'00000000000000000000000000000000' 0
         DC    X'00000000000000000000000000000000' 1
         DC    X'00000000000000000000000000000000' 2
         DC    X'00000000000000000000000000000000' 3
         DC    X'000000000000000000000000FF00FFFF' 4 '<' '+' '|'
         DC    X'000000000000000000000000000000FF' 5 '^'
         DC    X'000000000000000000000000FF00FF00' 6 '%' '>'
         DC    X'000000000000000000FF00FF000000FF' 7 '`' '#' '"'
         DC    X'00000000000000000000000000000000' 8
         DC    X'00000000000000000000000000000000' 9
         DC    X'00FF0000000000000000000000FF0000' A '~' '['
         DC    X'00000000000000000000000000FF0000' B ']'
         DC    X'FF000000000000000000000000000000' C '{'
         DC    X'FF000000000000000000000000000000' D '}'
         DC    X'FF000000000000000000000000000000' E '\'
         DC    X'00000000000000000000000000000000' F

                Entry   __jcc_rtab_size
__jcc_rtab_size Alias C'__jcc_rtab_size'
__jcc_rtab_size DC F'2048'         Double JCCs dynamic loader limit.

R0       Equ   0
R1       Equ   1
R2       Equ   2
R3       Equ   3
R4       Equ   4
R5       Equ   5
R6       Equ   6
R7       Equ   7
R8       Equ   8
R9       Equ   9
R10      Equ   10
R11      Equ   11
R12      Equ   12
R13      Equ   13
R14      Equ   14
R15      Equ   15

         End
