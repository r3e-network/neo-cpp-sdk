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

def swift_file_to_cpp_name(swift_path):
    """Convert Swift file path to expected C++ file name."""
    filename = os.path.basename(swift_path)
    name_without_ext = os.path.splitext(filename)[0]
    
    # Convert camelCase to snake_case
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name_without_ext)
    snake_case = re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()
    
    return snake_case

def main():
    swift_dir = "/Users/jinghuiliao/git/r3e/NeoSwift/Sources/NeoSwift"
    cpp_include_dir = "/Users/jinghuiliao/git/r3e/NeoSwift/NeoCpp/include/neocpp"
    cpp_src_dir = "/Users/jinghuiliao/git/r3e/NeoSwift/NeoCpp/src"
    
    # Get all Swift files
    swift_files = get_files(swift_dir, '.swift')
    
    # Get all C++ header and source files
    cpp_headers = get_files(cpp_include_dir, '.hpp')
    cpp_sources = get_files(cpp_src_dir, '.cpp')
    
    # Extract base names
    swift_names = [swift_file_to_cpp_name(f) for f in swift_files]
    cpp_header_names = [os.path.splitext(os.path.basename(f))[0] for f in cpp_headers]
    cpp_source_names = [os.path.splitext(os.path.basename(f))[0] for f in cpp_sources]
    
    # Find missing files
    missing_headers = []
    missing_sources = []
    
    for i, swift_name in enumerate(swift_names):
        if not any(swift_name in header_name for header_name in cpp_header_names):
            missing_headers.append((swift_files[i], swift_name))
        
        if not any(swift_name in source_name for source_name in cpp_source_names):
            missing_sources.append((swift_files[i], swift_name))
    
    # Print results
    print("Top 10 Missing Headers:")
    for i, (swift_file, name) in enumerate(missing_headers[:10]):
        print(f"{i+1}. {os.path.basename(swift_file)} -> {name}.hpp")
    
    print("\nTop 10 Missing Sources:")
    for i, (swift_file, name) in enumerate(missing_sources[:10]):
        print(f"{i+1}. {os.path.basename(swift_file)} -> {name}.cpp")
    
    print(f"\nTotal: {len(missing_headers)} missing headers, {len(missing_sources)} missing sources")

if __name__ == "__main__":
    main()