f = open("output.txt", "r", encoding="utf-8")

lines = f.readlines()

filename = ''
bitrate = ''

for line in lines:

    if "from" in line:
        start = line.find('from')
        start = start + len('from ')
        filename = line[start:-2]

    if "bitrate" in line:
        
        start = line.find('bitrate')
        start = start + len('bitrate: ')
        bitrate = line[start:-6]
    
        if (len(filename) > 0):
            print(filename, ',', bitrate)
        