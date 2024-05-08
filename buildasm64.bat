@echo off
ml64 -nologo /c /Cx hello.asm
link -nologo /SUBSYSTEM:CONSOLE /NODEFAULTLIB /entry:_start hello.obj

REM /merge:.CRT=.rdata hello.obj kernel32.lib ucrt.lib legacy_stdio_definitions.lib legacy_stdio_wide_specifiers.lib