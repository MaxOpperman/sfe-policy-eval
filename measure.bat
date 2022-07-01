@echo off

SET stats_command=docker stats %*
SET output_file=./output.txt

echo "executing %stats_command% | findstr /v CONTAINER >> %output_file%"
type NUL > %output_file%

:loop
%stats_command% | findstr /v "CONTAINER" >> %output_file%
timeout 1
goto loop