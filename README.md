# CaptoGlove HRI – FANUC Robot Controller

A gesture-based Human-Robot Interface (HRI) application for controlling a FANUC industrial robot using CaptoGlove smart gloves. The system uses a two-glove setup where the left glove selects control modes and the right glove executes motion commands. Communication with the robot is handled via TCP/IP.

---

## Requirements

- Windows 10 or later
- Visual Studio 2019 or later
- FANUC robot or RobotGuide simulation with TCP/IP enabled
- Two CaptoGlove HMG devices (left + right)
- Bluetooth adapter

---

## Dependencies

The CaptoGlove SDK is included in this repository under the `CaptoGlove/` folder:

```
CaptoGlove/
    Include/
    Lib/
```

---

## Visual Studio Setup

1. Open the project in Visual Studio.

2. Add the SDK include path:
   - Right-click the project → **Properties**
   - Go to **C/C++ → General → Additional Include Directories**
   - Add: `$(SolutionDir)CaptoGlove\Include`

3. Link the static library:
   - Go to **Linker → General → Additional Library Directories**
   - Add: `$(SolutionDir)CaptoGlove\Lib`
   - Go to **Linker → Input → Additional Dependencies**
   - Add: `GSdkCoreStatic.lib`

4. Make sure the build configuration matches the library — use **x64 Release** or **x64 Debug** accordingly.

---

## Configuration

Edit `config.txt` in the project root before running:

```
Robot_Port=23432
LeftGlove_ID=4305
RightGlove_ID=4272
```

- `Robot_Port` — TCP port on which the application listens for the robot connection
- `LeftGlove_ID` — last 4 digits of the left glove Bluetooth ID
- `RightGlove_ID` — last 4 digits of the right glove Bluetooth ID

---

## Running the Application

1. Start the KAREL program on the robot/RobotGuide first.
2. Run the compiled application.
3. Put on both gloves and wait for Bluetooth connection.
4. Load calibration automatically from `calibration_Left.csv` and `calibration_Right.csv`.

### Controls

| Key | Action |
|-----|--------|
| `c` | Start calibration |
| `s` | Save calibration step |
| `e` | Start gesture experiment |
| `x` | Stop experiment early |
| `m` | Send manual command (3-digit code) |
| `q` | Quit application |

---

## Calibration

Calibration data is saved to and loaded from:
- `calibration_Left.csv`
- `calibration_Right.csv`

To recalibrate, press `c` and follow the on-screen instructions. Press `s` to confirm each step.

---
---

# CaptoGlove HRI – FANUC Robot Controller (SK)

Aplikácia pre gestom ovládané rozhranie človek-robot (HRI) na ovládanie priemyselného robota FANUC pomocou inteligentných rukavíc CaptoGlove. Systém využíva dve rukavice — ľavá vyberá režim ovládania, pravá vykonáva pohybové príkazy. Komunikácia s robotom prebieha cez TCP/IP.

---

## Požiadavky

- Windows 10 alebo novší
- Visual Studio 2019 alebo novšie
- Robot FANUC alebo simulácia RobotGuide s povoleným TCP/IP
- Dve rukavice CaptoGlove HMG (ľavá + pravá)
- Bluetooth adaptér

---

## Závislosti

CaptoGlove SDK je súčasťou tohto repozitára v priečinku `CaptoGlove/`:

```
CaptoGlove/
    Include/
    Lib/
```

---

## Nastavenie Visual Studio

1. Otvor projekt vo Visual Studio.

2. Pridaj cestu k hlavičkovým súborom SDK:
   - Pravý klik na projekt → **Properties**
   - Prejdi na **C/C++ → General → Additional Include Directories**
   - Pridaj: `$(SolutionDir)CaptoGlove\Include`

3. Nalinkuj statickú knižnicu:
   - Prejdi na **Linker → General → Additional Library Directories**
   - Pridaj: `$(SolutionDir)CaptoGlove\Lib`
   - Prejdi na **Linker → Input → Additional Dependencies**
   - Pridaj: `GSdkCoreStatic.lib`

4. Skontroluj, že konfigurácia zostavenia zodpovedá knižnici — použi **x64 Release** alebo **x64 Debug** podľa potreby.

---

## Konfigurácia

Pred spustením uprav súbor `config.txt` v koreňovom priečinku projektu:

```
Robot_Port=23432
LeftGlove_ID=4305
RightGlove_ID=4272
```

- `Robot_Port` — TCP port, na ktorom aplikácia čaká na pripojenie robota
- `LeftGlove_ID` — posledné 4 cifry Bluetooth ID ľavej rukavice
- `RightGlove_ID` — posledné 4 cifry Bluetooth ID pravej rukavice

---

## Spustenie aplikácie

1. Najprv spusti KAREL program na robote/RobotGuide.
2. Spusti skompilovanú aplikáciu.
3. Nasaď si obe rukavice a počkaj na Bluetooth pripojenie.
4. Kalibrácia sa načíta automaticky zo súborov `calibration_Left.csv` a `calibration_Right.csv`.

### Ovládanie

| Kláves | Akcia |
|--------|-------|
| `c` | Spustiť kalibráciu |
| `s` | Uložiť krok kalibrácie |
| `e` | Spustiť experiment s gestami |
| `x` | Predčasne ukončiť experiment |
| `m` | Manuálne zadanie príkazu (3-ciferný kód) |
| `q` | Ukončiť aplikáciu |

---

## Kalibrácia

Kalibračné dáta sa ukladajú do a načítavajú zo súborov:
- `calibration_Left.csv`
- `calibration_Right.csv`

Na opätovnú kalibráciu stlač `c` a postupuj podľa pokynov na obrazovke. Každý krok potvrď stlačením `s`.
