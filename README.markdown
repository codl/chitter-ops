# chitter-ops

this repository documents non-obvious parts of chitter's architecture

## servers

There are two servers involved:

* `boop.chitter.xyz` or `boop`
    * main app server

    * Scaleway C2M in PAR1

    * Paid for by Patreon backers

* `they.codl.fr` or `they`

    * media cache and misc stuff

    * Scaleway C2M in PAR1

    * Paid out of pocket by codl (I have other stuff running on it)

## app

the app server, streaming server, sidekiq, and database are all on `boop`

the app and streaming server are behind a caddy instance, which handles all the TLS stuff. the </Caddyfile> is in this repo

> TODO document sidekiq

> TODO document backups

> TODO document deployment procedure

## media

Media is stored in Backblaze B2, in the `chitter-media` bucket

A minio instance running in docker on `boop` does the bridging between Mastodon's S3 support and B2

Incoming requests to `media.chitter.xyz` go to BunnyCDN, which then goes to a cache server on `they`, which then goes to B2 itself

It might seem silly to have two layers of caching (the CDN and `they`), but B2 has a pretty bad time to first byte, and were it not for the central cache on `they`, each BunnyCDN location would hit B2 separately and we'd end up "paying" the time to first byte once in each location before the resource is cached.

The implementation of the cache server is in </media-cache> in this repository.

### historical media locations

because toots are (indefinitely?) cached on remote servers with whatever media url was appropriate at the time, the following locations should continue serving or redirecting to the canonical location for media:

* `https://media.chitter.xyz/` current canonical URL
* `https://media-b.chitter.xyz/` occasionally used when rotating servers. served by bunnycdn identically to the other one
* `https://chitter-media.codl.fr/` served by `they`, redirects
* `https://chitter.xyz/system/` currently 404s 😕

## status

TODO
