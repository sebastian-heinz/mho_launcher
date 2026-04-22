# mho_launcher

Companion launcher, injector, and observation hooks for running the original
**Monster Hunter Online** client (`MHOClient.exe`).

## Subprojects

| Target       | Source dir       | Output        | Purpose |
|--------------|------------------|---------------|---------|
| `ag_mho_exe` | `ag_mho_exe/`    | `ag_mho.exe`  | Launcher. Spawns `MHOClient.exe` suspended, optionally injects `ag_mho.dll`, then resumes on keypress. Reads `ag_mho.ini` to decide whether to inject. |
| `ag_mho_dll` | `ag_mho_dll/`    | `ag_mho.dll`  | Injected observation DLL. Installs jmp/call hooks in `MHOClient.exe`, `CryGame.dll`, `ProtocalHandler.dll`, `TenProxy.dll`, and `TerSafe.dll` to log client logs, static-data env setup, protocol TPDU encrypt/decrypt buffers, QQ login traffic, etc. |
| `tersafe_dll`| `tersafe_dll/`   | `TerSafe.dll` | Drop-in stub for Tencent's real `TerSafe.dll`. Exports the same symbols so the client loads cleanly without the real anti-cheat. |
| `bugtrace_dll`| `bugtrace_dll/` | `BugTrace.dll`| Drop-in stub for Tencent's `BugTrace.dll` (crash reporter). |
| `common/`    | header-only      | —             | Shared utilities |

## Config — `ag_mho.ini`

Both the launcher and the injected DLL read `ag_mho.ini` from their own exe
directory (which is the same `Bin32/` dir in practice). If the file is
missing, it's created with defaults.

```ini
# Inject ag_mho.dll into MHOClient.exe (0 = disabled, 1 = enabled)
inject_dll = 0

# Log client_log messages captured by the mhoclient hook (0 = disabled, 1 = enabled)
log_client_logs = 0

# Log encrypted network traffic (TPDU encrypt/decrypt input + output buffers) in protocalhandler (0 = disabled, 1 = enabled)
log_encrypted_network = 0
```

## Building

The build runs in a MinGW-w64 Docker image and produces 32-bit Windows
binaries (the client is 32-bit).

```bash
./build.sh
```

Outputs land in `out/`:

- `ag_mho.exe`
- `ag_mho.dll`
- `TerSafe.dll`
- `BugTrace.dll`

## Deploying

Copy all four artifacts next to `MHOClient.exe`:

Then run `ag_mho.exe` from that directory. It will create `ag_mho.ini` on
first run if one isn't already there. Edit the ini to flip features on.

## Running

```
ag_mho.exe                                  # default args
ag_mho.exe "<mho args>"                     # override MHOClient args
ag_mho.exe <mho_dir> <mho_exe> <mho_args>   # override everything
```

With `inject_dll = 1`, the launcher creates the process suspended, injects
`ag_mho.dll`, and waits for a keypress before resuming — giving you time to
attach a debugger. With `inject_dll = 0`, no injection happens; for
running the client.
