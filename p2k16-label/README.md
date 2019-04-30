
The Print label service is a daemon that listens to 
messages on mqtt and prints a QR code and name, telephone and email on that label.

## Prepare installation

```sudo pip3 install qrcode brother_ql paho-mqtt```

## Running
```./print_label_service.py```

## Testing
mosquitto_pub -h mqtt.bitraf.no  -t "public/p2k16-dev/label/" -m '{"username": "test", "id": 1, "name": "Test Test", "phone": "01234567", "email": "test@example.com"}'


## License

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

