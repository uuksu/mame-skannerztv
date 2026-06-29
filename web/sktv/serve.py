#!/usr/bin/env python3
"""Simple HTTP server with correct MIME types and COOP/COEP headers for SharedArrayBuffer."""
import http.server
import os
import sys

PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 8080

class Handler(http.server.SimpleHTTPRequestHandler):
    def end_headers(self):
        self.send_header('Cross-Origin-Opener-Policy', 'same-origin')
        self.send_header('Cross-Origin-Embedder-Policy', 'require-corp')
        super().end_headers()

    def guess_type(self, path):
        if path.endswith('.wasm'):
            return 'application/wasm'
        return super().guess_type(path)

os.chdir(os.path.dirname(os.path.abspath(__file__)))
print(f'Serving on http://localhost:{PORT}')
http.server.HTTPServer(('', PORT), Handler).serve_forever()
