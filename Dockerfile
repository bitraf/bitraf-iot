FROM node:6

# Export the Websocket port for Flowhub connection
EXPOSE 3569

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
RUN NPM_CONFIG_LOGLEVEL=warn npm install

# Map the volumes
VOLUME /var/bitraf-iot/graphs /var/bitraf-iot/components /var/bitraf-iot/spec

CMD npm start
