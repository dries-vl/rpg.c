tcc src/main.c -Iinclude -Llib -lminifb -luser32 -lgdi32 -lwinmm icon.res && (main.exe) || (
    echo Compilation failed. Please check for errors.
    pause
)
