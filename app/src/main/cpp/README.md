# Нативная часть

Все изменения этого комплекта находятся внутри `app/src/main/cpp`.

## Исправление `FileNotFoundException: deps/raymob/display.c`

Существующий `app/build.gradle` открывает файл
`src/main/cpp/deps/raymob/display.c` на этапе `:app:preBuild`, ещё до запуска
CMake. Поэтому исключение нельзя устранить одним изменением `CMakeLists.txt`.

В `deps/raymob/display.c` добавлен небольшой compatibility translation unit.
Он обеспечивает наличие ожидаемого Gradle пути. Текущая игра использует
стандартный API raylib напрямую, поэтому этот файл намеренно не компилируется
и не подменяет полноценную реализацию Android backend.

CMake собирает raylib и игровые исходники из `game/`, а не старый `raymoblib`.
`main.cpp` и игровой UI подключают `raylib.h`.

## Применение

Распаковать архив с корнем репозитория, разрешив замену файлов. Архив содержит
только каталог `app/src/main/cpp`. Никаких команд, изменений Gradle,
AndroidManifest, workflow или файлов вне `cpp` не требуется. Существующий
`deps/raylib` не удалять и не заменять.

## Проверки

- `tests/world_test.cpp` — генерация мира, перемещение, бой, предметы.
- `tests/network_client_test.cpp` — протокол и сетевой клиент.
- `tests/compile_ui_stub.sh` — компиляция UI и точки входа с raylib API stub.

Эти проверки не заменяют сборку APK в GitHub Actions.
