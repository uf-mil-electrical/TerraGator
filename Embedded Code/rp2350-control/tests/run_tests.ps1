param([string]$Compiler = 'gcc')
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
Push-Location $projectRoot
try {
    & $Compiler -std=c11 -Wall -Wextra -Werror -I tests/stubs -I . tests/receiver_test.c communication/src/data_parser.c communication/src/rover_i2c.c peripherals/src/motor_speed_control.c peripherals/src/safety_devices.c control/src/rover_cli.c control/src/auto_control.c -o tests/receiver_test.exe
    if ($LASTEXITCODE -ne 0) { throw 'Receiver test compilation failed.' }
    & './tests/receiver_test.exe'
    if ($LASTEXITCODE -ne 0) { throw 'Receiver tests failed.' }
    $CppCompiler = $Compiler -replace 'gcc(\.exe)?$', 'g++$1'
    & $CppCompiler -std=c++11 -Wall -Wextra -Wno-unused-parameter -Werror -I tests/stubs/esp32 tests/esp32_link_test.cpp -o tests/esp32_link_test.exe
    if ($LASTEXITCODE -ne 0) { throw 'ESP32 reply timing test compilation failed.' }
    & './tests/esp32_link_test.exe'
    if ($LASTEXITCODE -ne 0) { throw 'ESP32 reply timing tests failed.' }
} finally {
    Pop-Location
}
