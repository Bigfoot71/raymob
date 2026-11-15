from pathlib import Path

root = Path(__file__).resolve().parents[1]
display = root / "deps" / "raymob" / "display.c"
assert display.is_file(), f"Missing Gradle preBuild dependency: {display}"
assert display.read_text(encoding="utf-8").strip(), "display.c must not be empty"
cmake = (root / "CMakeLists.txt").read_text(encoding="utf-8")
assert '"${CMAKE_CURRENT_SOURCE_DIR}/game/*.cpp"' in cmake
assert "raymoblib" not in cmake
print("Gradle preBuild compatibility path exists; legacy raymob backend is not linked.")
