import tkinter as tk
from tkinter import filedialog
from pathlib import Path

def main():
    root = tk.Tk()
    root.withdraw()

    # Select sample files
    sample_paths = filedialog.askopenfilenames(
        title="Select Sample Files"
    )

    if not sample_paths:
        print("No files selected.")
        return

    # Choose output file
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
            f.write(f"{i} {Path(path).as_posix()};\n")

    print(f"Saved {len(sample_paths)} entries to {output_file}")

if __name__ == "__main__":
    main()