#!/usr/bin/env python3

import time
import socket
import sys
import time
import string
import pprint
import collections
import binascii

frames_to_send = [b'\x10\x00\x00\x08\x00\x0d\x01\x00\x00\x00\x00\x00\x00',
b'\x10\x00\x00\x08\x00\x0d\x02\x00\x00\x00\x00\x00\x00',
b'\x10\x00\x00\x08\x00\x0d\x03\x00\x00\x00\x00\x00\x00',
b'\x10\x00\x00\x08\x00\x0a\x01\x01\x00\x00\x00\x00\x00',
b'\x10\x00\x00\x08\x00\x0a\x02\x01\x00\x00\x00\x00\x00',
b'\x10\x00\x00\x08\x00\x0a\x03\x01\x00\x00\x00\x00\x00',
b'\x10\x00\x00\x08\x00\x0a\x04\x01\x00\x00\x00\x00\x00',
b'\x10\x00\x00\x08\x00\x0a\x05\x01\x00\x00\x00\x00\x00',
b'\x10\x00\x00\x08\x00\x0a\x06\x01\x00\x00\x00\x00\x00',
b'\x10\x00\x00\x08\x00\x0a\x07\x01\x00\x00\x00\x00\x00',
b'\x10\x00\x00\x08\x00\x0a\x01\x02\x00\x00\x00\x00\x00',
b'\x10\x00\x00\x08\x00\x0a\x02\x02\x00\x00\x00\x00\x00',
b'\x10\x00\x00\x08\x00\x0a\x03\x02\x00\x00\x00\x00\x00',
b'\x10\x00\x00\x08\x00\x0a\x04\x02\x00\x00\x00\x00\x00',
b'\x10\x00\x00\x08\x00\x0a\x05\x02\x00\x00\x00\x00\x00',
b'\x10\x00\x00\x08\x00\x0a\x06\x02\x00\x00\x00\x00\x00',
b'\x10\x00\x00\x08\x00\x0a\x07\x02\x00\x00\x00\x00\x00']

def get_config():
    config = {}
    msg = ""

    for i in range(len(frames_to_send)):
        while sys.getsizeof(msg, int) != 46:
            s.send(frames_to_send[i])
            time.sleep(0.05)
            msg = s.recv(1024)
            if sys.getsizeof(msg, int) == 46:
                if (i == 0):
                    config['ip'] = ".".join(map(str, msg[9:13]))
                if (i == 1):
                    config['gateway'] = ".".join(map(str, msg[9:13]))
                if (i == 2):
                    config['mask'] = ".".join(map(str, msg[9:13]))
                if (i == 3):
                    config['can1-baud'] = (int)(msg[9])
                if (i == 4):
                    config['can1-tcpport'] = (int)((msg[10]<<8)+msg[9])
                if (i == 5):
                    config['can1-mode'] = (int)(msg[9])
                if (i == 6):
                    config['can1-filter'] = (int)(msg[9])
                if (i == 7):
                    config['can1-tcpmode'] = (int)(msg[9])
                if (i == 8):
                    config['can1-remoteip'] = ".".join(map(str, msg[9:13]))
                if (i == 9):
                    config['can1-remoteport'] = (int)((msg[10]<<8)+msg[9])
                if (i == 10):
                    config['can2-baud'] = (int)(msg[9])
                if (i == 11):
                    config['can2-tcpport'] = (int)((msg[10]<<8)+msg[9])
                if (i == 12):
                    config['can2-mode'] = (int)(msg[9])
                if (i == 13):
                    config['can2-filter'] = (int)(msg[9])
                if (i == 14):
                    config['can2-tcpmode'] = (int)(msg[9])
                if (i == 15):
                    config['can2-remoteip'] = ".".join(map(str, msg[9:13]))
                if (i == 16):
                    config['can2-remoteport'] = (int)((msg[10]<<8)+msg[9])
        msg=""
    config = collections.OrderedDict(config)
    pprint.pprint(config)

def set_config(config_to_set, value):
    if (config_to_set == 0):
        print("Setting module ip to", value, "...")
        split = value.split(".")
        string = '100000080102070000' + f"{int(split[0]):02x}" + f"{int(split[1]):02x}" + f"{int(split[2]):02x}" + f"{int(split[3]):02x}"
        s.send(binascii.a2b_hex(string))
    elif (config_to_set == 1):
        print("Setting gateway to", value, "...")
        split = value.split(".")
        string = '100000080102090000' + f"{int(split[0]):02x}" + f"{int(split[1]):02x}" + f"{int(split[2]):02x}" + f"{int(split[3]):02x}"
        s.send(binascii.a2b_hex(string))
    elif (config_to_set == 2):
        print("Setting subnet mask to", value, "...")
        split = value.split(".")
        string = '100000080102080000' + f"{int(split[0]):02x}" + f"{int(split[1]):02x}" + f"{int(split[2]):02x}" + f"{int(split[3]):02x}"
        s.send(binascii.a2b_hex(string))
    elif (config_to_set == 3):
        print("Setting can1 baudrate to", value, "...")
        string = '100000080102010100' + str(f"{int(value):02x}") + '000000'
        s.send(binascii.a2b_hex(string))
    elif (config_to_set == 4):
        print("Setting can1 tcp port to", value, "...")
        temp = "{:02x}".format(int(str(value)))
        string = '100000080102020100' + temp[2:4] + temp[0:2] + '0000'
        s.send(binascii.a2b_hex(string))
    elif (config_to_set == 5):
        print("Setting can1 can mode to", value, "...")
        string = '100000080102030100' + str(f"{int(value):02x}") + '000000'
        s.send(binascii.a2b_hex(string))
    elif (config_to_set == 6):
        print("Setting can1 tcp mode to", value, "...")
        string = '100000080102040100' + str(f"{int(value):02x}") + '000000'
        s.send(binascii.a2b_hex(string))
    elif (config_to_set == 7):
        print("Setting can1 remote IP to", value, "...")
        split = value.split(".")
        string = '100000080102050100' + f"{int(split[0]):02x}" + f"{int(split[1]):02x}" + f"{int(split[2]):02x}" + f"{int(split[3]):02x}"
        s.send(binascii.a2b_hex(string))
    elif (config_to_set == 8):
        print("Setting can1 remote port to", value, "...")
        temp = "{:02x}".format(int(str(value)))
        string = '100000080102060100' + temp[2:4] + temp[0:2] + '0000'
        s.send(binascii.a2b_hex(string))
    elif (config_to_set == 9):
        print("Setting can2 baudrate to", value, "...")
        string = '100000080102010200' + str(f"{int(value):02x}") + '000000'
        s.send(binascii.a2b_hex(string))
    elif (config_to_set == 10):
        print("Setting can2 tcp port to", value, "...")
        temp = "{:02x}".format(int(str(value)))
        string = '100000080102020200' + temp[2:4] + temp[0:2] + '0000'
        s.send(binascii.a2b_hex(string))
    elif (config_to_set == 11):
        print("Setting can2 can mode to", value, "...")
        string = '100000080102030200' + str(f"{int(value):02x}") + '000000'
        s.send(binascii.a2b_hex(string))
    elif (config_to_set == 12):
        print("Setting can2 tcp mode to", value, "...")
        string = '100000080102040200' + str(f"{int(value):02x}") + '000000'
        s.send(binascii.a2b_hex(string))
    elif (config_to_set == 13):
        print("Setting can2 remote IP to", value, "...")
        split = value.split(".")
        string = '100000080102050200' + f"{int(split[0]):02x}" + f"{int(split[1]):02x}" + f"{int(split[2]):02x}" + f"{int(split[3]):02x}"
        s.send(binascii.a2b_hex(string))
    elif (config_to_set == 14):
        print("Setting can2 remote port to", value, "...")
        temp = "{:02x}".format(int(str(value)))
        string = '100000080102060200' + temp[2:4] + temp[0:2] + '0000'
        s.send(binascii.a2b_hex(string))
    else:
        print("WARNING : Unrecognized config_to_set value\n")
        print_usage(0)
        sys.exit(0)

    #wait & apply
    time.sleep(0.25)
    s.send(b'\x10\x00\x00\x08\x01\x03\x01\x00\x00\x00\x00\x00\x00')
    print("\nWARNING !!!\nPlease electrically reboot your module to apply the configuration\nNote : You can apply multiple modification with one and only reboot.")

def print_usage(bool):
    print ("Usage :\nThis program is meant to get or set configs of a GCAN-202 CAN Ethernet module\n")
    print ("GET CONFIG:\n\tpython3 configurator.py get IP")
    print ("SET CONFIG:\n\tpython3 configurator.py set IP configID value")
    print ("--help:\n\tGet more information about setable configurations")

    if(bool == 1):
        print ("\n\nSetable configurations:\n")
        print ("Module IP : \n\tID : 0\n\t192.168.1.10")
        print ("Module subnet mask : \n\tID : 1\n\t255.255.255.0")
        print ("Module Gateway : \n\tID : 2\n\t192.168.1.1")
        print ("CAN baudrate : \n\tID : 3 (CAN1) or 9 (CAN2)")
        print("\t0=1Mbit/s\n\t1=800kbit/s\n\t2=666kbit/s\n\t3=500kbit/s\n\t4=400kbit/s\n\t5=250kbit/s\n\t6=200kbit/s\n\t7=125kbit/s\n\t8=100kbit/s\n\t9=80kbit/s\n\t10=50kbit/s\n\t11=40kbit/s\n\t12=20kbit/s\n\t13=10kbit/s\n\t14=5kbit/s")
        print ("CAN TCP Port : \n\tID : 4 (CAN1) or 10 (CAN2)\n\t4001")
        print ("CAN Mode : \n\tID : 5 (CAN1) or 11 (CAN2)\n\t0=Normal mode\n\t2=Listen mode\n\t4=Loopback mode")
        print ("CAN TCP Mode : \n\tID : 6 (CAN1) or 12 (CAN2)\n\t1=tcp server\n\t2=tcp client\n\t3=udp")
        print ("CAN Remote IP : \n\tID : 7 (CAN1) or 13 (CAN2)\n\t192.168.1.11")
        print ("CAN Remote port : \n\tID : 8 (CAN1) or 14 (CAN2)\n\t8001")

        print("\nExamples :")
        print("Get informations from module with IP 192.168.1.10\n\tconfigurator.py get 192.168.1.10\n")
        print("Change module IP from 192.168.1.10 to 192.168.1.15\n\tconfigurator.py set 192.168.1.10 0 192.168.1.15\n")
        print("Change CAN1 baudrate to 500kbits/s for module with IP 192.168.1.10\n\tconfigurator.py set 192.168.1.10 3 3\n")
        print("Change CAN2 TCP Port to 6789 for module with IP 192.168.1.10\n\tconfigurator.py set 192.169.1.10 10 6789")

if(len(sys.argv) > 1):
    if (sys.argv[1] == 'get' or sys.argv[1] == 'GET'):
        if len(sys.argv) < 3:
            print("WARNING : Need the module IP as argument\n")
            print_usage(0)
            sys.exit(0)

        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(3)
        try:
            s.connect((sys.argv[2], 22080))
        except OSError:
            print("WARNING : Can't connect to the IP address :",sys.argv[2], "!")
            sys.exit(0)
        s.settimeout(None)
        get_config()
        s.close()
    elif(sys.argv[1] == 'set' or sys.argv[1] == 'SET'):
        if len(sys.argv) < 5:
            print("WARNING : More arguments expected to set a configuration\n")
            print_usage(0)
            sys.exit(0)

        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(3)
        try:
            s.connect((sys.argv[2], 22080))
        except OSError:
            print("WARNING : Can't connect to the IP address :",sys.argv[2], "!")
            sys.exit(0)
        s.settimeout(None)
        try:
            set_config(int(sys.argv[3]), sys.argv[4])
        except ValueError:
            print("WARNING : Wrong arguments, can't set configuration\n")
            print_usage(0)
            sys.exit(0)
        s.close()
    elif(sys.argv[1] == '--help' or sys.argv[1] == '-h'):
        print_usage(1)
    else:
        print("WARNING : Unrecognized argument ->", sys.argv[1], "\n")
        print_usage(0)
        sys.exit(0)
else:
    print("WARNING : No argument\n")
    print_usage(0)
    sys.exit(0)
