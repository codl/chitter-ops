server {
    server_name tempo.orig.media.chitter.xyz;
    listen 80;
    listen [::]:80;
    return 301 https://tempo.orig.media.chitter.xyz$request_uri;
}

server {
    server_name tempo.orig.media.chitter.xyz;
    listen 443 ssl http2;
    listen [::]:443 ssl http2;
    ssl_certificate /etc/letsencrypt/live/tempo.orig.media.chitter.xyz/fullchain.pem;
    ssl_certificate_key /etc/letsencrypt/live/tempo.orig.media.chitter.xyz/privkey.pem;
    include /etc/letsencrypt/options-ssl-nginx.conf;
    ssl_dhparam /etc/letsencrypt/ssl-dhparams.pem;
    proxy_ssl_server_name on;
    proxy_ssl_verify on;
    proxy_ssl_protocols TLSv1.2 TLSv1.3;
    proxy_intercept_errors on;
    recursive_error_pages on;

    location /.well-known/acme-challenge {
        root /srv/http/acme;
    }

    location = / {
        return 204;
    }

    proxy_hide_header expires;
    proxy_hide_header cache-control;

    location / {
        root /data/chittermedia/chitter-media;
        error_page 404 = /parasect;
        add_header upstream tempo always;
        add_header cache-control "max-age=432000; immutable; public";
        add_header access-control-allow-origin "*";
    }

    location /parasect {
        internal;
        rewrite .* $request_uri break;
        proxy_pass https://chitter-media.parasect.codl.fr;
        error_page 404 = /b2/$request_uri;
        add_header upstream parasect always;
        add_header cache-control "max-age=432000; immutable; public";
        add_header access-control-allow-origin "*";
    }

    location /b2 {
        internal;
        rewrite ^/b2/([^?]*)(\?.*)?$ /file/chitter-media$1 break;
        proxy_pass https://f001.backblazeb2.com;
        add_header upstream b2 always;
        add_header cache-control "max-age=432000; immutable; public";
        add_header access-control-allow-origin "*";
        add_header dollar-uri $uri always;
    }

    proxy_cache chittermedia;
    proxy_ignore_headers cache-control expires;
    proxy_cache_valid 200 30d;
    proxy_cache_key "1:$scheme$proxy_host$request_uri $slice_range";
    slice 1m;
    proxy_set_header Range $slice_range;
    proxy_http_version 1.1;
}

proxy_cache_path /data/ngx-cache/chittermedia
    levels=1:2 max_size=10G keys_zone=chittermedia:16m inactive=5d;


server {
    server_name chitter-media.codl.fr;
    listen 80;
    listen [::]:80;
    listen 443 ssl http2;
    listen [::]:443 ssl http2;
    ssl_certificate /etc/letsencrypt/live/chitter-media.codl.fr/fullchain.pem;
    ssl_certificate_key /etc/letsencrypt/live/chitter-media.codl.fr/privkey.pem;
    include /etc/letsencrypt/options-ssl-nginx.conf;
    ssl_dhparam /etc/letsencrypt/ssl-dhparams.pem;
    location /.well-known/acme-challenge {
        root /srv/http/acme;
    }
    location / {
        return 301 https://media.chitter.xyz$request_uri;
    }
}
