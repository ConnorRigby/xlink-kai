# XlinkKai
This is a simple wrapper around the [Xlink Kai Engine](https://www.teamxlink.co.uk/).

# Setup
* Burn the sdcard with the latest [firmware](https://github.com/ConnorRigby/xlink-kai/releases/download/v0.2.0/xlink-rpi3-v0.2.0.img).
* Open the fat32 boot partition.
* Input your network settings.
* Insert sdcard in raspberry pi 3.
* Wait for connect.
* Connect to raspberry pi ip address port 80.

# Configuring
in the `boot` fat32 partition there is a file called `xlink-config.json`
open it up in your favorite text editor. 
If you plan on using ethernet, you can leave it as is. If you are using 
wifi, go ahead and replace `TestSSID` with your SSID and `psk` with your psk.
Currently only wpa psk version one and two are supported.

# Troubleshooting

## rpi never connects?
You probably typed the SSID or psk in wrong.

## something else is broken
  * ¯\\_(ツ)_/¯

# Building from source
Building from source requires a few dependencies. Follow
[this](https://hexdocs.pm/nerves/getting-started.html) guide to get up and
running.

```bash
git clone https://github.com/ConnorRigby/xlink-kai
cd xlink-kai
export MIX_TARET=rpi3 # or whatever device you are using.
mix deps.get
mix firmware
mix burn
```

## Disclaimer
Anything bad that happens to you is not my fault.
