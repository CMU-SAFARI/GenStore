import sys
import itertools

headers = []
read = ""
qual = ""
last_name = ""
last_score = 0
last_cigar = "*"
total_reads = 0
total_exact = 0
total_unmapped = 0
with open(sys.argv[1], 'r') as f:
    for line in f:
        if line[0] == '@':
            continue
        sp = line.rstrip().split("\t")
        if sp[0] != last_name:
            if len(last_name):
                total_reads += 1
                if last_cigar == "*":
                    total_unmapped += 1
                elif last_cigar == "{}=".format(len(read)):
                    total_exact += 1
                else:
                    print("@{}\n{}\n+\n{}".format(last_name,read,qual))
            last_score = 0
            last_cigar = "*"
            read = sp[9]
            qual = sp[10]
        last_name = sp[0]
        if sp[2] == "*":
            continue

        score = int(sp[13].split(":")[2])
        if score > last_score:
            last_score = score
            last_cigar = sp[5]

if len(last_name):
    total_reads += 1
    if last_cigar == "*":
        total_unmapped += 1
    elif last_cigar == "{}=".format(read):
        total_exact += 1
    else:
        print("@{}\n{}\n+\n{}".format(last_name,read,qual))

sys.stderr.write("{} {} {}\n".format(total_reads, total_exact, total_unmapped))
