//MYJOB    JOB
//MYSTEP   EXEC PGM=JCC,
//         PARM='-I//DDN:JCCINCL -fcode=MYCSECT //DDN:JCCINPUT'
//STEPLIB  DD   DSN=SYS1.LINKLIB,DISP=SHR
//STDOUT   DD   SYSOUT=*
//*
//* Library include pds (<...> files, with .h removed)
//*(Specified via -I in parms above)
//JCCINCL  DD   DSN=JCCINC,DISP=SHR
//*
//* User include pds (quoted files, with .h removed)
//* (DD:hardcoded in JCC)
//JCCINCS  DD   DSN=JCCHINC,DISP=SHR
//*
//* File to compile - (see parm above)
//JCCINPUT DD   DSN=JCCIN,DISP=SHR
//*
//* Temp (precompiled) output (DD:hardcoded in JCC)
//JCCOUTPT DD   DSN=JCCOUT,DISP=(OLD,KEEP)
//*
//* Generated assembler (DD:hardcoded in JCC)
//JCCOASM  DD   DSN=JCCASM,DISP=(OLD,KEEP)
