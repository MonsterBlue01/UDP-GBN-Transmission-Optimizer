import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates

file_path = 'client_log.txt'

column_names = ["timestamp", "unknown", "ip", "port", "type", "value"]

df = pd.read_csv(file_path, names=column_names, header=None)

df['timestamp'] = pd.to_datetime(df['timestamp'])

print("Unique values in 'type' column:")
print(df['type'].unique())

df['type'] = df['type'].str.strip()

df_ack = df[df['type'] == 'ACK']
df_data = df[df['type'] == 'DATA']

print("ACK DataFrame head:")
print(df_ack.head())
print("\nDATA DataFrame head:")
print(df_data.head())

print("\nACK Timestamp range:")
print(df_ack['timestamp'].min(), df_ack['timestamp'].max())
print("\nDATA Timestamp range:")
print(df_data['timestamp'].min(), df_data['timestamp'].max())

print("\nACK Value statistics:")
print(df_ack['value'].describe())
print("\nDATA Value statistics:")
print(df_data['value'].describe())

plt.figure(figsize=(10,5))
plt.plot(df_ack['timestamp'], df_ack['value'], label='ACK')
plt.plot(df_data['timestamp'], df_data['value'], label='DATA')

plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%H:%M:%S'))
plt.gca().xaxis.set_major_locator(mdates.SecondLocator())

plt.xlabel('Time')
plt.ylabel('Value')
plt.title('ACK and DATA over Time')
plt.legend()
plt.show()
