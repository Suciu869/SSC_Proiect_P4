# Roadmap proiect SSC - P4

**Vectorizarea si optimizarea algoritmilor folosind instructiuni SIMD**

| | |
|---|---|
| Student | Stefan Suciu, grupa 30236 |
| Disciplina | Structura Sistemelor de Calcul - componenta de proiect |
| Categoria | proiect pentru nota maxima 8, cu imbunatatire originala pentru nota 10 |
| Platforma | PC x86 - AMD Ryzen 7 8845HS (Zen 4), Windows 11, nativ (fara WSL) |
| Compilator | g++ 16.2 (MinGW-w64, distributia WinLibs), GNU make 4.4 |
| Limbaj | C++17, intrinsics SSE / AVX2 / AVX-512 (`<immintrin.h>`) |
| Unelte auxiliare | Git + GitHub, Python 3 + matplotlib (doar pentru grafice) |

### Sistemul de test

| Componenta | Detalii |
|---|---|
| Nuclee | 8 nuclee / 16 fire de executie |
| Extensii SIMD | SSE4.2, AVX, AVX2, FMA, AVX-512 F / VL / BW / DQ |
| Cache L1D | 32 KB / nucleu |
| Cache L2 | 1 MB / nucleu |
| Cache L3 | 16 MB partajat |

## 0. Regula de lucru

Lucrul la proiect se face **exclusiv in timpul laboratoarelor**. De aceea:

- fiecare laborator are o lista proprie de sarcini, gandita sa incapa in durata lui; nu exista sarcini "pentru acasa";
- la finalul fiecarui laborator se face un commit cu mesajul `Lab N: <ce s-a realizat>`, ca progresul sa fie vizibil in istoricul repository-ului;
- ce nu se termina intr-un laborator se reia la inceputul celui urmator, inaintea sarcinilor noi.

## 1. Obiectivul proiectului

Implementarea a trei algoritmi cu operatii repetitive asupra vectorilor si matricelor, fiecare in doua variante:

- o **varianta scalara** (o operatie la un moment dat), folosita ca referinta;
- o **varianta vectorizata explicit prin intrinsics** (mai multe operatii in paralel, cu o singura instructiune).

Pentru fiecare algoritm se masoara timpul de executie si speedup-ul fata de varianta scalara. Se analizeaza efectul alinierii datelor, al dimensiunii vectorilor (128 / 256 / 512 biti) si al modului de acces la memorie.

## 2. Algoritmii alesi

| Nr. | Algoritm | Tip date | Ce se urmareste |
|---|---|---|---|
| A1 | Produs scalar a doi vectori | float | reducerea (suma orizontala), FMA |
| A2 | Inmultirea a doua matrice NxN | float | accesul la memorie (randuri vs coloane), cache |
| A3 | Convolutie 2D 3x3 pe imagine grayscale | uint8 | procesare de pixeli, tratarea marginilor (clamp) |

## 3. Imbunatatirea propusa pentru nota 10

Optimizarea folosirii procesorului si a memoriei cache, peste vectorizarea de baza:

1. **Acumulatori multipli si desfasurarea buclelor (loop unrolling)** la produsul scalar si la inmultirea matricelor. In loc de un singur registru in care se aduna rezultatul, se folosesc 4 registre independente. Procesorul poate executa astfel mai multe instructiuni FMA in paralel, pentru ca nu mai asteapta rezultatul celei anterioare.
2. **Cache blocking (tiling)** la inmultirea matricelor. Matricele sunt impartite in blocuri mici care incap in cache-ul L1/L2, astfel incat datele sunt refolosite cat timp sunt inca in cache, in loc sa fie aduse de mai multe ori din RAM.
3. **Extinderea la AVX-512** si comparatia SSE (128 biti) vs AVX2 (256 biti) vs AVX-512 (512 biti) pe acelasi procesor, cu interpretarea rezultatelor (pe Zen 4, AVX-512 este executat intern ca doua operatii de 256 de biti).

Beneficiul demonstrabil: speedup suplimentar fata de varianta vectorizata simpla, masurat si prezentat in grafice. Toate tehnicile sunt implementate doar cu cod C++ simplu, fara biblioteci externe.

Imbunatatirea trebuie discutata si validata cu cadrul didactic (laboratorul 3).

## 4. Planificarea pe laboratoare

Laboratorul de proiect are loc o data la doua saptamani. Proiectul trebuie finalizat pana la laboratorul 6.

### Laboratorul 3 - Organizare si fundatie

**De predat la laborator**

- [ ] Repository privat pe GitHub, cu acces pentru cadrul didactic
- [ ] `ROADMAP.md` (acest document)
- [ ] `CADRU_CONCEPTUAL.md` (documentul de context)
- [ ] Validarea cu profesorul a algoritmilor alesi si a imbunatatirii pentru nota 10

**De lucrat in laborator**

*Mediul de lucru si procesorul*

- [ ] Verificarea mediului: `g++ --version`, `make --version`, `git --version`, `python -c "import matplotlib"`
- [ ] Program `check_cpu` care detecteaza extensiile SIMD prin CPUID (`__builtin_cpu_supports`): SSE4.2, AVX2, FMA, AVX-512 F/VL/BW/DQ; verificarea ca sistemul de operare salveaza registrele extinse (XGETBV), altfel instructiunile AVX-512 ar genera exceptie
- [ ] Smoke test: cate un fisier minimal pentru fiecare backend (o instructiune `_mm_`, `_mm256_`, `_mm512_`), compilat cu flag-urile lui, ca sa confirmam ca toate cele trei latimi de vector ruleaza

*Structura si compilarea*

- [ ] Structura proiectului: `src/common`, `src/scalar`, `src/sse`, `src/avx2`, `src/avx512`, `bench`, `scripts`, `results`, `docs`
- [ ] `Makefile` cu flag-uri separate per backend, fiecare director compilat doar cu flag-urile lui:

  | Backend | Flag-uri |
  |---|---|
  | scalar | `-O2 -fno-tree-vectorize` |
  | sse | `-O2 -msse4.2` |
  | avx2 | `-O2 -mavx2 -mfma` |
  | avx512 | `-O2 -mavx512f -mavx512vl -mavx512bw -mavx512dq` |

- [ ] Targete `make`: `all`, `run`, `plot`, `clean`, `check-cpu`, `asm` (generarea assembly-ului cu `-S`)
- [ ] Tabel de pointeri la functii *algoritm x backend*; variantele neimplementate (stub-uri, `nullptr`) sunt sarite automat. Un backend nou (de ex. ARM NEON) se adauga cu un director si o linie in tabel

*Modulul comun (`src/common`)*

- [ ] Alocare aliniata configurabila (implicit 64 B = o linie de cache = un registru AVX-512), cu `new (std::align_val_t(N))`; `std::aligned_alloc` nu exista pe Windows. Optiune de buffer deplasat intentionat cu cativa octeti, pentru testul de aliniere
- [ ] Generator de date pseudo-aleatoare cu seed fix (xorshift32), ca datele sa fie identice la fiecare rulare si pe orice sistem
- [ ] Timer de inalta rezolutie, monoton: `std::chrono::steady_clock`
- [ ] Functii de validare: toleranta relativa pentru float, egalitate exacta pentru uint8
- [ ] Fixarea procesului pe un singur nucleu (`SetThreadAffinityMask` pe Windows, `sched_setaffinity` pe Linux)

**Rezultat la finalul laboratorului:** proiectul se compileaza cu `make`, `make check-cpu` afiseaza extensiile, iar stub-urile sunt sarite fara erori. Commit `Lab 3: ...`.

### Laboratorul 4 - Versiunile scalare, harness-ul si primele variante vectorizate

**De lucrat in laborator**

- [ ] Versiunile scalare ale celor 3 algoritmi: produs scalar; matrice naiv (ijk) si cu B transpus; convolutie 3x3 cu margini prin clamp
- [ ] Verificarea cu `make asm` ca assembly-ul scalar nu contine instructiuni vectoriale (`addps`, `mulps`, `vfmadd...ps`, registre `ymm`/`zmm`)
- [ ] Harness-ul de masurare: warm-up, 11 rulari, mediana / minim / maxim, validare fata de scalar, export in `results/results.csv`, argumente in linia de comanda (algoritm, backend, dimensiuni, numar de rulari)
- [ ] Dimensiuni alese dupa cache, ca sa se vada trecerea L1 -> L2 -> L3 -> RAM: produs scalar 1K - 64M elemente, matrice 64 - 1024, imagini 256x256 - 4096x4096
- [ ] Produsul scalar vectorizat (SSE + AVX2), validat
- [ ] Convolutia 2D vectorizata (AVX2), validata

**De prezentat la final:** varianta scalara + harness functional, cu primele masuratori; A1 (SSE, AVX2) si A3 (AVX2) validate. Commit `Lab 4: ...`.

### Laboratorul 5 - Analiza si backend-ul AVX-512

**De lucrat in laborator**

- [ ] Inmultirea matricelor: naiv, cu B transpus, vectorizat AVX2
- [ ] Backend-ul AVX-512 pentru cei 3 algoritmi
- [ ] Analiza alinierii: buffer aliniat (`load`) vs nealiniat (`loadu`)
- [ ] Analiza dimensiunii vectorului: SSE (4 float) vs AVX2 (8 float) vs AVX-512 (16 float)
- [ ] Analiza accesului la memorie la inmultirea matricelor
- [ ] Masuratori pe dimensiuni de date de la L1 pana la RAM
- [ ] Acumulatori multipli + loop unrolling (A1, A2)

**De prezentat la final:** toti cei 3 algoritmi vectorizati si validati; primele rezultate cu acumulatori multipli. Commit `Lab 5: ...`.

### Laboratorul 6 - Optimizarea finala si predarea

**De lucrat in laborator**

- [ ] Cache blocking (tiling) la inmultirea matricelor, cu test pe mai multe dimensiuni de bloc
- [ ] Rularea finala a masuratorilor si graficele: timp si speedup in functie de dimensiune, pentru fiecare algoritm si backend
- [ ] Raportul tehnic `docs/RAPORT.md`: arhitectura, implementare, teste, rezultate, interpretare

**Predarea**

- [ ] Demonstratie practica: compilare, rulare, validare, afisarea rezultatelor
- [ ] Prezentarea graficelor si a interpretarii lor
- [ ] Raportul tehnic final in repository
- [ ] Ultimul commit cu versiunea finala

## 5. Livrabile finale

| Livrabil | Locatie in repository |
|---|---|
| Cod sursa (scalar, SSE, AVX2, AVX-512, optimizari) | `src/` |
| Harness de benchmark | `bench/` |
| Script pentru grafice | `scripts/plot.py` |
| Rezultate masuratori | `results/` |
| Roadmap | `ROADMAP.md` |
| Cadru conceptual | `CADRU_CONCEPTUAL.md` |
| Raport tehnic | `docs/RAPORT.md` |

## 6. Criterii de evaluare urmarite

- Corectitudinea rezultatelor (comparatie cu varianta scalara)
- Timpii de executie si speedup-ul pentru fiecare algoritm
- Interpretarea rezultatelor (de ce speedup-ul variaza cu dimensiunea datelor)
- Beneficiul masurat al imbunatatirii pentru nota 10

## 7. Riscuri si solutii

| Risc | Solutie |
|---|---|
| Timpul de lucru e limitat la orele de laborator | Sarcini planificate pe fiecare laborator; commit la final; prioritate pentru cerintele de baza (nota 8) inaintea imbunatatirilor |
| Compilatorul vectorizeaza automat varianta scalara si speedup-ul pare fals | Varianta scalara este compilata cu `-fno-tree-vectorize`; se verifica assembly-ul generat (`make asm`) |
| Masuratori instabile sub Windows (procese in fundal) | Rulari repetate, raportarea medianei, fixarea procesului pe un nucleu, prioritate ridicata a firului de executie |
| Diferente mici intre rezultatele float scalare si vectorizate | Validare cu toleranta relativa; ordinea adunarilor difera, iar FMA rotunjeste o singura data |
| Dimensiunea datelor nu se imparte exact la latimea vectorului | Tratarea separata a elementelor ramase (bucla de final; la AVX-512, masti) |
| `std::aligned_alloc` lipseste pe Windows | Alocare aliniata cu `new (std::align_val_t(64))` (C++17), portabila si pe Linux / ARM |
