all: debug

debug : debug.mak .SYMBOLIC
        @%null
        
retail : retail.mak .SYMBOLIC
        @%null
        
clean : clean.mak .SYMBOLIC
        @%null

debug.mak: .SYMBOLIC
        @echo ******************************
        @echo *** Building fastfile.lib ***
        @echo ******************************
        @wmake /h /s /f fastfile.wat debug
        @echo ************************************
        @echo *** Building ffcreate.exe ***
        @echo ************************************
        @wmake /h /s /f ffcreate.wat debug
        
retail.mak: .SYMBOLIC
        @echo ******************************
        @echo *** Building fastfile.lib ***
        @echo ******************************
        @wmake /h /s /f fastfile.wat retail
        @echo ************************************
        @echo *** Building ffcreate.exe ***
        @echo ************************************
        @wmake /h /s /f ffcreate.wat retail
        
clean.mak: .SYMBOLIC
        @echo ******************************
        @echo *** Building fastfile.lib ***
        @echo ******************************
        @wmake /h /s /f fastfile.wat clean
        @echo ************************************
        @echo *** Building ffcreate.exe ***
        @echo ************************************
        @wmake /h /s /f ffcreate.wat clean
