"""
prerequisite:
   pip install pyserial

check COM ports in shell:
   python -m serial.tools.list_ports

usage:
   python serial_log.py COM4 115200    # USB
   python serial_log.py COM3 115200    # USB-2-TTL
"""
import sys
import serial

print(f"sys.argv: {sys.argv}")

if len(sys.argv) != 3:
   print(f"ERROR: need to provide 2 arguments")
   print(f"usage example: python {sys.argv[0]} COM4 115200")
   sys.exit(1)

#device_name, baudrate = 'COM4', 115200
device_name = sys.argv[1]
baudrate = int(sys.argv[2])

with serial.Serial(device_name, baudrate, timeout=None) as ser:
   print(f"serial: {ser}", flush=True)

   while True:
      l = ser.readline()
      #print([l])
      print(l.decode('utf-8').rstrip(), flush=True)
      #print(l.decode('utf-8'), flush=True, end='')
