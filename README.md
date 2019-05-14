# TIN_generator

<h2>Polecenie</h2>
Zaimplementować generator ruchu wykorzystujący standardowy stos TCP/IP w systemie Linux. Ruch powinien być generowany z wykorzystaniem protokołu UDP. Należy zaimplementować aplikację klienta i serwera. Należy przyjąć że serwer obsługuje w danym momencie tylko jednego klienta. Generator powinien umożliwiać jak najprecyzyjniejsze generowanie ruchu (pakietów UDP o określonej długości) z zadaną szybkością w obu kierunkach (szybkość dla obu kierunków powinna być ustalana niezależnie). Zarówno klient jak serwer powinien być zaimplementowany jako aplikacja wielowątkowa z dedykowanymi wątkami do generacji i odbierania pakietów (wątki te powinny być niezależnie od wątku głównego). Generator powinien mierzyć uzyskiwaną przepływność w obu kierunkach transmisji. Zarówno klient jak i serwer powinien umożliwiać wyświetlenie chwilowej przepływności uzyskiwanej w obu kierunkach transmisji np. wpisanie komendy print na konsoli klient powinno skutkować pobraniem statystyki z serwera i jej wyświetlenie na ekranie. Porównać dokładność działania zaimplementowanego generatora z aplikacją TRex (https://trex-tgn.cisco.com/trex/doc/index.html)

<h2>Uruchomienie</h2>
Aby odpalic program nalezy miec zainstalowane:
-boost
-cmake
-make
Aby zbudowac nasz program nalezy wpisac nastepujaca komende w glownym katalogu:

    cmake .

W katalogu src_server aby zbudowac i uruchomic, trzeba wpisac:

    make

A nastepnie:

    ./TIN_Server

W katalogu src_server aby zbudowac i uruchomic, trzeba wpisac:

    make

A nastepnie:

    ./TIN_Generator

