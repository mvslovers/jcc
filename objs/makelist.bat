@copy nul liblst.txt /y
@for %%i in (*.obj) do @echo %%i>> liblst.txt
