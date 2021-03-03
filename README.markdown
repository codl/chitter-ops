# chitter operations 🦝🔧

this repository documents non-obvious parts of chitter's architecture

## servers

There are two servers involved:

* `tempo.chitter.xyz` or [`tempo`](https://www.mobygames.com/game/sega-32x/tempo)

    * `10.235.0.4`
      `176.9.92.130`
      `2a01:4f8:151:337a::1`

    * PostgreSQL, Redis, and media hosting / proxying

    * Hetzner auction server in Falkenstein (FSN1)

      Core i7-2600, 250 GB SATA SSD, 3+3 TB SATA HDD, 8+8 GiB DDR3 RAM

    * Paid for by Patreon patrons (€36.30/mo) (Thank you!)

* `parasect.codl.fr` or [`parasect`](https://bulbapedia.bulbagarden.net/wiki/Parasect_(Pok%C3%A9mon))

    * `10.235.0.3`
      `144.76.7.70`
      `2a01:4f8:190:8245::2`

    * Ingress traffic, Application server, Sidekiq queue workers, media hosting

    * Hetzner auction server in Falkenstein (FSN1)

    * Paid out of pocket by codl

    * On its way out. `tempo` is more than capable of handling both it and
      `parasect`'s jobs

## app

the app server, streaming server, sidekiq, are all on `parasect`, running with docker-compose

the app and streaming server are behind a caddy instance, which handles serving static content, reverse proxying, and all the TLS stuff. the [Caddyfile](/Caddyfile) is in this repo

The PostgreSQL and Redis databases are on `tempo`.

The two servers are on a tinc VPN with name `chitter` and network `10.235.0.0/16`,
PostgreSQL and Redis are set up to only accept connections from that network.
A couple of my dev machines also have keys and connect to this network, which I
know isn't ideal security wise. (Reason being it started as an administrative
network and I didn't take the time to make another one)

> TODO document backups

> TODO document deployment procedure

## media

Media is stored in three places, because we've moved media a bunch and I never
bothered consolidating it afterwards. Some media is on Backblaze B2 in the
`chitter-media` bucket, some media is served from a minio instance on
`parasect`, and the rest, including all newly uploaded media, is on a minio
instance on `tempo`. Nginx on `tempo` takes care of trying all three locations (in
reverse order) and caching responses from the first two. The [nginx config](/chitter-media.nginx.inc)
is in this repo.

Incoming requests to `media.chitter.xyz` go to BunnyCDN, which then goes to `tempo`.

The full public URL to the B2 bucket is `https://f001.backblazeb2.com/file/chitter-media/`

`parasect`'s media is at `https://chitter-media.parasect.codl.fr`.

`tempo`'s proxy/cache/media is at `https://tempo.orig.media.chitter.xyz`.
Inconsistent, I know.

Test URLs:

* <https://f001.backblazeb2.com/file/chitter-media/mascots/danfoxx.png>
* <https://tempo.orig.media.chitter.xyz/mascots/danfoxx.png>
* <https://media.chitter.xyz/mascots/danfoxx.png>

### historical media locations

because toots are (indefinitely?) cached on remote servers with whatever media url was appropriate at the time, the following locations should continue serving or redirecting to the canonical location for media:

* `https://media.chitter.xyz/` current canonical URL
* `https://media-b.chitter.xyz/` was occasionally used when rotating servers. served by bunnycdn identically to the other one
* `https://chitter-media.codl.fr/` points to `tempo`, which `301 Moved Permanently`s to `media.chitter.xyz`
* `https://chitter.xyz/system/` currently 404s 😕

## [status](https://github.com/codl/status.chitter.xyz)

<https://status.chitter.xyz/> is hosted on netlify

updown.io is set up to monitor chitter.xyz, specifically [the public timeline api endpoint][tl], and the index page of media.chitter.xyz. this page fetches statuses for both of those thru the updown.io api

for outage details, `updates.html` in the `chitter-outages` S3 bucket is loaded every 10 seconds and inserted in the page

this has been designed to not have any server component. everything is static html or third-party APIs and none of it is on our servers

[tl]: https://chitter.xyz/api/v1/timelines/public?local=true

`updates.html` can be updated by hand with `rclone` or in the AWS console.
there used to be a barebones form to update `updates.html` at <https://update.status.chitter.xyz/> but the machine it was on was decommissioned.
