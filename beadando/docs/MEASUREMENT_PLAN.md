# Mérési Terv: Mandelbrot Halmaz Párhuzamosítási Összehasonlítás

## 1. Mérési környezet
* **Processzor (CPU):** Intel(R) Core(TM) i5-11320H @ 3.20GHz
* **Fizikai magok / Logikai szálak:** 4 mag / 8 szál
* **Videókártya (GPU):** NVIDIA GeForce RTX 3050 Ti Laptop GPU
* **Memória (RAM):** 16,0 GB
* **Operációs rendszer:** Windows 11 64-bit

## 2. Mérési paraméterek
* **Maximális iterációszám:** 1000 (fix)
* **Átlagolás:** 5 futtatás átlaga (a program automatikusan számolja)
* **Adatpontosság:** `double` (64-bit)

---

## 3. Tesztelési sorozatok

### A. Sorozat: OpenMP (CPU) skálázhatóság
*Cél: Megvizsgálni a párhuzamosítás hatékonyságát a szálak növelésével.*
*Fix felbontás: 2048 x 2048*

| Szálak száma | Futtatandó parancs | Megjegyzés |
| :--- | :--- | :--- |
| 1 | `mandelbrot.exe 2048 2048 1` | Szekvenciális alapvonal |
| 2 | `mandelbrot.exe 2048 2048 2` | 2 szál |
| 4 | `mandelbrot.exe 2048 2048 4` | Összes fizikai mag |
| 6 | `mandelbrot.exe 2048 2048 6` | Részleges Hyper-Threading |
| 8 | `mandelbrot.exe 2048 2048 8` | Összes logikai szál |

### B. Sorozat: Technológiai összehasonlítás (CPU vs GPU)
*Cél: A CPU maximális erejének összevetése a GPU-val.*
*CPU beállítás: 8 szál (OpenMP)*

| Felbontás | Platform | Parancs | Várt szálak (Threads) |
| :--- | :--- | :--- | :--- |
| **512 x 512** | OpenMP | `mandelbrot.exe 512 512 8` | 8 |
| **512 x 512** | OpenCL | `mandelbrot_ocl.exe 512 512` | 262 144 |
| **1024 x 1024** | OpenMP | `mandelbrot.exe 1024 1024 8` | 8 |
| **1024 x 1024** | OpenCL | `mandelbrot_ocl.exe 1024 1024` | 1 048 576 |
| **2048 x 2048** | OpenMP | `mandelbrot.exe 2048 2048 8` | 8 |
| **2048 x 2048** | OpenCL | `mandelbrot_ocl.exe 2048 2048` | 4 194 304 |
| **4096 x 4096** | OpenMP | `mandelbrot.exe 4096 4096 8` | 8 |
| **4096 x 4096** | OpenCL | `mandelbrot_ocl.exe 4096 4096` | 16 777 216 |
| **8192 x 8192** | OpenMP | `mandelbrot.exe 8192 8192 8` | 8 |
| **8192 x 8192** | OpenCL | `mandelbrot_ocl.exe 8192 8192` | 67 108 864 |

---

## 4. Diagramok és elemzés (Dokumentációhoz)

### 1. OpenMP Skálázhatósági Grafikon (A. sorozat)
* **X-tengely:** Szálak száma (1, 2, 4, 6, 8)
* **Y-tengely:** Futási idő (ms)
* *Elemzés:* Mutassuk be, hogy 1-ről 4-re váltásnál majdnem negyedelődik az idő, de 4-ről 8-ra (Hyper-Threading) már kisebb a gyorsulás.

### 2. CPU vs GPU Összehasonlító Grafikon (B. sorozat)
* **X-tengely:** Felbontás (512-től 8192-ig)
* **Y-tengely:** Futási idő (ms)
* *Elemzés:* Itt fog látszani, hogy míg kis felbontáson a különbség elhanyagolható (sőt, a GPU az overhead miatt lassabb), 8192-nél a GPU sokkal gyorsabb lesz.

### 3. Gyorsítási tényező (Speedup)
Számoljuk ki minden felbontásnál: $Speedup = T_{OpenMP\_8} / T_{OpenCL}$
* *Elemzés:* Írjuk le, hogy a GPU hányszor gyorsabb a CPU-nál a legnagyobb terhelés mellett.

---

## 5. Elemzési szempontok a dokumentációhoz

1.  **RTX 3050 Ti előnye:** Mivel dedikált videókártyáról van szó, a saját VRAM és a több ezer CUDA mag miatt a CPU-nál jelentősen nagyobb gyorsulás várható nagy felbontáson.
2.  **Inicializációs Overhead:** Az OpenCL első futása (Run 1) tartalmazza a kernel fordítási idejét, amit az elemzéskor külön érdemes kezelni.
3.  **Memória transzfer:** A GPU-nak a számítás után a dedikált memóriából vissza kell másolnia az adatokat a rendszermemóriába, ez nagy felbontásnál (pl. 8192x8192) mérhető időt vehet igénybe.
4.  **Váltópont:** Annak a felbontásnak a meghatározása, ahol az NVIDIA GPU már hatékonyabb, mint az Intel CPU.