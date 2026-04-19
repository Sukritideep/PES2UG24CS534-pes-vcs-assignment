# PES-VCS — A Simplified Version Control System

## Overview

PES-VCS is a simplified Git-like version control system implemented in C.  
It demonstrates fundamental version control and filesystem concepts such as:

- Content-addressable object storage
- Staging area (index)
- Tree-based directory representation
- Commit history tracking

The system mimics basic Git operations in a lightweight manner.

---

## Features

- Blob storage using hashing
- Deduplication of identical file contents
- Tree structures for directory snapshots
- Commit creation with parent linkage
- Commit history traversal (log)
- Simple staging area (index)

---

## Supported Commands

```bash
./pes init              # Initialize repository
./pes add <file>...     # Stage files
./pes status            # Show staged files
./pes commit -m "msg"   # Create commit
./pes log               # Show commit history
```
```bash
Project Structure
.pes/
├── objects/        # Stores blobs, trees, commits
├── refs/
│   └── heads/
│       └── main    # Branch reference
├── index           # Staging area
└── HEAD            # Points to current branch
```
Build Instructions

Compile the project using:
```bash

gcc object.c tree.c index.c commit.c pes.c -o pes
```

Usage Example
```bash
./pes init
echo hello > file.txt
./pes add file.txt
./pes commit -m "Initial commit"
./pes log
```
Testing

Phase 1 — Object Tests
```bash
gcc test_objects.c object.c -o test_objects

./test_objects
```
Phase 2 — Tree Tests
```bash
gcc test_tree.c tree.c object.c index.c -o test_tree
./test_tree
```
Integration Test
```bash
bash test_sequence.sh
```
Implementation Notes
A simplified hashing mechanism is used instead of OpenSSL.
Integrity checking is partially implemented due to custom hashing.
Designed and tested in a Windows (PowerShell) environment.
Concepts Demonstrated
Content-addressable storage
File deduplication
Tree-based filesystem representation
Commit graph (linked history)
Index-based staging workflow
Conclusion

PES-VCS successfully replicates the core working principles of a version control system like Git in a simplified form. It provides hands-on understanding of how versioning, storage, and history tracking work internally.
