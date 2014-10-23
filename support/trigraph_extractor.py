#!/usr/bin/env python3

import sys

if len(sys.argv) != 2:
    print("Usage: " + sys.argv[0] + " <word-file>")
    sys.exit(1)

words = [s.strip().lower() for s in open(sys.argv[1]).readlines()]

trigraphs = {}
for word in words:
    w = "XX" + word + "XX"
    for i in range(0, len(w)-2):
        tri = w[i:i+3]
        if tri[0] not in trigraphs:
            trigraphs[tri[0]] = {}
        if tri[1] not in trigraphs[tri[0]]:
            trigraphs[tri[0]][tri[1]] = {}
        if tri[2] not in trigraphs[tri[0]][tri[1]]:
            trigraphs[tri[0]][tri[1]][tri[2]] = 0
        trigraphs[tri[0]][tri[1]][tri[2]] += 1

for a in trigraphs.keys():
    for b in trigraphs[a].keys():
        amt = 0
        for c in trigraphs[a][b].keys():
            amt += trigraphs[a][b][c]
        for c in trigraphs[a][b].keys():
            trigraphs[a][b][c] /= amt

for a in trigraphs.keys():
    for b in trigraphs[a].keys():
        for c in trigraphs[a][b].keys():
            print(a,b,c,trigraphs[a][b][c])
