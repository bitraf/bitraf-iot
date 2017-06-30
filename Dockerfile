FROM node:6

# Export the Websocket port for Flowhub connection
EXPOSE 3569

# Reduce npm install verbosity, overflows Travis CI log view
ENV NPM_CONFIG_LOGLEVEL warn
# Configure Bitraf MQTT
ENV MSGFLO_BROKER mqtt://mqtt.bitraf.no

RUN mkdir -p /var/bitraf-iot
WORKDIR /var/bitraf-iot

COPY . /var/bitraf-iot

# Install msgflo-python
RUN apt-get update && apt-get install -y \
  python \
  python-dev \
  python-pip
RUN pip install -r requirements.pip

# Install MsgFlo and dependencies
RUN npm install

# Map the volumes
VOLUME /var/bitraf-iot/graphs /var/bitraf-iot/components /var/bitraf-iot/spec

CMD npm start
