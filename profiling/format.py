import sys

if __name__ == '__main__':
        THRESH = 50
        if len(sys.argv) < 2:
                print('usage: python3 map_from_profile.py <profile file>')
                exit(0)
        outfile = ('../exploitation/map.txt')
        with open(outfile, 'w') as writer:
                with open(sys.argv[1]) as reader:
                        for line in reader:
                                l = line.split(',')
                                for i in range(1, len(l)-1):
                                        s = l[i].split('=')
                                        if int(s[1]) > THRESH:
                                                writer.write('%s,%s\n' % (l[0], s[0]))
