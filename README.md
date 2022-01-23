# ntp_client
Just another NTP client [RFC NTPv3](https://datatracker.ietf.org/doc/html/rfc1305)

To set the date:
```
./ntp_packet es.pool.ntp.org 2>/dev/null | sed 's/^/@/' | xargs sudo date -s
```

# TODO

Add every field of the packet as an argument

Save Last Timestamp and other protocol specific variables.
