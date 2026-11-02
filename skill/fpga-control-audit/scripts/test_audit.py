"""Behavior checks for the read-only audit helper; no toolchain required."""
import hashlib
from pathlib import Path
import tempfile
import unittest
import sys

sys.dont_write_bytecode = True
from audit_project import audit


class AuditTests(unittest.TestCase):
    def fixture(self, root):
        (root / 'src').mkdir()
        source = root / 'src/control.c'
        source.write_bytes(b'int main(void) { return 0; }\n')
        (root / 'docs').mkdir()
        digest = hashlib.sha256(source.read_bytes()).hexdigest()
        (root / 'docs/source_sha256.txt').write_text(
            digest + '  src/control.c\n', encoding='utf-8')
        return source

    def test_original_and_changed_source(self):
        with tempfile.TemporaryDirectory(prefix='fpga_audit_') as temp:
            root = Path(temp).resolve()
            source = self.fixture(root)
            self.assertEqual(audit(root)['failures'], 0)
            source.write_bytes(b'changed\n')
            result = audit(root)
            provenance = next(c for c in result['checks'] if c['check'] == 'source_provenance')
            self.assertEqual(provenance['status'], 'FAIL')
            self.assertEqual(provenance['detail']['changed'], ['src/control.c'])

    def test_strict_missing_assets(self):
        with tempfile.TemporaryDirectory(prefix='fpga_audit_') as temp:
            root = Path(temp).resolve()
            self.fixture(root)
            self.assertEqual(audit(root, strict=True)['failures'], 4)

    def test_strict_asset_presence(self):
        with tempfile.TemporaryDirectory(prefix='fpga_audit_') as temp:
            root = Path(temp).resolve()
            self.fixture(root)
            for relative in ['src/top.v', 'src/top.xdc', 'build/run.tcl', 'data/readings.csv']:
                p = root / relative
                p.parent.mkdir(parents=True, exist_ok=True)
                p.write_text('fixture\n', encoding='utf-8')
            self.assertEqual(audit(root, strict=True)['failures'], 0)

    def test_manifest_cannot_escape_root(self):
        with tempfile.TemporaryDirectory(prefix='fpga_audit_') as temp:
            root = Path(temp).resolve()
            self.fixture(root)
            (root / 'docs/source_sha256.txt').write_text(
                '0' * 64 + '  ../outside.c\n', encoding='utf-8')
            result = audit(root)
            check = next(c for c in result['checks'] if c['check'] == 'source_provenance')
            self.assertEqual(check['status'], 'FAIL')
            self.assertEqual(check['detail']['malformed'], ['../outside.c'])


if __name__ == '__main__':
    unittest.main()
