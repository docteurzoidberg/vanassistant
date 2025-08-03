# serial test gui for VanAssistant

Let send and receive J7 serial packets


## Usage

```bash
  cd tools/
  ./serialgui --serial /dev/pts/7 --bauds=115200
```

## Loopback setup for local testing

Before testing:

```bash
  socat -d -d pty,raw,echo=0 pty,raw,echo=0 &
```

After:

```bash
  sudo killall socat
```

