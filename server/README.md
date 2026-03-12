## Maciej Gębala
## 2025-04-13

### Pliki:

- Makefile
- board.h&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;*// zasady i obsługa gry*
- game_server.c&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;*// serwer gry* 

### Wymagania:

- Kompilator:  - gcc version 13.3.0
- Biblioteka:  - gsl 2.7 (game_random_bot - generator liczb losowych)
-   ```bash
    sudo apt install gcc libgsl-dev
    ```

### Wywołanie serwera:

```bash
 ./game_server <numer ip> <numer portu>
 ```
