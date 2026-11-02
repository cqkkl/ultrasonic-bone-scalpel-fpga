#!/usr/bin/env python3
"""Read-only project inventory and provenance checks. Standard library only."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import sys


def audit(root, strict=False):
    files = sorted(p for p in root.rglob('*')
                   if p.is_file() and not p.is_symlink()
                   and '.git' not in p.relative_to(root).parts)
    checks = []

    def add(name, status, detail):
        checks.append({'check': name, 'status': status, 'detail': detail})

    bad_names = [str(p.relative_to(root)) for p in files
                 if any(not re.fullmatch(r'[A-Za-z0-9_.-]+', part)
                        for part in p.relative_to(root).parts)]
    add('portable_ascii_paths', 'WARN' if bad_names else 'PASS', bad_names)

    manifest = root / 'docs/source_sha256.txt'
    missing, changed, malformed = [], [], []
    checked = 0
    if manifest.is_file():
        for line in manifest.read_text(encoding='utf-8-sig').splitlines():
            if not line.strip() or line.startswith('#'):
                continue
            match = re.fullmatch(r'([0-9a-fA-F]{64})  (.+)', line)
            if not match:
                malformed.append(line)
                continue
            expected, relative = match.groups()
            target = (root / relative).resolve()
            if not target.is_relative_to(root):
                malformed.append(relative)
            elif not target.is_file():
                missing.append(relative)
            else:
                checked += 1
                if hashlib.sha256(target.read_bytes()).hexdigest() != expected.lower():
                    changed.append(relative)
        valid = checked > 0 and not (missing or changed or malformed)
        add('source_provenance', 'PASS' if valid else 'FAIL',
            {'verified_files': checked, 'missing': missing,
             'changed': changed, 'malformed': malformed})
    else:
        add('source_provenance', 'WARN', 'No optional docs/source_sha256.txt manifest.')

    requirements = {
        'hdl_sources': [p for p in files if p.suffix.lower() in {'.v', '.sv', '.vhd', '.vhdl'}],
        'timing_constraints': [p for p in files if p.suffix.lower() in {'.xdc', '.sdc'}],
        'build_entrypoints': [p for p in files
                              if 'build' in p.relative_to(root).parts
                              and p.suffix.lower() in {'.tcl', '.ps1', '.sh', '.py'}],
        'measured_data': [p for p in files
                          if 'data' in p.relative_to(root).parts
                          and p.suffix.lower() in {'.csv', '.json', '.mat', '.npz'}],
    }
    for name, paths in requirements.items():
        add(name, 'PASS' if paths else ('FAIL' if strict else 'WARN'),
            [p.relative_to(root).as_posix() for p in paths])

    failures = sum(c['status'] == 'FAIL' for c in checks)
    return {'schema_version': 1, 'mode': 'full_project' if strict else 'snapshot',
            'files_scanned': len(files), 'checks': checks,
            'failures': failures,
            'scope': 'Presence and provenance only; no compile, timing, algorithm or hardware certification.'}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('root', type=Path)
    parser.add_argument('--require-full-project', action='store_true')
    args = parser.parse_args()
    root = args.root.resolve()
    if not root.is_dir():
        parser.error('root must be an existing directory')
    result = audit(root, args.require_full_project)
    print(json.dumps(result, ensure_ascii=True, indent=2))
    return 1 if result['failures'] else 0


if __name__ == '__main__':
    sys.exit(main())
