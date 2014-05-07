@echo off
SETLOCAL
set "return=0"
pushd ..
set "dest=%CD%/BasicDataStructures"

call:transferFiles "%CD%/binaryTree_program"  %dest% binarytree
call:transferFiles "%CD%/binaryTree_program"  %dest% bnode
call:transferFiles "%CD%/Stack_program" %dest% stack
call:transferFiles "%CD%/Queue_program" %dest% queue
call:transferFiles "%CD%/hashTable_Program"  %dest% hashtable
call:transferFiles "%CD%/linkedList_Program" %dest% list
call:transferFiles "%CD%/linkedList_Program" %dest% linked_list
call:transferFiles "%CD%/bigint_Program" %dest% bigint

popd
ENDLOCAL
goto:EOF



::tranferFiles(srcFolderPath, targetName)
:transferFiles
SETLOCAL
	echo gathering %3.h %3.c
	robocopy "%1" "%2/include" "%3.h" >nul
	robocopy "%1" "%2/src" "%3.c" > nul

ENDLOCAL
goto:EOF

