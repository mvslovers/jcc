//MYJOB    JOB
//MYSTEP   EXEC PGM=PRELINK,
//         PARM='-r //DDN:L //DDN:O //DDN:I'
//STEPLIB  DD   DSN=SYS1.LINKLIB,DISP=SHR
//STDOUT   DD   SYSOUT=*
//L        DD   DSN=PREL,DISP=SHR
//I        DD   DSN=PREI,DISP=SHR
//O        DD   DSN=PREO,DISP=(OLD,KEEP)
