# Build the Windows VST3 without installing a compiler locally

This repository includes a GitHub Actions workflow that builds a 64-bit Windows VST3 using Microsoft's Windows runner.

1. Create a GitHub repository and upload the contents of this folder (the `CMakeLists.txt`, `Source` folder, and `.github` folder must be at the repository root).
2. Open the repository's **Actions** tab and select **Build Windows VST3**.
3. Click **Run workflow**. Wait for the run to finish successfully.
4. Open the completed run and download the `SaltGlueBus-Windows-x64-VST3` artifact.
5. Extract the ZIP. Copy `Salt Glue Bus.vst3` into `C:\Program Files\Common Files\VST3\`.
6. Restart Studio One or rescan plug-ins.

This workflow is a build route, not a precompiled binary. The artifact is only available after the workflow successfully completes. This is an independent mix-bus effect inspired by common mix-bus processing, not an official Black Salt Audio product.
