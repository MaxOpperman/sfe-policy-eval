@echo off

SET stats_command=docker stats %*
SET output_file=./measurements/output.txt

echo executing %stats_command%
type NUL > %output_file%

%stats_command% >> %output_file%
:loop
timeout 1
%stats_command% | findstr /v "CONTAINER" >> %output_file%
goto loop