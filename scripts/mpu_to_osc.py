import argparse
import serial

from pythonosc import udp_client
from pythonosc import osc_bundle_builder
from pythonosc import osc_message_builder


def send(client, y, p, r):
    """ Send yaw, pitch, roll to wekinator """
    msg = osc_message_builder.OscMessageBuilder(address="/wek/inputs")
    msg.add_arg(y)
    msg.add_arg(p)
    msg.add_arg(r)

    client.send_message('/wek/inputs', msg.build())


def transform_line(line):
    line = line.decode("utf-8")
    parts = line.split(",")
    try:
        return [float(i) for i in parts]
    except ValueError:
        return [None, None, None]

def main(port, oschost, oscport):
    client = udp_client.SimpleUDPClient(oschost, oscport)
    s = serial.Serial(port, timeout=20)
    while True:
        line = s.readline()
        print(line)
        y, p, r = transform_line(line)
        if y is not None:
            print(y, p, r)
            send(client, y, p, r)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('port', help='serial port to read from')
    parser.add_argument('oschost', help='osc host to send data to', default='localhost')
    parser.add_argument('oscport', type=int, help='osc port to send data to', default=6448)
    args = parser.parse_args()
    main(args.port, args.oschost, args.oscport)
