*
* JIS MVS3.8j WWW Application Server.  (C)2004 Jason Paul Winter,
*                                                All Rights Reserved.
*
* jasonwinter@hotmail.com, use "Subject" wisely - spam gets deleted
*                                                 without being read.
*
*

URLDecode Rsect
URLDecode Alias C'URLDecode'
* int URLDecode (char * buffer, int length); // Returns new length

         USING URLDecode,R15

         STM   R2,R6,28(R13)     Save caller's registers

         L     R2,0(0,R1)        buffer
         LR    R0,R2
         ICM   R1,15,4(R1)       length
         BZ    Finish

Search   ds    0h
         CLI   0(R2),X'25'       Look for ASCII '%'
         BZ    Found
         CLI   0(R2),X'2B'       Look for ASCII '+'
         BZ    Founda

NotFound ds    0h
         LA    R2,1(0,R2)
         BCT   R1,Search

Finish   ds    0h
         LR    R15,R2
         SLR   R15,R0
         LM    R2,R6,28(R13)     Restore caller's registers
         BR    R14               Return

Founda   ds    0h
         MVI   0(R2),X'20'       Make +'s Spaces!
         B     NotFound

Found    ds    0h
         SLR   R5,R5
         SLR   R6,R6
         IC    R5,1(0,R2)
         LA    R5,HexTab1(R5)
         CLI   0(R5),X'FF'
         BZ    NotFound
         O     R6,0(R5)
         SLR   R5,R5
         IC    R5,2(0,R2)
         LA    R5,HexTab2(R5)
         CLI   0(R5),X'FF'
         BZ    NotFound
         O     R6,0(R5)
         SRL   R6,24(0)
         STC   R6,0(0,R2)
         LA    R2,1(0,R2)        Write over these two chars.

         S     R1,Three
         BNH   Finish

         LR    R6,R2             Ready for next char (after copy)

         LR    R3,R1
         LR    R5,R3
         LA    R4,2(0,R2)        Kill the two hex-bytes
         MVCL  R2,R4             Copy bits...

         LR    R2,R6
         B     Search

         DROP  R15
*********************************************************************
*
* EBCDIC Version (Using ASCII decoding, with __ascii_to_ebcdic table
*
URLDecodeE ds    0h
           Entry   URLDecodeE
URLDecodeE Alias C'URLDecodeE'

*int URLDecodeE (char * buffer, int length, char * __ascii_to_ebcdic)
*// Returns new position to work with...

         USING URLDecodeE,R15

         STM   R2,R7,28(R13)     Save caller's registers

         L     R2,0(0,R1)        buffer
         LR    R0,R2
         L     R7,8(0,R1)        __ascii_to_ebcdic table
         ICM   R1,15,4(R1)       length
         BZ    Finish2

Search2  ds    0h
         CLI   0(R2),C'%'        Look for EBCDIC '%'
         BZ    Found2
         CLI   0(R2),C'+'        Look for EBCDIC '+'
         BZ    Founda2

NotFound2 ds    0h
         LA    R2,1(0,R2)
         BCT   R1,Search2

Finish2  ds    0h
         LR    R15,R2
         SLR   R15,R0
         LM    R2,R7,28(R13)     Restore caller's registers
         BR    R14               Return

Founda2  ds    0h
         MVI   0(R2),C' '        Make +'s Spaces!
         B     NotFound2

Found2   ds    0h
         SLR   R5,R5
         SLR   R6,R6
         IC    R5,1(0,R2)
         LA    R5,HexTab3(R5)
         CLI   0(R5),X'FF'
         BZ    NotFound2
         O     R6,0(R5)
         SLR   R5,R5
         IC    R5,2(0,R2)
         LA    R5,HexTab4(R5)
         CLI   0(R5),X'FF'
         BZ    NotFound2
         O     R6,0(R5)
         SRL   R6,24(0)
         IC    R6,0(R6,R7)
         STC   R6,0(0,R2)
         LA    R2,1(0,R2)        Write over these two chars.

         S     R1,Three
         BNH   Finish2

         LR    R6,R2             Ready for next char (after copy)

         LR    R3,R1
         LR    R5,R3
         LA    R4,2(0,R2)        Kill the two hex-bytes
         MVCL  R2,R4             Copy bits...

         LR    R2,R6
         B     Search2

         DROP  R15

*********************************************************************

         LtOrg

Three    DC    F'3'

HexTab1  DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'00102030405060708090FFFFFFFFFFFF'
         DC    X'FFA0B0C0D0E0F0FFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFA0B0C0D0E0F0FFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'

HexTab2  DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'00010203040506070809FFFFFFFFFFFF'
         DC    X'FF0A0B0C0D0E0FFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FF0A0B0C0D0E0FFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'

HexTab3  DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFA0B0C0D0E0F0FFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFA0B0C0D0E0F0FFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'00102030405060708090FFFFFFFFFFFF'

HexTab4  DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FF0A0B0C0D0E0FFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FF0A0B0C0D0E0FFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
         DC    X'00010203040506070809FFFFFFFFFFFF'

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
