EZASOKET Title 'E Z A S O K E T  - T C P I P '

***********************************************************
* RENT Socket interface.
* ----------------------
* For use with the modified Hercules emulator
*
* This module also has a C wrapper: see socket.c
*
* Copyright (c)2003 Jason Paul Winter, All Rights Reserved.
*
***********************************************************

EZASOKET Csect

         STM   R14,R12,12(R13)
         LA    12,0(,15)

         Using EZASOKET,12

         LR    R10,R1

         L     R2,0(0,R10)
         L     R2,0(0,R2)

         C     R2,INITAPI4
         BNE   NOTINIT

****************************
*** INITAPI
****************************

         SLR   R1,R1

         LA    R7,1(0,0)
         LA    R15,CALLPC
         BALR  R11,R15

         B     EZARETRN

****************************

NOTINIT  DS 0H

         C     R2,GETHOST4
         BNE   NOTGETH

****************************
*** GETHOSTBYNAME
****************************

         L     R1,4(0,R10)
         L     R1,0(0,R1)

         L     R5,8(0,R10)
         
         LA    R7,4(0,0)
         LA    R15,CALLPC
         BALR  R11,R15

         ST    R4,GADDR

         LA    R4,HOSTENT
         L     R1,12(0,R10)
         ST    R4,0(0,R1)

         L     R1,16(0,R10)
         SLR   R15,R15
         ST    R15,0(0,R1)

         B     EZARETRN

****************************

NOTGETH  DS 0H

         C     R2,SOCKET4
         BNE   NOTSOCK

****************************
*** SOCKET
****************************

         SLR   R1,R1

         L     R8,4(0,R10)    Get af address
         L     R8,0(0,R8)     Get af
         SLL   R8,16

         L     R9,8(0,R10)    Get type address
         L     R9,0(0,R9)     Get type
         AR    R8,R9

         L     R9,12(0,R10)   Get protocol address
         L     R9,0(0,R9)     Get protocol

         LA    R7,5(0,0)
         LA    R15,CALLPC
         BALR  R11,R15

         L     R1,20(0,R10)   Get &RetCd
         ST    R4,0(0,R1)

         C     R4,MINUS1
         BNZ   EZARETRN

         SLR   R1,R1

         LA    R7,3(0,0)      No Socket - GetError
         LA    R15,CALLPC
         BALR  R11,R15

         L     R1,16(0,R10)   Get &ErrCd
         ST    R4,0(0,R1)

         B     EZARETRN

****************************

NOTSOCK  DS 0H

         C     R2,BIND4
         BNE   NOTBIND

****************************
*** BIND
****************************

         SLR   R1,R1

         LA    R7,6(0,0)      Function 6: BIND
         L     R8,4(0,R10)    Get SOCKET address
         LH    R9,0(0,R8)     Get SOCKET number
         SLL   R9,16
         AR    R7,R9
         
         L     R9,8(0,R10)    Get NAME address
         L     R8,4(0,R9)     Get IP-ADDRESS
         L     R9,0(0,R9)     Get Family & Port number
         SLL   R9,8
         SRL   R9,8           Remove Length (if present)
         
         LA    R15,CALLPC
         BALR  R11,R15

         L     R1,16(0,R10)   Get &RetCd
         ST    R4,0(0,R1)

         C     R4,MINUS1      Did an error occur?
         BNZ   EZARETRN

         SLR   R1,R1

         LR    R8,R7
         SRL   R8,16          Quick-get-sock

         LA    R7,2(0,0)      Function GetError(sock)
         LA    R15,CALLPC
         BALR  R11,R15

         L     R1,12(0,R10)   Get &ErrCd
         ST    R4,0(0,R1)

         B     EZARETRN

****************************

NOTBIND  DS 0H

         C     R2,CONNECT4
         BNE   NOTCONN

****************************
*** CONNECT
****************************

         LA    R7,2(0,0)      Loop 3 times (wait twice)

CONNECTL DS    0H

         SLR   R1,R1

         SLL   R7,24          Clear 16 'left' and
         SRL   R7,16          Clear 8  'right' = 0000xx00
         LA    R7,7(0,R7)     Function 7: CONNECT
         L     R8,4(0,R10)    Get SOCKET address
         LH    R9,0(0,R8)     Get SOCKET number
         SLL   R9,16
         AR    R7,R9

         L     R9,8(0,R10)    Get NAME address
         L     R8,4(0,R9)     Get IP-ADDRESS
         L     R9,0(0,R9)     Get Family & Port number
         SLL   R9,8
         SRL   R9,8           Remove Length (if present)

         LA    R15,CALLPC
         BALR  R11,R15

         C     R4,MINUS2      Need to WAIT?
         BNZ   CNOWAIT

         SLL   R7,16
         SRL   R7,24
         LTR   R7,R7
         BZ    CWAITLNG

         A     R7,MINUS1
         LTR   R7,R7
         BNZ   SHRTWAIT

         LA    R15,TASKWAIS   Wait a whole second
         BALR  R11,R15        If the initial wait fails

         B     CONNECTL

SHRTWAIT DS    0H

         LA    R15,TASKWAIT   Initially only wait .08 secs
         BALR  R11,R15

         B     CONNECTL

CWAITLNG DS    0H

         L     R4,MINUS1
         L     R1,16(0,R10)   Get &RetCd
         ST    R4,0(0,R1)

         LA    R4,61(0,0)     hECONNREFUSED
         L     R1,12(0,R10)   Get &ErrCd
         ST    R4,0(0,R1)

         B     EZARETRN

CNOWAIT  DS    0H

         L     R1,16(0,R10)   Get &RetCd
         ST    R4,0(0,R1)

         C     R4,MINUS1      Did an error occur?
         BNZ   EZARETRN

         SLR   R1,R1

         L     R8,4(0,R10)    Get SOCKET address
         LH    R8,0(0,R8)     Get SOCKET number

         LA    R7,2(0,0)      Function GetError(sock)
         LA    R15,CALLPC
         BALR  R11,R15

         L     R1,12(0,R10)   Get &ErrCd
         ST    R4,0(0,R1)

         B     EZARETRN

****************************

NOTCONN  DS 0H

         C     R2,LISTEN4
         BNE   NOTLIST

****************************
*** LISTEN
****************************

         SLR   R1,R1

         L     R8,4(0,R10)
         LH    R8,0(0,R8)
         
         L     R9,8(0,R10)
         L     R9,0(0,R9)
         
         LA    R7,8(0,0)
         LA    R15,CALLPC
         BALR  R11,R15

         L     R1,16(0,R10)
         ST    R4,0(0,R1)

         C     R4,MINUS1
         BNZ   EZARETRN

         SLR   R1,R1

         LA    R7,2(0,0)
         LA    R15,CALLPC
         BALR  R11,R15

         L     R1,12(0,R10)
         ST    R4,0(0,R1)

         B     EZARETRN

****************************

NOTLIST  DS 0H

         C     R2,ACCEPT4
         BNE   NOTACCE

****************************
*** ACCEPT
****************************

ACCEPTL  DS 0H

         SLR   R1,R1

         L     R8,4(0,R10)
         LH    R8,0(0,R8)
         
         L     R6,8(0,R10)

         LA    R7,9(0,0)
         LA    R15,CALLPC
         BALR  R11,R15

         C     R4,MINUS2
         BNZ   ANOWAIT

         LA    R15,TASKWAIT
         BALR  R11,R15

         B     ACCEPTL

ANOWAIT  DS 0H

         L     R1,16(0,R10)
         ST    R4,0(0,R1)

         C     R4,MINUS1
         BNZ   EZARETRN

         SLR   R1,R1

         LA    R7,2(0,0)
         LA    R15,CALLPC
         BALR  R11,R15

         L     R1,12(0,R10)
         ST    R4,0(0,R1)

         B     EZARETRN

****************************

NOTACCE  DS 0H

         C     R2,SEND4
         BNE   NOTSEND

****************************
*** SEND
****************************

         L     R1,12(0,R10)
         L     R1,0(0,R1)

         L     R5,16(0,R10)

         L     R8,4(0,R10)
         LH    R8,0(0,R8)

         LA    R7,10(0,0)
         LA    R15,CALLPC
         BALR  R11,R15

         L     R1,24(0,R10)
         ST    R4,0(0,R1)

         C     R4,MINUS1
         BNZ   EZARETRN

         SLR   R1,R1

         LA    R7,2(0,0)
         LA    R15,CALLPC
         BALR  R11,R15

         L     R1,20(0,R10)
         ST    R4,0(0,R1)

         B     EZARETRN

****************************

NOTSEND  DS 0H

         C     R2,RECV4
         BNE   NOTRECV

****************************
*** RECV
****************************

RECVL    DS 0h

         SLR   R1,R1

         L     R8,4(0,R10)
         LH    R8,0(0,R8)

         L     R9,12(0,R10)
         L     R9,0(0,R9)

         L     R6,16(0,R10)

         LA    R7,11(0,0)
         LA    R15,CALLPC
         BALR  R11,R15

         C     R4,MINUS2
         BNZ   RNOWAIT

         LA    R15,TASKWAIT
         BALR  R11,R15

         B     RECVL

RNOWAIT  DS 0H

         L     R1,24(0,R10)
         ST    R4,0(0,R1)

         C     R4,MINUS1
         BNZ   EZARETRN

         SLR   R1,R1

         LA    R7,2(0,0)
         LA    R15,CALLPC
         BALR  R11,R15

         L     R1,20(0,R10)
         ST    R4,0(0,R1)

         B     EZARETRN

****************************

NOTRECV  DS 0H

         C     R2,CLOSE4
         BNE   NOTCLOS

****************************
*** CLOSE
****************************

         SLR   R1,R1

         L     R8,4(0,R10)
         LH    R8,0(0,R8)

         LA    R7,12(0,0)
         LA    R15,CALLPC
         BALR  R11,R15

         B     EZARETRN

****************************

NOTCLOS  DS 0H

         C     R2,EBCDIC24
         BNE   NOTEBCD

****************************
*** EBCDIC2ASCII
****************************

         L     R1,4(0,R10)
         L     R1,0(0,R1)

         L     R5,8(0,R10)
         LR    R6,R5

         LA    R7,13(0,0)
         LA    R15,CALLPC
         BALR  R11,R15

         B     EZARETRN

****************************

NOTEBCD  DS 0H

         C     R2,ASCII24
         BNE   NOTASCI

****************************
*** ASCII2EBCDIC
****************************

         L     R1,4(0,R10)
         L     R1,0(0,R1)

         L     R5,8(0,R10)
         LR    R6,R5

         LA    R7,14(0,0)
         LA    R15,CALLPC
         BALR  R11,R15

         B     EZARETRN

****************************

NOTASCI  DS 0H

         C     R2,IOCTL4
         BNE   NOTIOCT

****************************
*** IOCTL
****************************

         SLR   R1,R1

         LA    R7,15(0,0)
         L     R8,4(0,R10)
         LH    R8,0(0,R8)
         SLL   R8,16
         AR    R7,R8

         L     R8,8(0,R10)
         L     R8,0(0,R8)
         L     R9,12(0,R10)
         L     R9,0(0,R9)

         LA    R15,CALLPC
         BALR  R11,R15

         C     R8,PLUS1
         BNZ   FIONREAD

         SR    R4,R4          FIONBIO can't produce an error
         L     R1,24(0,R10)
         ST    R4,0(0,R1)
         B     EZARETRN

FIONREAD DS    0H

         C     R4,MINUS1
         BZ    IOCTLERR

         L     R1,16(0,R10)   Store 0+ Result
         ST    R4,0(0,R1)

         SR    R4,R4          And no error!
         L     R1,24(0,R10)
         ST    R4,0(0,R1)
         B     EZARETRN

IOCTLERR DS    0H

         L     R1,24(0,R10)
         ST    R4,0(0,R1)

         SLR   R1,R1

         L     R8,4(0,R10)
         LH    R8,0(0,R8)

         LA    R7,2(0,0)      Go back and get the error code
         LA    R15,CALLPC
         BALR  R11,R15

         L     R1,20(0,R10)
         ST    R4,0(0,R1)

         B     EZARETRN

****************************

NOTIOCT  DS 0H

         C     R2,GETSOCK4
         BNE   NOTGETS

****************************
*** GETSOCKETNAME
****************************

         SLR   R1,R1

         L     R8,4(0,R10)
         LH    R8,0(0,R8)

         L     R6,8(0,R10)

         LA    R7,16(0,0)
         LA    R15,CALLPC
         BALR  R11,R15

         L     R1,16(0,R10)
         ST    R4,0(0,R1)

         C     R4,MINUS1
         BNZ   EZARETRN

         SLR   R1,R1

         LA    R7,2(0,0)
         LA    R15,CALLPC
         BALR  R11,R15

         L     R1,12(0,R10)
         ST    R4,0(0,R1)

         B     EZARETRN

****************************

NOTGETS  DS 0H

         C     R2,SELECT4
         BNE   EZARETRN

****************************
*** SELECT
****************************

         SR    R1,R1          Nothing to send
         LA    R7,17(0,0)     Function SELECT
         L     R8,4(0,R10)
         L     R8,0(0,R8)     Get MaxSock+1
         
         LTR   R8,R8
         BZ    EZARETRN
         BM    EZARETRN       Should allow sleep, but no!
         C     R8,PLUS1
         BZ    EZARETRN

         LR    R9,R8          Aux2 = MaxSock+1
         A     R8,MINUS1      Get 'owner' socket
         SLL   R8,16
         AR    R7,R8
         LA    R8,0(0,0)      Subcode = 0 - Init
         LA    R15,CALLPC
         BALR  R11,R15
***
***
         L     R5,12(0,R10)   Get &Rd_in
         LTR   R5,R5
         BZ    NOSETRD
         LR    R1,R9
         LA    R1,30(0,R1)    MaxSock + 31 (=MS+1+30)
         SRL   R1,5           Get words in struct
         SLL   R1,2           Get bytes in struct
         LA    R8,1(0,0)      Subcode = 1 - Set Read
         LA    R15,CALLPC
         BALR  R11,R15
***
NOSETRD  DS    0H

         L     R5,16(0,R10)   Get &Wr_in
         LTR   R5,R5
         BZ    NOSETWR
         LR    R1,R9
         LA    R1,30(0,R1)    MaxSock + 31 (=MS+1+30)
         SRL   R1,5           Get words in struct
         SLL   R1,2           Get bytes in struct
         LA    R8,2(0,0)      Subcode = 2 - Set Write
         LA    R15,CALLPC
         BALR  R11,R15
***
NOSETWR  DS    0H

         L     R5,20(0,R10)   Get &Ex_in
         LTR   R5,R5
         BZ    NOSETEX
         LR    R1,R9
         LA    R1,30(0,R1)    MaxSock + 31 (=MS+1+30)
         SRL   R1,5           Get words in struct
         SLL   R1,2           Get bytes in struct
         LA    R8,3(0,0)      Subcode = 3 - Set Exception
         LA    R15,CALLPC
         BALR  R11,R15
***
NOSETEX  DS    0H
***
***
         L     R1,8(0,R10)    Get &timer
         LTR   R1,R1          NULL = Always wait forever
         BZ    SRERUN

         L     R8,0(0,R1)     Get seconds to wait
         SLL   R8,5           Multiply secs by 32
         LR    R15,R8
         SLL   R8,1           Multiply secs by 64
         AR    R8,R15         Multiply by ~100 (96) = #.01 secs

         SRL   R8,3           Make it ~#.08 sec waits

         L     R1,4(0,R1)     Get ms to wait
         LTR   R1,R1          Zero?
         BZ    SRERUN         Must be 'seconds'-wait or no-wait

         SRL   R1,6           Divide ms by ~10 (8) & make .08 secs
         LTR   R1,R1          Is it zero now?
         BNZ   SNOROUND       No need to round it up
         LA    R1,1(0,0)      Ensure we wait .08 secs at least

SNOROUND DS    0H

         AR    R8,R1          Now we have ~#.08 sec waits

SRERUN   DS    0H

         SR    R1,R1          Nothing to send

         SLL   R8,8           Move timer value <<8

         A     R8,PLUS4       Subcode = 4 - ***select***
         LA    R15,CALLPC
         BALR  R11,R15

         C     R4,MINUS2      If not -2, must be -1 or >0
         BNZ   SNOWAIT

         L     R1,8(0,R10)    Get &timer
         LTR   R1,R1          NULL = Always wait forever
         BZ    SDOWAIT

         SR    R4,R4          Assume 0 were triggered

         SRL   R8,8           Get timer value
         LTR   R8,R8
         BZ    SNOWAIT        No time left, return with 0
         A     R8,MINUS1

SDOWAIT  DS    0H

         LA    R15,TASKWAIT
         BALR  R11,R15

         B     SRERUN         Try again, loop up

SNOWAIT  DS    0H

         L     R1,40(0,R10)   Get &RetCd
         ST    R4,0(0,R1)

         C     R4,MINUS1
         BNZ   SNOERR         Not -1 must be >0

         SLR   R1,R1

         LR    R8,R7
         SRL   R8,16          Quick-get-sock

         LA    R7,2(0,0)      Function GetError(sock)
         LA    R15,CALLPC
         BALR  R11,R15

         L     R1,36(0,R10)   Get &ErrCd
         ST    R4,0(0,R1)

***
***
SNOERR   DS    0H

         SR    R1,R1          Nothing to send
         L     R6,24(0,R10)   Get &Rd_out
         LTR   R6,R6
         BZ    NOGETRD
         LA    R8,5(0,0)      Subcode = 5 - Get Read
         LA    R15,CALLPC
         BALR  R11,R15
***
NOGETRD  DS    0H

         SR    R1,R1          Nothing to send
         L     R6,28(0,R10)   Get &Wr_out
         LTR   R6,R6
         BZ    NOGETWR
         LA    R8,6(0,0)      Subcode = 6 - Get Write
         LA    R15,CALLPC
         BALR  R11,R15
***
NOGETWR  DS    0H

         SR    R1,R1          Nothing to send
         L     R6,32(0,R10)   Get &Ex_out
         LTR   R6,R6
         BZ    NOGETEX
         LA    R8,7(0,0)      Subcode = 7 - Get Exception
         LA    R15,CALLPC
         BALR  R11,R15
***
NOGETEX  DS    0H

         LA    R8,8(0,0)      Subcode = 8 - Clean-Up
         LA    R15,CALLPC
         BALR  R11,R15

****************************

EZARETRN DS 0H

         SLR   R15,R15
         L     R14,12(0,R13)
         LM    R0,R12,20(R13)
         BR    R14

* CALLPC: (** REPRESENTS REQUIRED VALUE ON ENTRY)
*  R0  = 0 (Initially, but turns to > 0 after the native call.
** R1  = Byte Counter to send
*  R2  = Source/Destination of PC buffer.  32bits.
*  R3  = Direction (0 = to Host PC, 1 = from Host PC)
*  R4  = Returned Bytes/Code/Socket etc.
** R5  = Source Buffer
** R6  = Destination Buffer (If required)
** R7  = Function Code (+ Socket for 'Connect', 'ioctl' & 'select')
** R8  = Aux. Data (Socket, or Port for 'Connect'...)
** R9  = Aux. Data (Port, or IP Address for 'Connect'...)
*  R10 = <Preserved>
** R11 = Return Address
*  R12 = <Preserved Base>
*  R13 = <Preserved Save>
*  R14 = Identifier (returned & passed back for conversations.)
*  R15 = Work Variable / Return Code

CALLPC   DS 0H

         LA    R3,0(0,0)         To Host PC
         SLR   R0,R0             Restart = No
         DC    X'75005000'       TCPIP 0,000(0,R5)
         LTR   R15,R15
         BNZ   ERROR

         LA    R3,1(0,0)         From Host PC
         SLR   R0,R0             Restart = No
         DC    X'75006000'       TCPIP 0,000(0,R6)

ERROR    ds 0h

         BR    R11

TASKWAIT DS 0H

*         STIMER WAIT,BINTVL=WAITTIME        MVS38j
         STIMER7 WAIT,BINTVL=WAITTIME        OS/390 for MVS38j

         BR    R11

TASKWAIS DS 0H

*         STIMER WAIT,BINTVL=WAITTIMS        MVS38j
         STIMER7 WAIT,BINTVL=WAITTIMS        OS/390 for MVS38j

         BR    R11

* *******************************************************************
DataBase Equ   *

         LTORG

INITAPI4 DC    C'INIT'       function 1, func 2&3: error(sock)/error
GETHOST4 DC    C'GETH'       function 4
SOCKET4  DC    C'SOCK'       function 5
BIND4    DC    C'BIND'       function 6
CONNECT4 DC    C'CONN'       function 7
LISTEN4  DC    C'LIST'       function 8
ACCEPT4  DC    C'ACCE'       function 9
SEND4    DC    C'SEND'       function 10
RECV4    DC    C'RECV'       function 11
CLOSE4   DC    C'CLOS'       function 12
EBCDIC24 DC    C'EBCD'       function 13
ASCII24  DC    C'ASCI'       function 14
IOCTL4   DC    C'IOCT'       function 15
GETSOCK4 DC    C'GETS'       function 16
SELECT4  DC    C'SELE'       function 17

MINUS1   DC    F'-1'
MINUS2   DC    F'-2'
PLUS1    DC    F'1'          code for FIONBIO
PLUS4    DC    F'4'          call *select* function

WAITTIME DC    F'8'          shorter timer interval = 0.08 seconds
WAITTIMS DC    F'100'        slower  timer interval = 1.00 second

HOSTENT  DC    F'0'          h_name
         DC    A(GNULL)      h_aliases
         DC    A(AF_INET)    h_addrtype
         DC    F'4'          h_length
         DC    A(GADD2)      h_addr_list
GNULL    DC    F'0'          NULL
GADD2    DC    A(GADDR)      Array [2] addresses
         DC    F'0'          NULL Terminator word
GADDR    DC    F'0'          Address

DataEnd  Equ   *
* *******************************************************************

AF_INET  Equ   2

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

         End   EZASOKET
