#!/usr/bin/python3

import regex;
import math;

notedict = {
    'r' : 0,
    'c' : 1,
    'c#' : 2,
    'd' : 3,
    'd#' : 4,
    'e' : 5,
    'f' : 6,
    'f#' : 7,
    'g' : 8,
    'g#' : 9,
    'a' : 10,
    'a#' : 11,
    'b' : 12,
}

def getnoteval(note, step):
    pitch = notedict.get(note)
    if pitch == 0:
        return pitch
    
    pitch += (int(step) - 1) * 12
    if note == 'a' or note == 'a#' or note == 'b':
        pitch -= 12
    return pitch

inp = open("input.txt","r")

out = open("output.txt","w")
out.write("static const uint8_t melody[] = { ")

r = regex.compile('([1-9]*)([a-z|A-Z]\\#{0,1})([1-9])')

for line in inp:
    notes = line.split(' ')
    for note in notes:
        m = r.match(note)
        notelength = math.log2(int(m.group(1)))
        assert notelength.is_integer()
            
        on = int(2**5 * notelength)
        pitch = getnoteval(m.group(2),m.group(3))
        assert pitch <= 0b00011111
        out.write(hex(on + pitch) + ", ")


out.write("0x00 };")