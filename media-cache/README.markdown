# media-cache

this set of docker containers sets up a cache in front of backblaze b2 for media.chitter.xyz

## usage

```
docker-compose up -d
```

will set up the varnish cache and proxy to b2

The cache is listening on `http://127.0.0.1:51442` and will need a reverse proxy in front of it to handle the TLS stuff

## usage with reverse proxy

usually there will be an instance of caddy reverse proxying a bunch of stuff but in emergency situations when I need a cache server from scratch, included is a config that will also set up a reverse proxy and set up TLS certs with letsencrypt and all that

```
docker-compose -f docker-compose.yml -f docker-compose.frontend.yml up -d
```
