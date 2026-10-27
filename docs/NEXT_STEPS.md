# Next implementation steps

1. Build and load this plugin in FL Studio.
2. Verify that input peak changes with audio and that capture duration increases.
3. Add unit tests for CaptureBuffer, including wrap-around and full-buffer behavior.
4. Add a background worker that is the sole consumer of CaptureBuffer.
5. Save captured audio into a temporary analysis object outside the audio callback.
6. Add FFT/chroma features and key ranking.
7. Add chord candidates and progression editing.
8. Add bass generation and MIDI export.

Do not add FFT, file I/O, model inference, memory allocation, or blocking locks to
processBlock(). Keep the real-time callback bounded.
