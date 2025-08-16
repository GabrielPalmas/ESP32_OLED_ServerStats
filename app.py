from fastapi import FastAPI
import httpx
import psutil

app = FastAPI()

# Replace with your Netdata or monitoring server URL
NETDATA_URL = "http://<NETDATA_SERVER_IP>:19999/api/v1/data"

async def fetch_chart(client, chart: str):
    url = f"{NETDATA_URL}?chart={chart}"
    resp = await client.get(url)
    resp.raise_for_status()
    return resp.json()

def bytes_to_gb(n_bytes):
    """Convert bytes to GB with 2 decimals."""
    return round(n_bytes / (1024 ** 3), 2)

def bytes_to_human(n_bytes):
    """Convert bytes to KB, MB, GB, etc."""
    for unit in ['B', 'KB', 'MB', 'GB', 'TB']:
        if n_bytes < 1024:
            return f"{n_bytes:.1f} {unit}"
        n_bytes /= 1024
    return f"{n_bytes:.1f} PB"

@app.get("/espdata")
async def espdata():
    async with httpx.AsyncClient(timeout=5.0) as client:
        # --- CPU ---
        cpu_chart = await fetch_chart(client, "system.cpu")
        latest_cpu = cpu_chart["data"][0]
        cpu_total = round(latest_cpu[6] + latest_cpu[7], 1)  # user + system

        # --- RAM ---
        ram_chart = await fetch_chart(client, "system.ram")
        ram_data_points = ram_chart["data"][:5]  # average of last 5 points

        used_list = []
        for dp in ram_data_points:
            free = dp[1] * 1024 * 1024
            used = dp[2] * 1024 * 1024
            cached = dp[3] * 1024 * 1024
            buffers = dp[4] * 1024 * 1024

            actual_used = used  # already excludes buffers/cached
            used_list.append(actual_used)

        actual_used_bytes = sum(used_list) / len(used_list)
        total_bytes = free + used + cached + buffers
        ram_percent = round((actual_used_bytes / total_bytes) * 100, 1)

        # --- Network ---
        net_chart = await fetch_chart(client, "system.net")
        latest_net = net_chart["data"][0]
        net_in = latest_net[1]
        net_out = latest_net[2]

    return {
        "cpu_total": f"{cpu_total}%",
        "ram_used": f"{bytes_to_gb(actual_used_bytes)} GB",
        "ram_total": f"{bytes_to_gb(total_bytes)} GB",
        "ram_percent": f"{ram_percent}%",
        "net_in": bytes_to_human(net_in) + "/s",
        "net_out": bytes_to_human(abs(net_out)) + "/s"
    }
