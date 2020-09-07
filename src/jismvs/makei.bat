call runjcc "-class=String.h" String.c
call runjcc "-class=Variant.h" Variant.c
call runjcc "-class=Collection.h" Collection.c
call runjcc "-class=ObjectContext.h" ObjectContext.c
call runjcc "-class=Request.h" Request.c
call runjcc "-class=Session.h" Session.c
call runjcc "-class=Response.h" Response.c
call runjcc "-class=vberror.h" vberror.c
call runjcc "-class=Server.h" Server.c
call runjcc "-class=Application.h" Application.c
call runjcc init.c
call runjcc main.c

c:\jcc\prelink -s c:\jcc\objs\rent jis.obj main.obj init.obj Application.obj Collection.obj ObjectContext.obj Request.obj Response.obj Server.obj Session.obj String.obj Variant.obj vberror.obj URLDecode.obj URLEncode.obj

@del applicat.h /Q
@del collecti.h /Q
@del objectco.h /Q

@ren application.h applicat.h
@ren collection.h collecti.h
@ren objectcontext.h objectco.h

@del String.obj /Q
@del Variant.obj /Q
@del Collection.obj /Q
@del ObjectContext.obj /Q
@del Request.obj /Q
@del Session.obj /Q
@del Response.obj /Q
@del vberror.obj /Q
@del Server.obj /Q
@del Application.obj /Q
@del init.obj /Q
@del main.obj /Q

pause
