import tkinter as tk
from tkinter import filedialog
from pathlib import Path

# CHANGE THIS if your folder name differs on Bela
BELA_BASE_PATH = "Sounds"

def main():
    root = tk.Tk()
    root.withdraw()

    sample_paths = filedialog.askopenfilenames(
        title="Select Sample Files"
    )

    if not sample_paths:
        print("No files selected.")
        return

    output_file = filedialog.asksaveasfilename(
        title="Save Sample List",
        defaultextension=".txt",
        filetypes=[("Text Files", "*.txt")]
    )

    if not output_file:
        print("No output file selected.")
        return

    with open(output_file, "w", encoding="utf-8") as f:
        for i, path in enumerate(sample_paths):

            filename = Path(path).name  # <-- ONLY the file name
            bela_path = f"{BELA_BASE_PATH}/{filename}"

            f.write(f"{i} {bela_path};\n")

    print(f"Saved {len(sample_paths)} entries to {output_file}")

if __name__ == "__main__":
    main()