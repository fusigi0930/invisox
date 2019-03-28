md %1\..\auto\inc
md %1\..\auto\lib
copy %2\*.a %1\..\auto\lib
copy %1\*.h %1\..\auto\inc

exit 0