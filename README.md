# ArtNet-MQTT

Purpose is to control christmas tree using QLC+ as a ArtNet DMX controller.

With QLC+ I am able to program lights and control christmas tree lights during play.

Python artNetServer.py receives artNet messages and then updates an MQTT server.
Arduino connects to MQTT server and set relays.
