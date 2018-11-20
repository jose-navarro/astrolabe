REM Parameter 1: The name of current project platform, for example, "Win32" or "x64"
REM Parameter 2: The name of the current project configuration, for example, "Debug".
REM Parameter 3: The base name of the primary output file for the build.

ECHO COPYING HEADERS TO headers folder...

REM Create the header folder if it does not exist.

IF NOT EXIST ".\headers" (
  ECHO Creating headers folder...
  MKDIR ".\headers"
)

IF NOT EXIST ".\headers\xtl" (
  ECHO Creating headers folder...
  MKDIR ".\headers\xtl"
)

REM Create the binary folder if it does not exist.

IF NOT EXIST ".\binaries" (
  ECHO Creating binaries folder...
  MKDIR ".\binaries"
)

REM Copy all *.hpp & *.h files.

COPY .\src\*.hpp   .\headers\*.hpp
COPY .\src\*.h     .\headers\*.h
COPY .\src\xtl\*.h .\headers\xtl\*.h

REM Copy library

COPY .\%1\%2\%3.lib .\binaries\*.*
