@echo off
echo "====start build xlcomm===="
@echo on

rm -r install

mkdir build
cd build
cmake ..
cmake --build . --config release

cd ..
xcopy /y third_party\lib\*.dll install\
xcopy /y build\lib\release\*.dll install\
xcopy /y build\bin\release\*.exe install\
