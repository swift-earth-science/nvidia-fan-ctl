# nvidia-fan-ctl

NVIDIA Fan Control Tool that uses NVIDIA's NVML library for direct hardware access without requiring X11 or nvidia-settings. It will work in background sessions and as a service.

# Build

```bash
# create symlink to libnvidia-ml.so.1
sudo ln -s /usr/lib/x86_64-linux-gnu/libnvidia-ml.so.1 /usr/lib/x86_64-linux-gnu/libnvidia-ml.so

# build
./build.sh
```

# Install

Copy to `/usr/local/bin`:

```bash
sudo cp -f ./gpu-fan-control /usr/local/bin/gpu-fan-control
```

Create service file:

```bash
sudo nano /etc/systemd/system/gpu-fan-control.service
```

Copy this config in it:

```bash
[Unit]
Description=GPU Fan Control Service
After=nvidia-persistenced.service
Wants=nvidia-persistenced.service

[Service]
Type=simple
ExecStart=/usr/local/bin/gpu-fan-control
Restart=always
RestartSec=3
User=root

[Install]
WantedBy=multi-user.target
```

Enable and start the service:

```bash
sudo systemctl daemon-reload
sudo systemctl enable gpu-fan-control
sudo systemctl start gpu-fan-control
```

Check status:

```bash
sudo systemctl status gpu-fan-control
sudo journalctl -u gpu-fan-control -f
```
