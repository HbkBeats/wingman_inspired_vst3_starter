# Independent Music Plugin — Windows VST3 starter

A beginner-friendly JUCE/CMake starter for an independent audio-analysis plugin.
The first version passes audio through unchanged and captures it into a bounded,
preallocated single-producer/single-consumer ring buffer for later analysis.

## Important limitations in this starter

- It does not yet analyze pitch, key, chords, or tempo.
- Captured audio is held in memory only; it is not written to disk.
- The capture buffer is intentionally bounded. If the consumer cannot keep up,
  samples may be dropped and an overflow counter is incremented.
- The UI timer polls lightweight status values; it does not perform analysis.
- A production version needs more host/DAW testing, state persistence, capture
  cancellation and explicit handling of all bus layouts.

## Requirements

- Windows 10/11 x64
- Visual Studio 2022 with **Desktop development with C++**
- CMake (Visual Studio's bundled CMake is fine)
- JUCE source checkout (download from https://juce.com/)
- FL Studio with VST3 support for host testing

## 1. Put JUCE somewhere stable

For example, extract or clone JUCE into:

`C:\Libraries\JUCE`

That directory must contain `CMakeLists.txt`.

## 2. Configure

Open **Developer PowerShell for VS 2022**, change directory to this project, then run:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DJUCE_DIR="C:/Libraries/JUCE"
```

Change `JUCE_DIR` if you installed JUCE elsewhere. If you use another Visual Studio
version, change the CMake generator accordingly.

## 3. Build

```powershell
cmake --build build --config Release
```

Find the generated `.vst3` bundle by looking in the `build` directory. The precise
output path can vary by JUCE/CMake version and configuration.

## 4. Test in FL Studio

1. Copy the generated `.vst3` bundle to a VST3 folder FL Studio scans, if it was not
   copied automatically. A common system folder is
   `C:\Program Files\Common Files\VST3`.
2. Open FL Studio and open **Options → Manage plugins**.
3. Add the VST3 folder if necessary and run a scan.
4. Insert **Independent Music Plugin** as an effect on an audio mixer track.
5. Start playback. The input meter in the plugin should respond.
6. Click **Start Capture**, play a few seconds, then click **Stop Capture**.
7. Confirm captured seconds increase. Audio should continue passing through normally.

Menu labels may vary between FL Studio versions.

## What the controls do

- **Start Capture**: starts copying input samples to the memory ring buffer.
- **Stop Capture**: stops capture and leaves queued data available to the consumer.
- **Clear Capture**: requests a clear on the next audio callback. This starter has no
  consumer/analysis worker yet, so it clears the queued ring buffer safely there.
- **Input level**: peak level for the most recent audio callback block.
- **Captured duration**: approximate queued audio duration, not a saved audio file.
- **Dropped samples**: number of samples that could not be queued because the buffer
  was full.

## Architecture notes

The audio callback does only bounded sample copying and atomic counter updates. It
does not allocate memory, lock a mutex, access the filesystem, or run FFT/ML analysis.
The ring buffer is single-producer/single-consumer: the audio callback is the producer.
A future background worker must be the only consumer.

Before shipping, test in multiple hosts, verify channel layouts, add cancellation and
generation IDs to analysis jobs, and review the JUCE and VST3 SDK licensing/terms.

## Build automatically on GitHub (Windows VST3)

This project includes a GitHub Actions workflow at
`.github/workflows/build-windows-vst3.yml`. It checks out JUCE 8.0.6, configures
the project with CMake on a Windows x64 runner, builds the VST3 target, and
uploads the resulting plugin bundle as a workflow artifact.

### First-time setup

1. Create or sign in to your GitHub account.
2. Create a new repository. A private repository is fine.
3. Upload the *contents* of this project folder to the repository (not the ZIP file
   itself). Make sure the `.github/workflows/build-windows-vst3.yml` file is included.
4. Name the default branch `main` if GitHub asks.
5. Open the repository's **Actions** tab. Select **Build Windows VST3** and click
   **Run workflow**. If the workflow is not listed yet, commit the workflow file to
   the `main` branch first.
6. Open the workflow run. When it finishes, download the artifact named
   `IndependentMusicPlugin-Windows-x64-VST3`.
7. Extract the downloaded artifact and locate the `.vst3` bundle inside it.
8. On Windows, copy the `.vst3` bundle into
   `C:\Program Files\Common Files\VST3` (administrator permission may be needed).
   Restart or rescan plugins in FL Studio: **Options → Manage plugins → Find installed plugins**.

### If the build fails

Open the failed workflow run, expand the step with the red X, and copy the first
meaningful compiler or CMake error. Share that error text to get help diagnosing it.
The workflow has not been verified in your GitHub account or inside FL Studio, so
the first run may reveal issues that need fixing.

### Important notes

- GitHub builds the code; it does not test the plugin inside FL Studio.
- The workflow uses a hosted Windows runner and the JUCE 8.0.6 tag. If JUCE or
  the project changes, the workflow may need updating.
- The plugin currently captures audio in a bounded buffer and displays input peak
  level. It does not yet analyze keys/chords, generate bass, or export MIDI.
- Keep the repository private if you do not want the source code to be public.
