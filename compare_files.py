#!/usr/bin/env python3
import os
import sys
import re

def get_files(directory, extension):
    """Get all files with the given extension in the directory and subdirectories."""
    result = []
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(extension):
                result.append(os.path.join(root, file))
    return result

def swift_to_cpp_name(swift_path):
    """Convert Swift file path to expected C++ header path."""
    # Extract the file name without extension
    filename = os.path.basename(swift_path)
    name_without_ext = os.path.splitext(filename)[0]
    
    # Convert camelCase to snake_case
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name_without_ext)
    snake_case = re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()
    
    # Get the directory structure after NeoSwift/Sources/NeoSwift or NeoSwift/Tests
    if '/Sources/NeoSwift/' in swift_path:
        subpath = swift_path.split('/Sources/NeoSwift/')[1]
        directory = os.path.dirname(subpath)
        return directory, snake_case
    elif '/Tests/' in swift_path:
        subpath = swift_path.split('/Tests/')[1]
        directory = os.path.dirname(subpath)
        return directory, snake_case
    return None, None

def main():
    if len(sys.argv) < 4:
        print("Usage: python compare_files.py <swift_dir> <cpp_include_dir> <cpp_src_dir> [cpp_test_dir]")
        sys.exit(1)
    
    swift_dir = sys.argv[1]
    cpp_include_dir = sys.argv[2]
    cpp_src_dir = sys.argv[3]
    cpp_test_dir = sys.argv[4] if len(sys.argv) > 4 else None
    
    # Get all Swift files
    swift_files = get_files(swift_dir, '.swift')
    
    # Get all C++ header and source files
    cpp_headers = get_files(cpp_include_dir, '.hpp')
    cpp_sources = get_files(cpp_src_dir, '.cpp')
    cpp_tests = get_files(cpp_test_dir, '.cpp') if cpp_test_dir else []
    
    # Check for missing C++ files
    missing_headers = []
    missing_sources = []
    
    for swift_file in swift_files:
        directory, name = swift_to_cpp_name(swift_file)
        if not directory or not name:
            continue
        
        # Check if corresponding header exists
        header_found = False
        for header in cpp_headers:
            if name in os.path.basename(header).lower():
                header_found = True
                break
        
        if not header_found:
            missing_headers.append((swift_file, directory, name))
        
        # Check if corresponding source exists
        source_found = False
        for source in cpp_sources:
            if name in os.path.basename(source).lower():
                source_found = True
                break
        
        if not source_found:
            missing_sources.append((swift_file, directory, name))
    
    # Print results
    print(f"Total Swift files: {len(swift_files)}")
    print(f"Total C++ headers: {len(cpp_headers)}")
    print(f"Total C++ sources: {len(cpp_sources)}")
    print(f"Total C++ tests: {len(cpp_tests)}")
    
    print("\nMissing C++ headers:")
    for swift_file, directory, name in missing_headers:
        print(f"  {swift_file} -> {directory}/{name}.hpp")
    
    print("\nMissing C++ sources:")
    for swift_file, directory, name in missing_sources:
        print(f"  {swift_file} -> {directory}/{name}.cpp")

if __name__ == "__main__":
    main()