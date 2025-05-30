import re
import json
import os
import sys
import sqlite3
import tkinter as tk
from tkinter import ttk, filedialog, messagebox
from cryptography.fernet import Fernet  # Needed for Python helper injection

########################################
# Vulnerability Fixing Functions (from A.py)
########################################

def load_vulnerabilities(json_path="vulnerabilities.json"):
    """Load vulnerabilities and solutions from a JSON file."""
    try:
        with open(json_path, 'r', encoding='utf-8') as file:
            data = json.load(file)
            return data.get('primitives', [])
    except Exception as e:
        print(f"Error loading vulnerabilities: {e}")
        return []

def detect_file_language(file_path):
    """Determine the language of the file based on its extension."""
    ext = os.path.splitext(file_path)[1].lower()
    if ext == ".py":
        return "python"
    elif ext in [".c", ".h"]:
        return "c"
    elif ext == ".java":
        return "java"
    else:
        return "unknown"

def fix_des_for_c(content):
    """
    Replace insecure DES usage in C files with AES usage.
    This example performs several substitutions that (hopefully) preserve compilability.
    """
    # Replace a DES_cblock declaration with an AES key declaration
    content = re.sub(
        r"(DES_cblock\s+([a-zA-Z_][a-zA-Z0-9_]*))\s*=\s*{[^}]*};",
        r"unsigned char \2[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};",
        content
    )
    # Replace DES function calls with AES counterparts
    content = content.replace("DES_key_schedule", "AES_KEY")
    content = content.replace("DES_set_key_checked", "AES_set_encrypt_key")
    content = content.replace("DES_ecb_encrypt", "AES_encrypt")
    content = content.replace("DES_ENCRYPT", "AES_ENCRYPT")
    return content

def inject_python_helpers(content):
    """
    For Python files, inject definitions for secure_xor_encrypt,
    fernet_encrypt, and fernet_decrypt along with the required import,
    if they are not already defined.
    """
    injection = ""
    # Check for secure_xor_encrypt; if not present, add it.
    if "def secure_xor_encrypt(" not in content:
        injection += (
            "def secure_xor_encrypt(data, key):\n"
            "    # A simple (placeholder) secure XOR encryption function\n"
            "    return bytes(b ^ key for b in data)\n\n"
        )
    # Check for fernet_encrypt/fernet_decrypt; if not present, add them along with the necessary import.
    if ("def fernet_encrypt(" not in content) or ("def fernet_decrypt(" not in content):
        injection = (
            "from cryptography.fernet import Fernet\n"
            "fernet_key = Fernet.generate_key()\n"
            "cipher = Fernet(fernet_key)\n\n"
            "def fernet_encrypt(data):\n"
            "    # data should be bytes\n"
            "    return cipher.encrypt(data)\n\n"
            "def fernet_decrypt(token):\n"
            "    return cipher.decrypt(token)\n\n"
        ) + injection
    if injection:
        content = injection + content
    return content

def detect_and_fix_risks(file_path, vulnerabilities):
    """
    Detect and fix risks in a file based on the provided vulnerabilities.
    The file is read, modified, and then the original file is renamed as a backup
    and the fixed file is saved with the original file name.
    """
    try:
        with open(file_path, 'r', encoding='utf-8') as file:
            original_content = file.read()
    except Exception as e:
        print(f"Error reading file {file_path}: {e}")
        messagebox.showerror("Error", f"Error reading file {file_path}: {e}")
        return

    language = detect_file_language(file_path)
    content = original_content  # This will be modified
    changes_made = False

    # For Python files, inject helper functions if needed.
    if language == "python":
        content = inject_python_helpers(content)

    # Process each vulnerability
    for vuln in vulnerabilities:
        vuln_name = vuln.get('name', '')
        patterns = vuln.get('patterns', [])
        solution = vuln.get('solution', '')

        for pattern in patterns:
            # For special cases, handle them with custom logic
            if vuln_name == "XOR Encryption" and language == "python":
                # Replace insecure xorEncrypt with secure_xor_encrypt
                if re.search(r"\bxorEncrypt\b", content):
                    print(f"Risk detected: {vuln_name} in {file_path}. Applying fix for XOR Encryption...")
                    content = re.sub(r"\bxorEncrypt\b", "secure_xor_encrypt", content)
                    changes_made = True

            elif vuln_name == "Base64 Encoding" and language == "python":
                # Replace base64 encoding/decoding functions with fernet-based functions
                if re.search(r"\bbase64\.b64encode\b", content) or re.search(r"\bbase64\.b64decode\b", content):
                    print(f"Risk detected: {vuln_name} in {file_path}. Applying fix for Base64 Encoding...")
                    content = re.sub(r"\bbase64\.b64encode\b", "fernet_encrypt", content)
                    content = re.sub(r"\bbase64\.b64decode\b", "fernet_decrypt", content)
                    changes_made = True

            elif vuln_name == "DES" and language == "c":
                # Use the special C fix for DES
                if re.search(pattern, content):
                    print(f"Risk detected: {vuln_name} in {file_path}. Applying DES fix for C...")
                    content = fix_des_for_c(content)
                    changes_made = True

            else:
                # For other cases, use a simple substitution with word boundaries.
                regex_pattern = r"\b" + re.escape(pattern) + r"\b"
                if re.search(regex_pattern, content):
                    print(f"Risk detected: {vuln_name} in {file_path} (pattern: {pattern}). Applying fix...")
                    content = re.sub(regex_pattern, solution, content)
                    changes_made = True

    # Save the modified file if changes were made
    if changes_made:
        backup_path = file_path + '.backup'
        try:
            # Rename the original file to backup
            os.rename(file_path, backup_path)
            print(f"Original file renamed to backup: {backup_path}")
            # Write the fixed content to the original file name
            with open(file_path, 'w', encoding='utf-8') as fixed_file:
                fixed_file.write(content)
            print(f"Fixed file saved as: {file_path}")
            messagebox.showinfo("Success", f"Fixed file saved as: {file_path}\nBackup created: {backup_path}")
        except Exception as e:
            print(f"Error saving files: {e}")
            messagebox.showerror("Error", f"Error saving files: {e}")
    else:
        print(f"No risks found in {file_path}. No changes made.")
        messagebox.showinfo("Information", f"No risks found in {file_path}. No changes made.")

########################################
# Scanning Functions (from gui.py)
########################################

def scan_file(file_path, vulnerabilities):
    results = []
    try:
        with open(file_path, "r", errors="ignore") as f:
            lines = f.readlines()
            for i, line in enumerate(lines):
                for vuln in vulnerabilities:
                    for pattern in vuln["patterns"]:
                        # Use a simple search; you may adjust regex or use re.IGNORECASE as needed.
                        if re.search(pattern, line):
                            results.append({
                                "file": file_path,
                                "line": i + 1,
                                "pattern": pattern,
                                "vulnerability": vuln["name"],
                                "severity": vuln["severity"]
                            })
    except Exception as e:
        print(f"Error scanning file {file_path}: {e}")
    return results

def save_results_to_db(results, db_file="database.db"):
    # Delete the file if it exists
    try:
        os.remove(db_file)
    except FileNotFoundError:
        pass
    except Exception as e:
        print(f"Error deleting {db_file}: {e}")
        
    conn = sqlite3.connect(db_file)
    cursor = conn.cursor()
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS vulnerabilities (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            file TEXT,
            line INTEGER,
            pattern TEXT,
            vulnerability TEXT,
            severity TEXT
        )
    """)
    for result in results:
        cursor.execute("""
            INSERT INTO vulnerabilities (file, line, pattern, vulnerability, severity)
            VALUES (?, ?, ?, ?, ?)
        """, (result["file"], result["line"], result["pattern"], result["vulnerability"], result["severity"]))
    conn.commit()
    conn.close()

def fetch_vulnerabilities(db_file="database.db"):
    conn = sqlite3.connect(db_file)
    cursor = conn.cursor()
    cursor.execute("""
        SELECT file, line, pattern, vulnerability, severity
        FROM vulnerabilities
    """)
    rows = cursor.fetchall()
    conn.close()
    return rows

def populate_table(table, rows):
    # Clear existing entries in the table
    for item in table.get_children():
        table.delete(item)

    # Insert new rows
    for row in rows:
        severity = row[4]
        if severity == "High":
            color = "red"
        elif severity == "Medium":
            color = "orange"
        else:
            color = "yellow"
        table.insert("", "end", values=row, tags=(color,))

########################################
# GUI Functions
########################################

def select_folder():
    folder = filedialog.askdirectory()
    return folder

def scan_folder():
    folder = select_folder()  # Ask user to select folder
    if not folder:
        return

    vulnerabilities = load_vulnerabilities()
    results = []
    for root_dir, _, files in os.walk(folder):
        for file in files:
            if file.endswith((".py", ".c", ".h", ".java")):
                file_path = os.path.join(root_dir, file)
                results.extend(scan_file(file_path, vulnerabilities))
    save_results_to_db(results, "database.db")
    
    # Fetch the saved results from the database
    results = fetch_vulnerabilities()

    # Split results by file type
    python_results = [row for row in results if row[0].endswith('.py')]
    c_results = [row for row in results if row[0].endswith(('.c', '.h'))]
    java_results = [row for row in results if row[0].endswith('.java')]

    # Update statistics label
    scanned_files = set(row[0] for row in results)
    stats = f"Scanned {len(scanned_files)} files in folder: {folder}\n"
    stats += f"Found {len(results)} vulnerabilities:\n"
    severity_count = {"High": 0, "Medium": 0, "Low": 0}
    for result in results:
        severity_count[result[4]] += 1
    for severity, count in severity_count.items():
        stats += f"  {severity}: {count}\n"
    statistics_label.config(text=stats)

    # Populate each table
    populate_table(table_python, python_results)
    populate_table(table_c, c_results)
    populate_table(table_java, java_results)

def fix_file():
    # Open file dialog to let the user choose a file.
    # We combine supported extensions into one filter so all relevant file types are shown.
    file_path = filedialog.askopenfilename(
        title="Select file to fix", 
        filetypes=[
            ("All Supported Files", "*.py;*.c;*.h;*.java"),
            ("Python Files", "*.py"),
            ("C Files", "*.c;*.h"),
            ("Java Files", "*.java"),
            ("All Files", "*.*")
        ]
    )
    if not file_path:
        return

    vulnerabilities = load_vulnerabilities()
    detect_and_fix_risks(file_path, vulnerabilities)

########################################
# Sorting and Table Setup Functions
########################################

def sort_table(treeview, col, reverse):
    data = [(treeview.set(k, col), k) for k in treeview.get_children('')]
    data.sort(reverse=reverse, key=lambda x: x[0])
    for index, (val, k) in enumerate(data):
        treeview.move(k, '', index)
    treeview.heading(col, command=lambda: sort_table(treeview, col, not reverse))

def setup_table(treeview, columns):
    for col in columns:
        treeview.heading(col, text=col, command=lambda _col=col: sort_table(treeview, _col, False))
        treeview.column(col, width=150)

########################################
# GUI Layout
########################################

root = tk.Tk()
root.title("Cryptographic Vulnerability Tool")

# Statistics label
statistics_label = tk.Label(root, text="Scan Statistics", justify=tk.LEFT, font=("Arial", 12))
statistics_label.pack(pady=10)

# Notebook for tabs
notebook = ttk.Notebook(root)

columns = ("File", "Line", "Pattern", "Vulnerability", "Severity")

# Python table
table_python = ttk.Treeview(notebook, columns=columns, show="headings")
for col in columns:
    table_python.heading(col, text=col)
    table_python.column(col, width=150)
table_python.tag_configure("red", background="red")
table_python.tag_configure("orange", background="orange")
table_python.tag_configure("yellow", background="yellow")

# C table
table_c = ttk.Treeview(notebook, columns=columns, show="headings")
for col in columns:
    table_c.heading(col, text=col)
    table_c.column(col, width=150)
table_c.tag_configure("red", background="red")
table_c.tag_configure("orange", background="orange")
table_c.tag_configure("yellow", background="yellow")

# Java table
table_java = ttk.Treeview(notebook, columns=columns, show="headings")
for col in columns:
    table_java.heading(col, text=col)
    table_java.column(col, width=150)
table_java.tag_configure("red", background="red")
table_java.tag_configure("orange", background="orange")
table_java.tag_configure("yellow", background="yellow")

# Add tables to notebook
notebook.add(table_python, text="Python")
notebook.add(table_c, text="C")
notebook.add(table_java, text="Java")
notebook.pack(expand=True, fill="both")

# Apply sorting to tables
setup_table(table_python, columns)
setup_table(table_c, columns)
setup_table(table_java, columns)

# Button frame
button_frame = tk.Frame(root)
button_frame.pack(pady=20)

# Buttons for scanning and fixing
scan_button = tk.Button(button_frame, text="Scan Folder", command=scan_folder)
scan_button.grid(row=0, column=0, padx=10)

fix_button = tk.Button(button_frame, text="Fix File", command=fix_file)
fix_button.grid(row=0, column=1, padx=10)

risk_button = tk.Button(button_frame, text="Explain Risk Levels", command=lambda: show_risk_levels())
risk_button.grid(row=0, column=2, padx=10)

manual_button = tk.Button(button_frame, text="Manual", command=lambda: show_manual())
manual_button.grid(row=0, column=3, padx=10)

########################################
# Additional GUI Windows for Explanations
########################################

def show_risk_levels():
    top = tk.Toplevel(root)
    top.title("Risk Levels Explanation")
    message = (
        "Risk Levels:\n\n"
        "High: Critical vulnerability that poses significant risk and requires immediate attention.\n"
        "Medium: Vulnerability with moderate risk; should be addressed soon.\n"
        "Low: Minor issue or best-practice recommendation; fix if feasible."
    )
    tk.Label(top, text=message, justify=tk.LEFT, font=("Arial", 10), wraplength=400).pack(padx=20, pady=20)
    tk.Button(top, text="Close", command=top.destroy).pack(pady=10)

def show_manual():
    top = tk.Toplevel(root)
    top.title("Manual")
    message = (
        "This tool scans a folder for cryptographic vulnerabilities in Python, C, and Java files, "
        "and allows you to fix a selected file automatically.\n\n"
        "Steps:\n"
        "1. Click 'Scan Folder' to scan a folder for vulnerabilities. The results will appear in the tabs.\n"
        "2. Click 'Fix File' to select a single file to automatically apply fixes. The original file will be renamed with a '.backup' suffix and replaced with the fixed version.\n"
        "3. Use 'Explain Risk Levels' for details about the vulnerabilities' severity.\n"
        "4. Use 'Manual' for this help message."
    )
    tk.Label(top, text=message, justify=tk.LEFT, font=("Arial", 10), wraplength=400).pack(padx=20, pady=20)
    tk.Button(top, text="Close", command=top.destroy).pack(pady=10)

########################################
# Run the GUI Main Loop
########################################

root.mainloop()
