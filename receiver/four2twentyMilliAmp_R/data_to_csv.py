import serial
import sys

with serial.Serial('COM4', 115200) as ser:
    with open('data_to.csv', 'wb') as csv_file:
        header = b'data, loop_current'
        csv_file.write(header)
        csv_file.write(b'\n')
        print(header)

        line_count = 0
        while True:
            line = ser.readline()
            if not line.startswith(b'V:  '): continue
            #print(line)

            data, current = line[4:].strip().split(b' ')

            result = b', '.join([data, current])
            csv_file.write(result)
            csv_file.write(b'\n')
            csv_file.flush()

            line_count += 1
            if line_count % 100 == 5:
                print(result)
                sys.stdout.flush()

