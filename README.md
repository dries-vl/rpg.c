# tcc compile command
tcc src/main.c -Iinclude -Llib -lminifb -lcurl -ltinycthread -luser32 -lgdi32 -lwinmm icon.res

# create cuda dll
nvcc --shared -o kernel.dll src/kernel.cu -lcudart

# create .ico with magick command (needed for windows icons)
magick convert logo.png logo.ico

# create .rc file (needed for .res)
#define MAINICON 101
MAINICON ICON "app_icon.ico"

# create .res with windres command (needed for .exe icon)
windres icon.rc -O coff -o icon.res
