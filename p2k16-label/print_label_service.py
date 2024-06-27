#!/usr/bin/env python3
'''
 Copyright 2019 Elias Bakken

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
'''
from PIL import Image, ImageDraw, ImageFont
import qrcode
from brother_ql.conversion import convert
from brother_ql.backends.helpers import send
from brother_ql.raster import BrotherQLRaster
import paho.mqtt.client as mqtt
import json
import tempfile

USER_URL="https://p2k16.bitraf.no/#!/user/"

offset_x = 40
offset_y = 360
picture_height=470
picture_width=370

model="QL-1060N"
backend="network"
printer="tcp://etikett2.lan.bitraf.no"
image_path="/tmp/user.png"
mqtt_topic="/public/p2k16-dev/label/#"

def make_label(user_id, username, name, telephone, email):
	im = qrcode.make(USER_URL+str(user_id))
	background = Image.new('RGBA', (picture_width, picture_height), (255, 255, 255, 255))
	background.paste(im, (0, 0))
	draw = ImageDraw.Draw(background)
	fnt = ImageFont.truetype('third_party/Open_Sans/OpenSans-Regular.ttf', 16)
	draw.text((offset_x,offset_y), "User name: {}".format(username), font=fnt, fill=(0,0,0,255))
	draw.text((offset_x,offset_y + 20), "Name: {}".format(name), font=fnt, fill=(0,0,0,255))
	draw.text((offset_x,offset_y + 40), "Phone: {}".format(telephone), font=fnt, fill=(0,0,0,255))
	draw.text((offset_x,offset_y + 60), "Email: {}".format(email), font=fnt, fill=(0,0,0,255))
	background.save(image_path)

def print_label():
	qlr = BrotherQLRaster(model)
	qlr.exception_on_warning = True
	kwargs = {}
	kwargs['cut'] = True
	kwargs['images'] = [image_path]
	kwargs['label'] = "102"
	instructions = convert(qlr=qlr, **kwargs)
	send(instructions=instructions, printer_identifier=printer, backend_identifier=backend, blocking=True)

def on_connect(client, userdata, flags, rc):
	client.subscribe(mqtt_topic)

def on_message(client, userdata, msg):
	try:
		user = json.loads(msg.payload.decode("utf-8"))
		make_label(user["id"], user["username"], user["name"], user["phone"], user["email"])
		print_label()
	except:
		print(f"Unable to parse message: {msg.payload}")

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.will_set("/public/p2k16-dev/label-alive/alive", "False", 0, True)
client.connect("mqtt.bitraf.no", 1883, 60)

client.publish("/public/p2k16-dev/label-alive/alive", "True", 0, True)

client.loop_forever()
