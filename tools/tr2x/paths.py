from pathlib import Path

TR2X_TOOLS_DIR = Path(__file__).parent.parent
TR2X_REPO_DIR = TR2X_TOOLS_DIR.parent
TR2X_DOCS_DIR = TR2X_REPO_DIR / "docs"
TR2X_DATA_DIR = TR2X_REPO_DIR / "data"
TR2X_SRC_DIR = TR2X_REPO_DIR / "src"

TR2X_PROGRESS_FILE = TR2X_DOCS_DIR / "progress.txt"
