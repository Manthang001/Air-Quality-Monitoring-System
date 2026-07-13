import serial
import csv
import os

PORT = 'COM4'   #We need to change this to whatever port      
BAUD_RATE = 115200   
CSV_FILE = 'sensor_data.csv'

HEADERS = ["PM2.5", "PM10", "NO2", "NH3", "03","CO"] 

def main():
    try:
        ser = serial.Serial(PORT, BAUD_RATE, timeout=1)
    except Exception as e:
        print(f"Could not open port {PORT}: {e}")
        return

    file_exists = os.path.isfile(CSV_FILE)
    
    print(f"Logging data to {CSV_FILE}. Press Ctrl+C to stop.")
    
    try:
        with open(CSV_FILE, mode='a', newline='') as file:
            writer = csv.writer(file)
            
            if not file_exists:
                writer.writerow(HEADERS)

            while True:
                if ser.in_waiting > 0:
                    # Read and clean the data
                    line = ser.readline().decode('utf-8').strip()
                    if line:
                        data = line.split(',')
                        
                        # Validate that we received all 6 values
                        if len(data) == 6:
                            writer.writerow(data)
                            file.flush() # Save data to disk immediately
    except KeyboardInterrupt:
        print("\nLogging stopped by user.")
    finally:
        ser.close()

if __name__ == "__main__":
    main()