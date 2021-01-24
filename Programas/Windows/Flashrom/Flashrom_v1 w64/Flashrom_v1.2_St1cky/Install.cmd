@echo off
>nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"
if '%errorlevel%' NEQ '0' (
echo Requesting administrative privileges... 
goto request
) else (goto start)

:request
echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
set params = %*:"=""
echo UAC.ShellExecute "cmd.exe", "/c %~s0 %params%", "", "runas", 1 >> "%temp%\getadmin.vbs"
"%temp%\getadmin.vbs"
del "%temp%\getadmin.vbs"
exit /b

:start
pushd %~dp0driver
certutil -delstore root wch > nul
certutil -delstore trustedpublisher wch > nul
echo MIIC4TCCAc2gAwIBAgIQ6esqeE1ssKtE9/pxf6xkqDAJBgUrDgMCHQUAMA4xDDAKBgNVBAMTA1dDSDAeFw0xNzExMDUxNTQxNTVaFw0zOTEyMzEyMzU5NTlaMA4xDDAKBgNVBAMTA1dDSDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANpz8hJ1UWUI8qOF9DdwGB3oLqmuW1YzRmEwwUEaXYo3bOF6f5N3dk8WOAXZLrRA5MMVQXCMpx4I8Xld+WUHTmOsJc43mBmiMfokxzrObS6jVNleUJg/vvgzS2PD3RpotPgUDN8jV0sxtHdDQpqXOy9/N4IgIm7JUA67D/WSPwykg1sPsEyz2nspsahWDyNEBol+jSdGYge0CwdXI7Pr7gCdisNmOxtJhRcvvFfgJA6RDt2Q/9ydCaNvC9BlydHXUePh1yfW9FsiTemnUYar1VBjMYzV19dfDIXSZQx1vZ74PLBlW0YqwJT4EKKnMdHBTJjOvkmP9/RE+vgpfraD1ikCAwEAAaNDMEEwPwYDVR0BBDgwNoAQf+OaDkIgi00oCp0rEUKon6EQMA4xDDAKBgNVBAMTA1dDSIIQ6esqeE1ssKtE9/pxf6xkqDAJBgUrDgMCHQUAA4IBAQAlJS7D0VXU2MtNd1fdXjvoMTLKjL4nwfZmQUU6ujX2XLze2pzwMIpWHlC5HUCg31EEO8WGqOko/KeMwV1IFtNtZqrAKGkjc1SqwvggQOmmaM/RAxQ05F4AiCkFexU9PGTS5erAQ8BVbcDV8/QWD9GGx139KckPxXGhbTphLQT+uR8tss0LpCDBxQuhGSRax/cdNuEcAiUiZaDDV8zBELxVxIfzUbGm4bakQnxR3u+9NfT0+o7z3kzyTGRXCl3Q8HCs1tlXruH9nEOqt8X9my8sN1VaeOHUqeI5GM5CqiiLYOJ6tRmv08Q7Snz6uTLXgbyL1DN/XnU6o+s5C4Q2nIG3 > cert.cer
certutil -addstore root cert.cer > nul
certutil -addstore trustedpublisher cert.cer > nul
del cert.cer
pnputil -i -a CH341A.inf > nul
cd ..
cmd /k echo Enter flashrom to probe for flash chips.