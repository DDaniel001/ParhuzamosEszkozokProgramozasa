# ParhuzamosEszkozokProgramozasa# Féléves Beadandó: Mandelbrot halmaz párhuzamosítása (OpenMP vs OpenCL)

## Projekt leírása
A projekt célja a Mandelbrot halmaz generálásának párhuzamosítása és a számítási teljesítmény mérése. A feladat során összehasonlítjuk a hagyományos, processzor alapú (CPU) párhuzamosítást **OpenMP** segítségével a modern, videókártya alapú (GPU) gyorsítással, amelyet **OpenCL** segítségével valósítunk meg.

A programok kimenetként legenerálják a Mandelbrot halmaz képét (`.ppm` formátumban), valamint a futási időket és paramétereket `.csv` fájlokba naplózzák a későbbi teljesítményelemzéshez és dokumentációhoz.

## Mappaszerkezet

* `openmp/`: A CPU-s párhuzamosítás forráskódjai és fordító szkriptje.
* `opencl/`: A GPU-s párhuzamosítás (OpenCL kernel és C wrapper) forráskódjai.
* `docs/`: Ide kerülnek az összesített mérések (Excel) és a beadandó hivatalos dokumentációja.

## Rendszerkövetelmények
* C fordító (pl. GCC, MinGW).
* OpenMP támogatás a fordítóban.
* OpenCL SDK és kompatibilis hardver (GPU/CPU).
* GNU Make.

---

## 1. OpenMP (CPU) verzió futtatása

Ez a verzió a feladatot a CPU magjain osztja el.

**Fordítás:**
Lépjen be az `openmp` mappába, majd használja a make parancsot:
```bash
cd openmp
make
```

**Futtatás:**
A program 3 opcionális paramétert vár: szélesség, magasság és szálak száma.
```bash
./mandelbrot.exe 1024 1024 4
```

## 2. OpenCL (GPU) verzió futtatása

Ez a verzió a videókártya sok ezer számítási magját használja fel a pixelek párhuzamos kiszámítására.

**Fordítás:**
Lépjen be az opencl mappába, majd használja a make parancsot:
```bash
cd opencl
make
```

**Futtatás:**
A program 2 opcionális paramétert vár: szélesség és magasság (a szálak számát a GPU hardveresen optimalizálja).
```bash
./mandelbrot_ocl.exe 1024 1024
```

## Mérések és Eredmények
Mindkét program futása után az alábbi kimenetek keletkeznek a saját mappájukban:

* mandelbrot.ppm / mandelbrot_ocl.ppm: A vizuális eredmény.

* results/*.csv: A mérési adatok (kép felbontása, szálak száma/platform, futási idő milliszekundumban), amihez a program minden futtatáskor új sort fűz hozzá.