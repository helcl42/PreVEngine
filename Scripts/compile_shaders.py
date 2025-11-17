import argparse
import os
import shutil
import glob
import subprocess
import time

def get_compiler_path():
    COMPILER_NAME = 'glslc'
    VK_SDK_PATH = os.getenv('VULKAN_SDK')
    if VK_SDK_PATH != None:
        return os.path.join(VK_SDK_PATH, 'Bin', COMPILER_NAME)
    return COMPILER_NAME

def replace_last(input, to_replace, replacement):
    return replacement.join(input.rsplit(to_replace, 1))

def create_output_shader_path(shader_path):
    return replace_last(shader_path, '.', '_') + '.spv'

def compile_shader(shader_path, compile_serial, compiler_args):
    compiler_path = get_compiler_path()
    output_path = create_output_shader_path(shader_path)
    print('Compiling:', output_path, '...')
    args = [compiler_path, shader_path, '-c', '-o', output_path]
    if compiler_args:
        args.extend(compiler_args)
#     print(args)
    handle = subprocess.Popen(args, shell=False, stdin=subprocess.PIPE, universal_newlines=True)
    if compile_serial == True:
        handle.wait()
        return None
    return handle

def compile_shaders_with_extension(input_folder, extension, force_compile_all, compile_serial, compiler_args):
    handles = []
    shader_paths = glob.glob(input_folder + f'/**/*.{extension}', recursive=True)
    current_time = time.time();
    for shader_path in shader_paths:
        time_delta = current_time - os.path.getmtime(shader_path)
        time_delta_hours = time_delta / (60 * 60)
        if time_delta_hours < 24 or force_compile_all:
            handle = compile_shader(shader_path, compile_serial, compiler_args)
            handles.append(handle)
    return handles

def compile_shaders(input_folder, force_compile_all, compile_serial, compiler_args):
    all_handles = []
    extensions = ['vert', 'tesc', 'tese', 'geom', 'frag', 'comp']
    for ext in extensions:
        group_handles = compile_shaders_with_extension(input_folder, ext, force_compile_all, compile_serial, compiler_args)
        all_handles.extend(group_handles)

    print('Waiting for', len(all_handles), 'tasks...')
    for handle in all_handles:
        if handle:
            handle.wait()

def copy_to_output(input_folder, output_folder):
    if os.path.exists(output_folder):
        shutil.rmtree(output_folder)
    shutil.copytree(input_folder, output_folder)

def process(input_folder, output_folder, force_compile_all, compile_serial, compiler_args):
    start = time.time()
    compile_shaders(input_folder, force_compile_all, compile_serial, compiler_args)
    copy_to_output(input_folder, output_folder)
    end = time.time()
    print('Execution took:', (end - start), 's.')

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="This script compiles all shaders recursively and copies results to specified output folder.")
    parser.add_argument('--input_folder',
                        help='Path to a folder with shaders to compile.', required=True)
    parser.add_argument('--output_folder',
                        help='Path to a folder where outputs will be copied.', required=True)
    parser.add_argument('--force_compile_all',
                        help='This invokes compilation for all shaders (even for unmodified ones).', required=False, default=False, action='store_true')
    parser.add_argument('--compile_serial',
                        help='This arguments ensures that all shader compilation task will run in serial manner(meant for debugging).', required=False, default=False, action='store_true')
    parser.add_argument('--compiler_args', action='append',
                        help='Specifies optional compiler args.', required=False)
    args = parser.parse_args()
    print(args)
    process(args.input_folder, args.output_folder, args.force_compile_all, args.compile_serial, args.compiler_args)

# Example usage
#  python .\compile_shaders.py --input_folder "<a_path>" --output_folder "<a_path>" --compiler_args="-O" --compile_serial --force_compile_all

# python compile_shaders.py --input_folder ../Examples/PreVEngineExample/assets/Shaders/ --output_folder ../build_linux/Examples/PreVEngineExample/assets/Shaders/ --compile_serial --force_compile_all --compiler_args '-DMAX_VIEW_COUNT=2 ' --compiler_args '-DENABLE_XR=1 '
