import collections
import csv
import re
import struct
import sys
import time
import matplotlib.animation as animation
import matplotlib.pyplot as plt
import serial

PORT = 'COM7'
BAUD = 115200
RAW_HEX_FILE = 'raw_data.hex'
PARSED_CSV_FILE = 'parsed_data.csv'
MAX_POINTS = 100

REGEX = re.compile(r'radio_rx\s+([0-9a-fA-F]{12})')

time_data = collections.deque(maxlen=MAX_POINTS)
temp_data = collections.deque(maxlen=MAX_POINTS)
press_data = collections.deque(maxlen=MAX_POINTS)
lux_data = collections.deque(maxlen=MAX_POINTS)

try:
    ser = serial.Serial(PORT, BAUD, timeout=0.1)
except serial.SerialException as e:
    sys.exit(f"Serial connection failed: {e}")

with open(PARSED_CSV_FILE, 'w', newline='') as f:
    writer = csv.writer(f)
    writer.writerow(['timestamp', 'raw_hex', 'temperature_c', 'pressure_hpa', 'lux'])

fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(9, 7), sharex=True)
line_temp, = ax1.plot([], [], 'r-', label='Temperature (°C)')
line_press, = ax2.plot([], [], 'b-', label='Pressure (hPa)')
line_lux, = ax3.plot([], [], 'g-', label='Lux')

ax1.set_ylabel('Temp (°C)')
ax2.set_ylabel('Pressure (Pa)')
ax3.set_ylabel('Lux (lx)')
ax3.set_xlabel('Elapsed Time (s)')

for ax in (ax1, ax2, ax3):
    ax.legend(loc='upper right')
    ax.grid(True)

start_time = time.time()

def update_plot(frame):
    while ser.in_waiting:
        try:
            line = ser.readline().decode('utf-8', errors='ignore').strip()
        except Exception:
            continue

        match = REGEX.search(line)
        if not match:
            continue

        hex_str = match.group(1)
        
        with open(RAW_HEX_FILE, 'a') as f:
            f.write(f"{hex_str}\n")

        raw_bytes = bytes.fromhex(hex_str)
        # 2 bytes signed short (temp), 2 bytes unsigned short (pressure), 2 bytes unsigned short (lux)
        temp, press, lux = struct.unpack('>hHH', raw_bytes)
        #own calculations i guess:
        temp -= 1 << 14
        temp /= 100.0
        press += 1 << 16

        elapsed = time.time() - start_time
        time_data.append(elapsed)
        temp_data.append(temp)
        press_data.append(press)
        lux_data.append(lux)

        with open(PARSED_CSV_FILE, 'a', newline='') as f:
            writer = csv.writer(f)
            writer.writerow([round(elapsed, 2), hex_str, temp, press, lux])

    if time_data:
        line_temp.set_data(time_data, temp_data)
        line_press.set_data(time_data, press_data)
        line_lux.set_data(time_data, lux_data)

        for ax, data in zip((ax1, ax2, ax3), (temp_data, press_data, lux_data)):
            ax.set_xlim(min(time_data), max(time_data) if max(time_data) > 0 else 1)
            ax.set_ylim(min(data) - 5, max(data) + 5)

    return line_temp, line_press, line_lux

ani = animation.FuncAnimation(fig, update_plot, interval=100, cache_frame_data=False)

try:
    plt.tight_layout()
    plt.show()
finally:
    ser.close()
