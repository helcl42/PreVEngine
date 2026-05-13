#!/usr/bin/env python3
"""
Shader compilation and management tool for PreVEngine

Usage:
    python compile_shaders.py              # Auto-detect and compile
    python compile_shaders.py --spirv      # Compile GLSL → SPIR-V only
    python compile_shaders.py --wgsl       # Convert SPIR-V → WGSL only
    python compile_shaders.py --clean      # Remove compiled shaders
    python compile_shaders.py --list       # List all shaders
"""

import os
import sys
import subprocess
import argparse
from pathlib import Path
from typing import List, Optional

class ShaderCompiler:
    def __init__(self, shader_dir: Path, max_view_count: int, enable_xr: bool):
        self.shader_dir = shader_dir
        self.src_dir = shader_dir / "src"
        self.spirv_dir = shader_dir / "spirv"
        self.wgsl_dir = shader_dir / "wgsl"
        self.max_view_count = max_view_count
        self.enable_xr = enable_xr
        
    def find_glsl_files(self) -> List[Path]:
        """Find all GLSL source files"""
        # Compile only stage entry-point shaders. Include files (*.glsl)
        # are intentionally excluded.
        extensions = ["*.vert", "*.frag", "*.comp", "*.geom"]
        glsl_files = []
        for ext in extensions:
            glsl_files.extend(self.src_dir.rglob(ext))
        return sorted(glsl_files)
    
    def find_spirv_files(self) -> List[Path]:
        """Find all compiled SPIR-V files"""
        return sorted(self.spirv_dir.rglob("*.spv"))
    
    def find_wgsl_files(self) -> List[Path]:
        """Find all WGSL files"""
        return sorted(self.wgsl_dir.rglob("*.wgsl"))
    
    def compile_glsl_to_spirv(self, glsl_file: Path) -> bool:
        """Compile single GLSL file to SPIR-V"""
        # Determine relative path and output location
        rel_path = glsl_file.relative_to(self.src_dir)
        spirv_file = self.spirv_dir / self._to_legacy_compiled_name(rel_path, ".spv")
        spirv_file.parent.mkdir(parents=True, exist_ok=True)
        
        # Find glslc
        glslc = self._find_executable("glslc")
        if not glslc:
            print(f"❌ glslc not found. Install VulkanSDK to compile shaders.")
            return False
        
        try:
            print(f"🔨 Compiling: {rel_path} → {spirv_file.relative_to(self.shader_dir)}")
            cmd = [
                glslc,
                f"-DMAX_VIEW_COUNT={self.max_view_count}",
            ]
            if self.enable_xr:
                cmd.append("-DENABLE_XR=1")
            cmd.extend(["-o", str(spirv_file), str(glsl_file)])
            subprocess.run(cmd,
                          check=True, capture_output=True)
            return True
        except subprocess.CalledProcessError as e:
            print(f"❌ Failed: {e.stderr.decode()}")
            return False
    
    def convert_spirv_to_wgsl(self, spirv_file: Path) -> bool:
        """Convert SPIR-V to WGSL using naga"""
        # Determine output location (replace .spv with .wgsl)
        rel_path = spirv_file.relative_to(self.spirv_dir)
        wgsl_file = self.wgsl_dir / rel_path.with_suffix(".wgsl")
        wgsl_file.parent.mkdir(parents=True, exist_ok=True)
        
        # Find naga
        naga = self._find_executable("naga")
        if not naga:
            print(f"⚠️  naga not found - skipping WGSL generation. "
                  f"Install: cargo install naga-cli")
            return False
        
        try:
            print(f"🔄 Converting: {rel_path} → {wgsl_file.relative_to(self.shader_dir)}")
            subprocess.run([naga, str(spirv_file), str(wgsl_file)], 
                          check=True, capture_output=True)
            return True
        except subprocess.CalledProcessError as e:
            print(f"⚠️  Conversion failed (non-critical): {e.stderr.decode()[:100]}")
            return False
    
    def compile_all_spirv(self) -> bool:
        """Compile all GLSL to SPIR-V"""
        glsl_files = self.find_glsl_files()
        if not glsl_files:
            print("No GLSL files found in src/")
            return False
        
        print(f"📦 Found {len(glsl_files)} shader(s)")
        success_count = 0
        for glsl_file in glsl_files:
            if self.compile_glsl_to_spirv(glsl_file):
                success_count += 1
        
        print(f"✅ Compiled {success_count}/{len(glsl_files)} shaders")
        return success_count == len(glsl_files)
    
    def convert_all_wgsl(self) -> bool:
        """Convert all SPIR-V to WGSL"""
        spirv_files = self.find_spirv_files()
        if not spirv_files:
            print("No SPIR-V files found - compile GLSL first")
            return False
        
        print(f"📦 Found {len(spirv_files)} SPIR-V file(s)")
        success_count = 0
        for spirv_file in spirv_files:
            if self.convert_spirv_to_wgsl(spirv_file):
                success_count += 1
        
        print(f"✅ Converted {success_count}/{len(spirv_files)} shaders (non-critical failures ignored)")
        return True
    
    def clean(self):
        """Remove all compiled shaders"""
        import shutil
        for directory in [self.spirv_dir, self.wgsl_dir]:
            if directory.exists():
                shutil.rmtree(directory)
                print(f"🗑️  Removed {directory}")
    
    def list_shaders(self):
        """List all shaders and their compiled versions"""
        print("\n📄 GLSL Sources (src/):")
        for f in self.find_glsl_files():
            print(f"  • {f.relative_to(self.src_dir)}")
        
        print("\n🔵 SPIR-V Compiled (spirv/):")
        for f in self.find_spirv_files():
            print(f"  • {f.relative_to(self.spirv_dir)}")
        
        print("\n🟢 WGSL Converted (wgsl/):")
        for f in self.find_wgsl_files():
            print(f"  • {f.relative_to(self.wgsl_dir)}")
    
    @staticmethod
    def _find_executable(name: str) -> Optional[str]:
        """Find executable in PATH"""
        from shutil import which
        return which(name)

    @staticmethod
    def _to_legacy_compiled_name(rel_path: Path, out_ext: str) -> Path:
        """Map src/<cat>/name.<stage> to <cat>/name_<stage>.<ext>."""
        stage = rel_path.suffix.lstrip(".")
        stem = rel_path.stem
        return rel_path.with_name(f"{stem}_{stage}{out_ext}")


def main():
    parser = argparse.ArgumentParser(description="PreVEngine shader compiler")
    parser.add_argument("--spirv", action="store_true", help="Compile GLSL → SPIR-V only")
    parser.add_argument("--wgsl", action="store_true", help="Convert SPIR-V → WGSL only")
    parser.add_argument("--clean", action="store_true", help="Remove compiled shaders")
    parser.add_argument("--list", action="store_true", help="List all shaders")
    parser.add_argument("--max-view-count", type=int, default=1, help="Value for MAX_VIEW_COUNT define")
    parser.add_argument("--enable-xr", action="store_true", help="Enable ENABLE_XR define")
    
    args = parser.parse_args()
    
    # Find shader directory
    script_dir = Path(__file__).parent.absolute()
    shader_dir = script_dir.parent / "Examples" / "PreVEngineExample" / "assets" / "Shaders"

    # Backward-compatible fallback for older layout.
    if not shader_dir.exists():
        shader_dir = script_dir / "assets" / "Shaders"
    
    if not shader_dir.exists():
        print(f"❌ Shader directory not found: {shader_dir}")
        sys.exit(1)
    
    compiler = ShaderCompiler(shader_dir, args.max_view_count, args.enable_xr)
    
    if args.list:
        compiler.list_shaders()
    elif args.clean:
        compiler.clean()
    elif args.spirv:
        compiler.compile_all_spirv()
    elif args.wgsl:
        compiler.convert_all_wgsl()
    else:
        # Default: compile GLSL → SPIR-V → WGSL
        compiler.compile_all_spirv()
        compiler.convert_all_wgsl()
        compiler.list_shaders()


if __name__ == "__main__":
    main()
