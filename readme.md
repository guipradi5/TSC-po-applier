# Traductor de archivos .dat de MonoBehaviours con po

## Requisitos
### Carpeta /dat
Aqui pondremos los assets extraidos directamente de cualquier herramienta de extraccion de assets de Unity

### Carpeta /po
Aqui pondremos los archivos po.
Cada archivo .po tiene que tener el mismo nombre que el .dat al que se le aplicará
La estructura del archivo .po es la siguiente:

```
msgid "Original String"
msgstr "Translated string"

msgid "Book"
msgstr "Libro"

msgid "Gun"
msgstr "Pistola"

...
```
El msgid indica el string que reemplazarña
El msgstr indica el string que quedará finalmente


### Carpeta /output
Aqui saldrán los archivos .dat nuevos una vez se hayan aplicado las traducciones.

## Como ejecutar
1. Introduce los archivos .dat originales del juego en la carpeta `/dat` 
2. Introduce los archivos .po correspondientes dentro de la carpeta `/po`, 
3. Ejecuta translatePos.exe
4. Los nuevos archivos .dat traducidos se encontrarán en la carpeta `/output`



# Compile de application

## Download MinGW

Download and use the recommended installer:  
**MSYS2** (easier and up-to-date).  
[https://www.msys2.org/](https://www.msys2.org/)

## Install MSYS2 (recommended option)

1. Download the installer and run it.
2. Open **MSYS2 MSYS** from the Start menu.

## Install the g++ compiler

In the MSYS2 console, type:

```sh
pacman -Syu
pacman -S mingw-w64-x86_64-gcc
```

This will install g++ in your environment.

## Add MinGW to the PATH

If you used MSYS2, add the following to your Windows PATH environment variable:

```
C:\msys64\mingw64\bin
```

To add it:

- Right-click on **This PC** → **Properties**  
- Go to **Advanced system settings** → **Environment Variables**  
- Find **Path**, edit it, and add the above path.

## Verify the installation

Open CMD or PowerShell and type:

```sh
g++ --version
```

You should see something like:

```
g++ (Rev2, Built by MSYS2 project) 13.2.0
```

---

## ✅ Compile the program

1. Copy the file `translator.cpp` into a folder.
2. Open CMD or PowerShell in that folder.
3. Run:

```sh
g++ poApplier.cpp -o translatePos.exe -std=c++17
```

4. Run the program:

```sh
translatePos.exe
```
