vcl 4.0;

backend b2_f001 {
    .host = "proxy";
    .port = "80";
}

sub vcl_recv {
    if (req.url ~ "/$"){
        set req.url = req.url + "index.html";
    }
}

sub vcl_backend_fetch {
    unset bereq.http.cookie;
    unset bereq.http.host;

    #set bereq.url = "/file/chitter-media" + bereq.url;
}

sub vcl_backend_response {
    set beresp.http.x-from = bereq.backend;

    if (beresp.status != 200) {
        return(pass);
    }
    else {
        set beresp.ttl = 5d;
        set beresp.keep = 7d;
        set beresp.grace = 0s;
        set beresp.http.cache-control = "max-age=432000; immutable; public";
    }
}

sub vcl_deliver {
    set resp.http.access-control-allow-origin = "*";
    set resp.http.access-control-max-age = "432000";

    if (req.url == "/404.html"){
        set resp.status = 404;
        set resp.http.cache-control = "max-age=0";
        return(deliver);
    }
    elseif (resp.status >= 400 && resp.status < 500) {
        set req.url = "/404.html";
    }
}
