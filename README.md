# BingPictureOfTheDayLoader

A minimal, dependency‑free **C++98** utility built with **C++Builder 6** that downloads the **Bing Picture of the Day** and saves it locally.  
The project is intentionally lightweight, highly portable, and suitable for legacy Windows systems.

---

## Features

- **Pure C++98** — fully compatible with Borland C++Builder 6.
- **Extremely lightweight** — no external libraries, no frameworks, no dynamic dependencies except for WinInet.
- **Portable across old Windows versions** — uses only WinInet, available since Windows 98/2000.

---

## Requirements

- C++Builder 6  
- Windows with WinInet available (Windows 98/2000/XP and later)

No additional DLLs or packages are required.

---

## Program Flow

The program follows a linear, predictable sequence:

1. **Initialize WinInet session**  
   Creates an internet handle and prepares for HTTP requests.

2. **Download Bing metadata XML**  
   Retrieves the XML feed containing the current Picture of the Day information.

3. **Parse the XML manually**  
   A lightweight string‑search parser extracts the `"<url>"` field without using any XML library.

4. **Download the image**  
   Performs a second HTTP request to fetch the actual image bytes.

5. **Save the image to disk**  
   Writes the downloaded buffer to a file in temp.

6. **Cleanup**  
   Closes all WinInet handles and frees allocated memory.

This design avoids exceptions, templates, RTTI, and heavy STL usage, making the program extremely portable and suitable for constrained or legacy environments.

---

## Project Structure

```
BingPictureOfTheDayLoader/
 ├── main.cpp        # Core logic: WinInet calls, parsing, file writing
 ├── main.h          # Declarations
 ├── internet.h      # Minimal WinInet wrapper utilities
 ├── BingPOTD.bpr    # C++Builder 6 project file
 ├── LICENSE
```

---

## Portability Notes

This project is engineered for maximum compatibility:

- Written strictly in **C++98**, avoiding modern features.
- No external dependencies — the executable is self‑contained.
- Uses only WinInet, which is available on nearly all Windows systems from the early 2000s.
- Suitable for embedded Windows environments, thin clients, and VMs.

---

## Building

1. Open the project in **C++Builder 6**  
2. Select **Build All**  
3. Run the resulting executable

No configuration steps are required.

---

## License

This project is released under the **GNU GPL License**.  
See `LICENSE` for details.
