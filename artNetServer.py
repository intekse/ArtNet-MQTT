#!/usr/bin/env python3
#title			:artNetServer.py
#description	:Receives artNet messages and updates MQTT server
#author			:Dan
#date			:20161220
#version		:0.1
#usage			:./artNetServer.py
#notes			:
#python_version	:3.4.2
#==============================================================================

import socket
import struct
import paho.mqtt.publish as publish

Universe = 0
UDP_IP = "192.168.1.102"
UDP_PORT = 6454
mainTopic = 'church/'
mqttHost = '192.168.1.102'
relay1 = 0
relay2 = 0
relay3 = 0

def decodeArtNet(data):
    s = struct.Struct('<8s H 4B H 2B 512B') 
    packet = s.unpack(data)

    packetLength = (int(packet[7]) << 8) + int(packet[8])
    myData = {
            'id':packet[0],
            'opcode':packet[1],
            'protver':(int(packet[2]) << 8) + int(packet[3]),
            'sequence':packet[4],
            'physical':packet[5],
            'universe':packet[6],
            'length':(int(packet[7]) << 8) + int(packet[8]),
            'payload':packet[9:packetLength]
            }

    return myData

def getPacket():
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    ArtNetPacket = decodeArtNet(data)
    return ArtNetPacket

def validPacket(ArtNetPacket):
    if ArtNetPacket['id'] != b'Art-Net\x00':
        return 'Invalid ID' 
    if ArtNetPacket['opcode'] != 20480:
        return 'Invalid Opcode'
    if ArtNetPacket['protver'] != 14:
        return 'Invalid protver'
    if ArtNetPacket['universe'] != Universe:
        return 'Invalid Universe'
    return 'OK'

def publishValues(ArtNetPacket):
    global relay1, relay2
    # Relay 1
    if ArtNetPacket['payload'][0] > 127:
        if relay1 == 0:
            publish.single(mainTopic + 'relay1/value', 0, retain=True, hostname=mqttHost)
            print(0)
            relay1 = 1
    else:
        if relay1 == 1:
            publish.single(mainTopic + 'relay1/value', 1, retain=True, hostname=mqttHost)
            print(1)
            relay1 = 0
    if ArtNetPacket['payload'][1] > 127:
        if relay2 == 0:
            publish.single(mainTopic + 'relay2/value', 0, retain=True, hostname=mqttHost)
            print(0)
            relay2 = 1
    else:
        if relay2 == 1:
            publish.single(mainTopic + 'relay2/value', 1, retain=True, hostname=mqttHost)
            print(1)
            relay2 = 0


def Setup():
    global sock

    sock = socket.socket(socket.AF_INET, # Internet
        socket.SOCK_DGRAM) # UDP
    sock.bind((UDP_IP, UDP_PORT))

def Main():
    Setup()
    while True:
        ArtNetPacket = getPacket()
        if validPacket(ArtNetPacket) == 'OK':
            publishValues(ArtNetPacket)
            #print(ArtNetPacket['payload'][:5])
        else:
            pass

if __name__ == "__main__":
    Main()
