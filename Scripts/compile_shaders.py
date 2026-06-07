#!/usr/bin/env python3
"""
Slang shader compilation tool for PreVEngine

Compiles .slang shaders to SPIR-V and/or WGSL using slangc.

Usage:
    python compile_shaders.py              # Compile all targets
    python compile_shaders.py --spirv      # SPIR-V only
    python compile_shaders.py --wgsl       # WGSL only
    python compile_shaders.py --clean      # Remove compiled outputs
    python compile_shaders.py --list       # List all shaders
    python compile_shaders.py --enable-xr --max-view-count 2
"""

import os
import sys
import subprocess
import argparse
import shutil
from pathlib import Path
from typing import List, Optional, Tuple


class SlangCompiler:
    # Shaders that use geometry stage (SPIR-V only, no WGSL)
    GEOMETRY_SHADERS = {"debug/raycast_debug.slang"}

    def __init__(self, shader_dir: Path, max_view_count: int, enable_xr: bool, debug: bool = False, force: bool = False):
        self.shader_dir = shader_dir
        self.slang_dir = shader_dir / "slang"
        self.spirv_dir = shader_dir / "spirv"
        self.wgsl_dir = shader_dir / "wgsl"
        self.max_view_count = max_view_count
        self.enable_xr = enable_xr
        self.debug = debug
        self.force = force

    def find_slang_files(self) -> List[Path]:
        """Find all Slang shader files that have entry points."""
        all_files = sorted(self.slang_dir.rglob("*.slang"))
        import re as _re
        return [f for f in all_files if _re.search(r'\[shader\(', f.read_text())]

    def _get_entry_points_and_stages(self, slang_file: Path) -> List[Tuple[str, str]]:
        """Parse a .slang file and return (entry_name, stage) pairs."""
        entries = []
        stage_map = {
            "vertex": "vertex",
            "fragment": "fragment",
            "compute": "compute",
            "geometry": "geometry",
        }
        content = slang_file.read_text()
        import re
        for match in re.finditer(r'\[shader\("(\w+)"\)\]', content):
            stage = match.group(1)
            # Find the function name on the next non-empty line
            pos = match.end()
            rest = content[pos:]
            func_match = re.search(r'(?:void|float4|float3|float2|float|\w+)\s+(\w+)\s*\(', rest)
            if func_match:
                entries.append((func_match.group(1), stage_map.get(stage, stage)))
        return entries

    def _stage_to_suffix(self, stage: str) -> str:
        stage_suffixes = {
            "vertex": "vert",
            "fragment": "frag",
            "compute": "comp",
            "geometry": "geom",
        }
        return stage_suffixes.get(stage, stage)

    def _output_name(self, rel_path: Path, stage: str, ext: str) -> Path:
        """Map slang/<cat>/name.slang + stage -> <cat>/name_<stage>.<ext>"""
        stem = rel_path.stem
        suffix = self._stage_to_suffix(stage)
        return rel_path.parent / f"{stem}_{suffix}{ext}"

    def _find_slangc(self) -> Optional[str]:
        """Find slangc executable."""
        # Check PATH first
        from shutil import which
        result = which("slangc")
        if result:
            return result
        # Check common install locations
        candidates = [
            "/tmp/slang-install/bin/slangc",
            "/usr/local/bin/slangc",
            Path.home() / ".local" / "bin" / "slangc",
        ]
        for c in candidates:
            if Path(c).is_file() and os.access(c, os.X_OK):
                return str(c)
        return None

    def _needs_recompile(self, slang_file: Path, out_file: Path) -> bool:
        """Check if output is missing or older than source or common imports."""
        if self.force:
            return True
        if not out_file.exists():
            return True
        out_mtime = out_file.stat().st_mtime
        # Check main source file
        if slang_file.stat().st_mtime > out_mtime:
            return True
        # Check shared imports in common/
        common_dir = self.slang_dir / "common"
        if common_dir.exists():
            for dep in common_dir.glob("*.slang"):
                if dep.stat().st_mtime > out_mtime:
                    return True
        return False

    def _build_base_args(self, slang_file: Path) -> List[str]:
        """Build common slangc arguments."""
        args = [
            f"-DMAX_VIEW_COUNT={self.max_view_count}",
            "-I", str(self.slang_dir),
        ]
        if self.enable_xr:
            args.append("-DENABLE_XR=1")
        if self.debug:
            args.extend(["-g2", "-O0"])
        return args

    def compile_to_spirv(self, slang_file: Path) -> bool:
        """Compile a .slang file to SPIR-V (one .spv per entry point)."""
        slangc = self._find_slangc()
        if not slangc:
            print("[ERROR] slangc not found. Install Slang SDK.")
            return False

        rel_path = slang_file.relative_to(self.slang_dir)
        entries = self._get_entry_points_and_stages(slang_file)
        if not entries:
            print(f"[WARN] No entry points found in {rel_path}")
            return False

        success = True
        all_skipped = True
        for entry_name, stage in entries:
            out_file = self.spirv_dir / self._output_name(rel_path, stage, ".spv")
            out_file.parent.mkdir(parents=True, exist_ok=True)

            if not self._needs_recompile(slang_file, out_file):
                continue
            all_skipped = False

            cmd = [slangc, str(slang_file)]
            cmd.extend(self._build_base_args(slang_file))
            cmd.extend([
                "-target", "spirv",
                "-profile", "glsl_450",
                "-capability", "spirv_1_3",
                "-fvk-use-entrypoint-name",
                "-entry", entry_name,
                "-stage", stage,
                "-o", str(out_file),
            ])

            try:
                print(f"  SPIR-V: {rel_path} [{entry_name}] -> {out_file.relative_to(self.shader_dir)}")
                subprocess.run(cmd, check=True, capture_output=True)
                out_file.touch()
            except subprocess.CalledProcessError as e:
                print(f"  [ERROR] {e.stderr.decode().strip()}")
                success = False

        return success, all_skipped

    def compile_to_wgsl(self, slang_file: Path) -> bool:
        """Compile a .slang file to WGSL (one .wgsl per entry point)."""
        rel_path = slang_file.relative_to(self.slang_dir)

        # Skip geometry shaders
        if str(rel_path).replace(os.sep, "/") in self.GEOMETRY_SHADERS:
            return True, True

        slangc = self._find_slangc()
        if not slangc:
            print("[ERROR] slangc not found. Install Slang SDK.")
            return False, False

        entries = self._get_entry_points_and_stages(slang_file)
        if not entries:
            print(f"[WARN] No entry points found in {rel_path}")
            return False, False

        success = True
        all_skipped = True
        for entry_name, stage in entries:
            if stage == "geometry":
                continue

            out_file = self.wgsl_dir / self._output_name(rel_path, stage, ".wgsl")
            out_file.parent.mkdir(parents=True, exist_ok=True)

            if not self._needs_recompile(slang_file, out_file):
                continue
            all_skipped = False

            cmd = [slangc, str(slang_file)]
            cmd.extend(self._build_base_args(slang_file))
            cmd.extend([
                "-target", "wgsl",
                "-entry", entry_name,
                "-stage", stage,
                "-o", str(out_file),
            ])

            try:
                print(f"  WGSL:  {rel_path} [{entry_name}] -> {out_file.relative_to(self.shader_dir)}")
                subprocess.run(cmd, check=True, capture_output=True)
                out_file.touch()
            except subprocess.CalledProcessError as e:
                print(f"  [ERROR] {e.stderr.decode().strip()}")
                success = False

        return success, all_skipped

    def compile_all(self, spirv: bool = True, wgsl: bool = True) -> bool:
        """Compile all Slang shaders (incremental by default)."""
        slang_files = self.find_slang_files()
        if not slang_files:
            print("No .slang files found")
            return False

        total = len(slang_files)
        total_ok = 0
        total_compiled = 0
        total_skipped = 0

        for slang_file in slang_files:
            rel = slang_file.relative_to(self.slang_dir)
            ok = True
            file_skipped = True
            if spirv:
                s_ok, s_skip = self.compile_to_spirv(slang_file)
                ok = s_ok and ok
                file_skipped = file_skipped and s_skip
            if wgsl:
                w_ok, w_skip = self.compile_to_wgsl(slang_file)
                ok = w_ok and ok
                file_skipped = file_skipped and w_skip
            if ok:
                total_ok += 1
            if file_skipped:
                total_skipped += 1
            else:
                total_compiled += 1

        if total_compiled == 0:
            print(f"All {total} shaders up-to-date.")
        else:
            print(f"\nResult: {total_compiled} compiled, {total_skipped} up-to-date, {total_ok}/{total} OK")
        return total_ok == total

    def clean(self):
        """Remove compiled outputs."""
        for directory in [self.spirv_dir, self.wgsl_dir]:
            if directory.exists():
                shutil.rmtree(directory)
                print(f"Removed {directory}")

    def list_shaders(self):
        """List all Slang shaders and their entry points."""
        slang_files = self.find_slang_files()
        print(f"\nSlang shaders ({len(slang_files)} files):\n")
        for f in slang_files:
            rel = f.relative_to(self.slang_dir)
            entries = self._get_entry_points_and_stages(f)
            entry_str = ", ".join(f"{name}({stage})" for name, stage in entries)
            print(f"  {rel}  [{entry_str}]")


def main():
    parser = argparse.ArgumentParser(description="PreVEngine Slang shader compiler")
    parser.add_argument("--spirv", action="store_true", help="Compile to SPIR-V only")
    parser.add_argument("--wgsl", action="store_true", help="Compile to WGSL only")
    parser.add_argument("--clean", action="store_true", help="Remove compiled shaders")
    parser.add_argument("--list", action="store_true", help="List all shaders")
    parser.add_argument("--max-view-count", type=int, default=1, help="MAX_VIEW_COUNT define value")
    parser.add_argument("--enable-xr", action="store_true", help="Enable XR defines")
    parser.add_argument("--debug", action="store_true", help="Emit debug info and disable optimizations (for RenderDoc, validation layers)")
    parser.add_argument("--force", action="store_true", help="Recompile all shaders even if up-to-date")
    parser.add_argument("--shader-dir", type=str, default=None, help="Override shader directory")

    args = parser.parse_args()

    if args.shader_dir:
        shader_dir = Path(args.shader_dir)
    else:
        script_dir = Path(__file__).parent.absolute()
        shader_dir = script_dir.parent / "Examples" / "PreVEngineExample" / "assets" / "Shaders"

    if not shader_dir.exists():
        print(f"[ERROR] Shader directory not found: {shader_dir}")
        sys.exit(1)

    compiler = SlangCompiler(shader_dir, args.max_view_count, args.enable_xr, args.debug, args.force)

    if args.clean:
        compiler.clean()
    elif args.list:
        compiler.list_shaders()
    else:
        do_spirv = args.spirv or (not args.spirv and not args.wgsl)
        do_wgsl = args.wgsl or (not args.spirv and not args.wgsl)
        success = compiler.compile_all(spirv=do_spirv, wgsl=do_wgsl)
        sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
