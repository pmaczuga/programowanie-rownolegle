# Laboratorium 1 - komunikacja PP (MPI)

## Opis zawartości
Główny program: `mpi.c`.  
Zapisuje do pliku podanego jako argument opóźnienie i przepustowść dla przesyłanie standardowego i synchronicznego.  

Pliki z hostami:  
`allnodes`, `single_node`, `two_nodes_same_host`, `two_nodes_other_hosts`.  
Zawierają listę node'ów, na których uruchamiane są procesy.

`script.sh` uruchamia skompilowany program na dla róznych kombinacji node'ów.

Pliki do testowania MPI: 
`hello_world.c`, `send_recv.c`, `ping_pong.c`

Dokładny opis zadania poniżej.

## Cel zajęć
Wprowadzenie do programowania w MPI oraz komunikacji między procesami.

**UWAGA: zadania na zajęciach oraz domowe można realizować w wybranym języku: C lub Python.  
Zalecane jest używanie C, ponieważ biblioteka dla Pythona nie implementuje całej specyfikacji MPI oraz ma braki w dokumentacji.  
Student odpowiada za to, żeby w pełni zrealizować zadanie i problemy wynikające z wyboru biblioteki nie będą okolicznością łagodzącą przy ocenianiu.**  

## Środowisko vCluster
 - `ssh vnode-01.dydaktyka.icsr.agh.edu.pl`
 - Z maszynek katedralnych można dostać się bezpośrednio
 - Z WiFi AGH lub z domu należy wejść przez bastion.ki.agh.edu.pl
 - Potrzebny VPN AGH (panel.agh.edu.pl)
 - Sensowny shell (bastion jest ograniczony): shell.lab.ki.agh.edu.pl
 - Credentiale takie z accounts.ki.agh.edu.pl (sso.ki.agh.edu.pl)
 - $HOME jest współdzielony z katedralnym NFSem
 - Większe operacje IO lepiej wykonywać w lokalnym /tmp
 - Klaster jest zwirtualizowany
 - 4 maszyny x 4 CPU (nody 01, 02, 03, 04)
 - 8 maszyn x 1 CPU (nody od 05 do 12)

## Tutorial
 - `git clone https://github.com/kfigiela/tpr_lab.git -b lab1`
 - istotny jest plik allnodes (tzw. machinefile) - używać ze zrozumieniem!
 - uruchamianie: mpiexec, warto zajrzeć do man mpiexec

### C
 - Przydatne linki
    - https://www.mcs.anl.gov/research/projects/mpi
    - https://computing.llnl.gov/tutorials/mpi/
 - Tutorial mpitutorial.com i przykłady github.com/wesleykendall/mpitutorial
 - `$ mpicc -o hello_world_c hello_world.c`  
   `$ mpiexec -machinefile ./allnodes -np [liczba procesów] ./hello_world_c`

### Python
 - Przydatne linki:
    - https://mpi4py.readthedocs.io/en/stable/
    - https://mpi4py.readthedocs.io/en/stable/tutorial.html
 - `$ chmod +x hello_world.py`  
   `$ mpiexec -machinefile ./allnodes -np [liczba procesów] ./hello_world.py`

## Ćwiczenia

### Ćwiczenie 0 - rozgrzewka
Przeanalizuj i uruchom przykłady z tutoriala dla C oraz Pythona. Pozostałe można potraktować jako ciekawostka.

### Ćwiczenia 1 - na punkty
 - Przetestuj różne sposoby komunikacji P2P dostępne w MPI.
    - Standard send `MPI_Send`
    - Synchronous send `MPI_Ssend`
    - Buffered send `MPI_Bsend`
    - Ready send `MPI_Rsend`
    - Non-blocking send `MPI_Isend` + `MPI_Wait`, `MPI_Test`
    - Odbieranie danych `MPI_Recv`, `MPI_Irecv`
 - **Jak działa komunikacja standardowa?**
 - **http://www.dartmouth.edu/~rc/classes/intro_mpi/mpi_comm_modes.html**

### Ćwiczenie 2
 - Zaimplementuj aplikację, gdzie 2 węzły wymieniają się komunikatami (pingpong).
 - Sprawdź zwykłą i buforowaną komunikację.
 - Zmierz opóźnienie (przyda się funkcja `MPI_Wtime`).
 - **W jaki sposób należy dokonać pomiaru?**
 - **Do czego służy `MPI_Barrier?`**

### Ćwiczenie 3 - "z gwiaizdką"
 - Zaimplementuj aplikację, w której każdy node przesyła do kolejnego otrzymaną daną - **token ring**.
 - Sprawdź zwykłą i synchroniczną komunikację.
 - Jak poprzednio dokonaj pomiarów.

## Zadanie domowe
 - Celem zadania jest zmierzenie opóźnienia i przepustowości połączeń w klastrze.

 - Należy przetestować dwa różne typy komunikacji P2P w MPI.
 - Należy dokonać pomiarów:
    - przepustowości [Mbit/s] od długości komunikatów [B]: wykres,
    - opóźnienia [ms] przesyłania krótkiego komunikatu: wartość.
 - Implementacja w C lub Python.
 - Sugestia: Do rysowania wykresów można użyć Gnuplot lub R/ggplot2.
 - **Zwrócić uwagę czy podczas testu maszynka jest obciążona - ktoś inny może puszczać w tym samym czasie testy, co zaburza wyniki**
 - Proszę przeprowadzić testy w następujących konfiguracjach:
    - Komunikacja na 1 nodzie (pamięć współdzielona)
    - Komunikacja na 1 nodzie (bez pamięci współdzielonej, przez sieć)
    - Komunikacja między 2 nodami na tym samym hoście fizycznym (przez sieć)
       - należy wziąć po jednym procesorze z dwóch maszyn, spośród vnode-01, vnode-02, vnode-03
    - Komunikacja między 2 nodami na różnych hostach fizycznych (przez sieć)
       - należy wziąć po jednym procesorze z dwóch maszyn, spośród vnode-05 - vnode-12
 - Wynikiem zadania ma być sprawozdanie, które należy wgrać przed zajęciami na platformę: https://upel.agh.edu.pl/wiet/mod/assign/view.php?id=18239
    - Wymagana zawartość sprawozdania opisana pod powyższym linkiem
