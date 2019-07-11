@echo off
setlocal enableextensions
IF NOT EXIST %APPDATA%\QtProject\qtcreator\templates\wizards\classes (
   echo "Creating wizards folder:  %APPDATA%\QtProject\qtcreator\templates\wizards\classes"
   md %APPDATA%\QtProject\qtcreator\templates\wizards\classes
)
IF NOT EXIST %APPDATA%\QtProject\qtcreator\templates\wizards\projects (
   echo "Creating wizards folder:  %APPDATA%\QtProject\qtcreator\templates\wizards\projects"
   md %APPDATA%\QtProject\qtcreator\templates\wizards\projects
)
echo "Copying XPCF classes wizards to: %APPDATA%\QtProject\qtcreator\templates\wizards\classes\xpcf"
xcopy classes\xpcf %APPDATA%\QtProject\qtcreator\templates\wizards\classes\xpcf /s /e /i

echo "Copying XPCF projects wizards to: %APPDATA%\QtProject\qtcreator\templates\wizards\projects"
xcopy projects %APPDATA%\QtProject\qtcreator\templates\wizards\projects /s /e /i

IF NOT EXIST %USERPROFILE%\.remaken\rules (
   echo "Creating default remaken rules folder: %USERPROFILE%\.remaken\rules"
   md %USERPROFILE%\.remaken\rules
)
IF NOT EXIST %USERPROFILE%\.remaken\rules\qmake (
   echo "Copying default rules from ..\..\builddefs\qmake to %USERPROFILE%\.remaken\rules\qmake"
   xcopy ..\..\builddefs\qmake %USERPROFILE%\.remaken\rules\qmake /s /e /i
)
endlocal
