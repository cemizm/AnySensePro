xcopy /Y "..\xeniC.AnySense.DriverInstaller_x86\bin\Release\xeniC.AnySense.DriverInstaller_x86.msi" "..\Bootstrapper Package\AnySenseProDriver\"
xcopy /Y "..\xeniC.AnySense.DriverInstaller_x64\bin\Release\xeniC.AnySense.DriverInstaller_x64.msi" "..\Bootstrapper Package\AnySenseProDriver\"
"C:\Program Files\Windows Kits\10\bin\x86\signtool" sign /v /ac "GlobalSign Root CA.crt" /n "xeniC UG (haftungsbeschraenkt)" /tr http://timestamp.globalsign.com/scripts/timestamp.dll  /td sha256 "..\Bootstrapper Package\AnySenseProDriver\*.msi"
pause