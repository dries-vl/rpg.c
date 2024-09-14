nvcc -c src/kernel.cu -o kernel.o
tcc src/main.c kernel.o -Iinclude -I"%CUDA_HOME%/include" -Llib -lminifb -lcurl -ltinycthread -luser32 -lgdi32 -lwinmm icon.res && (main.exe) || (
    echo Compilation failed. Please check for errors.
    pause
)
